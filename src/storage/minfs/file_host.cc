// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <lib/zx/status.h>

#include <memory>

#include "src/storage/minfs/file.h"
#include "src/storage/minfs/minfs_private.h"

namespace minfs {

File::~File() = default;

bool File::CacheDirtyPages() const { return false; }

bool File::IsDirty() const { return false; }

zx::status<> File::WalkFileBlocks(size_t offset, size_t length,
                                  WalkWriteBlockHandlerType& handler) {
  return zx::ok();
}

zx::status<uint32_t> File::GetRequiredBlockCountForDirtyCache(size_t offset, size_t length,
                                                              uint32_t uncached_block_count) {
  return zx::ok(uncached_block_count);
}

zx::status<> File::MarkRequiredBlocksPending(size_t offset, size_t length) { return zx::ok(); }

zx::status<> File::FlushCachedWrites() { return zx::ok(); }
void File::DropCachedWrites() {}

zx::status<bool> File::TriggerFlush(bool is_truncate, size_t length, size_t offset) {
  return zx::ok(true);
}

zx::status<> File::FlushTransaction(std::unique_ptr<Transaction> transaction, bool force) {
  return ForceFlushTransaction(std::move(transaction));
}

zx::status<> File::ForceFlushTransaction(std::unique_ptr<Transaction> transaction) {
  // Ensure this Vnode remains alive while it has an operation in-flight.
  transaction->PinVnode(fbl::RefPtr(this));
  InodeSync(transaction.get(), kMxFsSyncMtime);  // Successful writes updates mtime
  Vfs()->CommitTransaction(std::move(transaction));
  return zx::ok();
}

}  // namespace minfs
