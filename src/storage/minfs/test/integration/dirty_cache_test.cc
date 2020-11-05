// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fcntl.h>
#include <fuchsia/minfs/c/fidl.h>
#include <fuchsia/minfs/llcpp/fidl.h>
#include <lib/fdio/cpp/caller.h>
#include <unistd.h>
#include <zircon/errors.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include <fbl/unique_fd.h>

#include "src/storage/fs_test/fs_test.h"
#include "src/storage/fs_test/fs_test_fixture.h"
#include "src/storage/fs_test/minfs_test.h"
#include "src/storage/minfs/format.h"

namespace minfs {
namespace {

using DirtyCacheTest = fs_test::FilesystemTest;

void EnableStats(DirtyCacheTest* fs) {
  fbl::unique_fd dir_fd(open(fs->fs().mount_path().c_str(), O_RDONLY | O_DIRECTORY));
  EXPECT_TRUE(dir_fd);

  fdio_cpp::FdioCaller caller(dir_fd.duplicate());
  zx_status_t status;
  EXPECT_EQ(fuchsia_minfs_MinfsToggleMetrics(caller.borrow_channel(), true, &status), ZX_OK);
  EXPECT_EQ(status, ZX_OK);
}

class BufferedFile {
 public:
  BufferedFile(DirtyCacheTest* fs, std::string path, size_t max_size, size_t bytes_to_write,
               size_t bytes_per_write) {
    fs_ = fs;
    bytes_per_write_ = bytes_per_write;
    std::cerr << path << max_size << bytes_to_write << bytes_per_write_ << std::endl;
    file_path_ = path;
    fd_.reset(open(file_path_.c_str(), O_RDWR | O_CREAT));
    EXPECT_TRUE(fd_);
    buffer_.resize(max_size, 0);

    // Fill buffer with "random" data.
    for (size_t i = 0; i < max_size; i++) {
      buffer_[i] = static_cast<uint8_t>(i);
    }

    for (size_t i = 0; i < bytes_to_write; i += bytes_per_write) {
      if ((i + bytes_per_write) > bytes_to_write) {
        bytes_per_write = bytes_to_write % bytes_per_write_;
      }
      Write(bytes_per_write, i);
    }
  }

  ~BufferedFile() {
    // Verify open fd.
    Verify(expected_file_size_);

    // Verify close/reopened file.
    Reopen();
    Verify(expected_file_size_);

    // Verify file after unmount and mount.
    RemountAndReopen();
    Verify(expected_file_size_);
  }

  void Reopen() {
    fd_.reset(open(file_path_.c_str(), O_RDWR));
    ASSERT_TRUE(fd_);
  }

  void Write(size_t bytes, std::optional<off_t> offset = std::nullopt) {
    if (!offset) {
      offset = lseek(fd_.get(), 0, SEEK_CUR);
    }
    ASSERT_TRUE(offset.value() < static_cast<off_t>(buffer_.size()));
    ASSERT_TRUE((offset.value() + bytes) <= buffer_.size());
    ASSERT_EQ(lseek(fd_.get(), offset.value(), SEEK_SET), offset.value());
    ASSERT_EQ(write(fd_.get(), buffer_.data() + offset.value(), bytes), static_cast<off_t>(bytes));
    if (expected_file_size_ < (offset.value() + bytes)) {
      expected_file_size_ = offset.value() + bytes;
    }
  }

  void RemountAndReopen() {
    ASSERT_TRUE(fs_->fs().Unmount().is_ok());
    ASSERT_TRUE(fs_->fs().Mount().is_ok());
    EnableStats(fs_);
    Reopen();
  }

 private:
  // Verifies file size and contents.
  void Verify(size_t expected_size) {
    struct stat stats;
    ASSERT_EQ(fstat(fd_.get(), &stats), 0);
    ASSERT_EQ(stats.st_size, static_cast<off_t>(expected_size));
    std::unique_ptr<uint8_t[]> read_buffer(new uint8_t[expected_size]);

    ASSERT_EQ(pread(fd_.get(), read_buffer.get(), expected_size, 0),
              static_cast<ssize_t>(expected_size));
    ASSERT_EQ(std::memcmp(buffer_.data(), read_buffer.get(), expected_size), 0);
  }

  // Pointer to the test to help unmount and remount.
  DirtyCacheTest* fs_ = nullptr;

  // Absolute path of the file.
  std::string file_path_;

  // File's open handle.
  fbl::unique_fd fd_;

  // File's content. Only |expected_file_size_| bytes of this are valid.
  std::vector<uint8_t> buffer_;

  // The max write size at a time.
  size_t bytes_per_write_;

  // The current expected size of the file.
  size_t expected_file_size_ = 0;
};

void CheckDirtyStats(std::string mount_path, uint64_t dirty_bytes) {
  fbl::unique_fd fd(open(mount_path.c_str(), O_RDONLY | O_DIRECTORY));
  ASSERT_TRUE(fd);

  fdio_cpp::FdioCaller caller(std::move(fd));
  auto result = llcpp::fuchsia::minfs::Minfs::Call::GetMetrics(caller.channel());
  ASSERT_TRUE(result.ok());
  ASSERT_EQ(result.value().status, ZX_OK);
  ASSERT_NE(result.value().metrics, nullptr);
  auto metrics = *result.value().metrics;
  ASSERT_EQ(metrics.dirty_bytes, dirty_bytes);
}

BufferedFile EnableStatsAndCreateFile(DirtyCacheTest* fs, size_t file_max_size, int bytes_to_write,
                                      int bytes_per_write, std::string file_name = "foo") {
  EnableStats(fs);
  return BufferedFile(fs, fs->fs().mount_path() + file_name, file_max_size, bytes_to_write,
                      bytes_per_write);
}

TEST_P(DirtyCacheTest, CleanlyMountedFs) {
  { auto file = EnableStatsAndCreateFile(this, 0, 0, 0); }
  CheckDirtyStats(fs().mount_path(), 0);
}

TEST_P(DirtyCacheTest, DirtyBytesAfterWrite) {
  constexpr size_t kFileMaxSize = kMinfsBlockSize;
  auto file = EnableStatsAndCreateFile(this, kFileMaxSize, kFileMaxSize, kFileMaxSize);
  CheckDirtyStats(fs().mount_path(), kFileMaxSize);
}

TEST_P(DirtyCacheTest, NoDirtyByteAfterClose) {
  {
    constexpr size_t kFileMaxSize = kMinfsBlockSize;
    auto file = EnableStatsAndCreateFile(this, kFileMaxSize, kFileMaxSize, kFileMaxSize);
    CheckDirtyStats(fs().mount_path(), kFileMaxSize);
  }
  CheckDirtyStats(fs().mount_path(), 0);
}

TEST_P(DirtyCacheTest, UnmountFlushedPendingWrites) {
  constexpr size_t kFileMaxSize = kMinfsBlockSize;
  auto file = EnableStatsAndCreateFile(this, kFileMaxSize, kFileMaxSize, kFileMaxSize);
  CheckDirtyStats(fs().mount_path(), kFileMaxSize);
  file.RemountAndReopen();
}

TEST_P(DirtyCacheTest, MultipleByteWriteToSameBlockKeepsDirtyBytesTheSame) {
  constexpr size_t kFileMaxSize = kMinfsBlockSize;
  constexpr size_t kBytesPerWrite = 10;
  {
    auto file = EnableStatsAndCreateFile(this, kFileMaxSize, kFileMaxSize, kBytesPerWrite);
    CheckDirtyStats(fs().mount_path(), minfs::kMinfsBlockSize);
  }
  CheckDirtyStats(fs().mount_path(), 0);
}

TEST_P(DirtyCacheTest, MultipleBlocWriteToSameOffsetKeepsDirtyBytesTheSame) {
  {
    constexpr size_t kBytesPerWrite = kMinfsBlockSize;
    constexpr size_t kBytesToWrite = kBytesPerWrite;
    constexpr size_t kFileMaxSize = kBytesToWrite;
    auto file = EnableStatsAndCreateFile(this, kFileMaxSize, kBytesToWrite, kBytesPerWrite);
    CheckDirtyStats(fs().mount_path(), kBytesPerWrite);
    for (int i = 0; i < 10; i++) {
      file.Write(kBytesPerWrite, 0);
      CheckDirtyStats(fs().mount_path(), kBytesPerWrite);
    }
    CheckDirtyStats(fs().mount_path(), kBytesPerWrite);
  }
  CheckDirtyStats(fs().mount_path(), 0);
}

TEST_P(DirtyCacheTest, MultipleBlocWritesMakesMultipleBlocksDirty) {
  constexpr size_t kBytesPerWrite = kMinfsBlockSize;
  constexpr size_t kBytesToWrite = 2 * kBytesPerWrite;
  constexpr size_t kFileMaxSize = kBytesToWrite;
  {
    auto file = EnableStatsAndCreateFile(this, kFileMaxSize, kBytesToWrite, kBytesPerWrite);
    CheckDirtyStats(fs().mount_path(), kBytesToWrite);
  }
  CheckDirtyStats(fs().mount_path(), 0);
}

}  // namespace
INSTANTIATE_TEST_SUITE_P(/*no prefix*/, DirtyCacheTest, testing::ValuesIn(fs_test::AllTestMinfs()),
                         testing::PrintToStringParamName());
}  // namespace minfs
