// Copyrisnapshotter.ccght 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SRC_UI_LIB_ESCHER_RENDERER_BATCH_GPU_DOWNLOADER_H_
#define SRC_UI_LIB_ESCHER_RENDERER_BATCH_GPU_DOWNLOADER_H_

#include <lib/fit/function.h>

#include <variant>

#include "src/ui/lib/escher/escher.h"
#include "src/ui/lib/escher/renderer/buffer_cache.h"
#include "src/ui/lib/escher/renderer/frame.h"
#include "src/ui/lib/escher/third_party/granite/vk/command_buffer.h"
#include "src/ui/lib/escher/vk/buffer.h"
#include "src/ui/lib/escher/vk/command_buffer.h"

#include <vulkan/vulkan.hpp>

namespace escher {

// Provides host-accessible GPU memory for clients to download Images and
// Buffers from the GPU to host memory. Offers the ability to batch downloads
// into consolidated submissions to the GPU driver.
//
// About synchronization:
//   We use semaphore (usually generated by ChainedSemaphoreGenerator) for
// synchronization between BatchGpuDownloader/Uploader and other gfx components,
// so we only need the barrier to synchronize all transfer-related commands.
//
//   Currently users of BatchGpuDownloader should manually enforce that
// the BatchGpuDownloader waits on other BatchGpuUploader or gfx::Engine if they
// write to the images / buffers the BatchGpuDownloader reads from, by using
// AddWaitSemaphore() function. Also, Submit() function will return a semaphore
// being signaled when command buffer finishes execution, which can be used for
// synchronization.
//
// TODO(SCN-1197) Add memory barriers so the BatchGpuUploader and
// BatchGpuDownloader can handle synchronzation of reads and writes on the same
// Resource.
//
class BatchGpuDownloader {
 public:
  // Called after download is complete, with a pointer into the downloaded data.
  using Callback = fit::function<void(const void* host_ptr, size_t size)>;

 public:
  static std::unique_ptr<BatchGpuDownloader> New(
      EscherWeakPtr weak_escher,
      CommandBuffer::Type command_buffer_type = CommandBuffer::Type::kTransfer,
      uint64_t frame_trace_number = 0);

  BatchGpuDownloader(EscherWeakPtr weak_escher,
                     CommandBuffer::Type command_buffer_type = CommandBuffer::Type::kTransfer,
                     uint64_t frame_trace_number = 0);
  ~BatchGpuDownloader();

  // Returns true if the BatchGpuDownloader has work to do on the GPU.
  bool HasContentToDownload() const { return !copy_info_records_.empty(); }

  // Returns true if BatchGpuDownloader needs a command buffer, i.e. it needs to
  // downloading images/buffers, or it needs to wait on/signal semaphores.
  bool NeedsCommandBuffer() const {
    return HasContentToDownload() || !wait_semaphores_.empty() || !signal_semaphores_.empty();
  }

  // Schedule a buffer-to-buffer copy that will be submitted when Submit()
  // is called.  Retains a reference to the source until the submission's
  // CommandBuffer is retired.
  //
  // |source_offset| is the starting offset in bytes from the start of the
  //   source buffer.
  // |copy_size| is the size to be copied to the buffer.
  //
  // These arguments are used to build VkBufferCopy struct.  See the Vulkan
  // specs of VkBufferCopy for more details. If copy_size is set to 0 by
  // default, it copies all the contents from source.
  void ScheduleReadBuffer(const BufferPtr& source, Callback callback,
                          vk::DeviceSize source_offset = 0U, vk::DeviceSize copy_size = 0U);

  // Schedule a image-to-buffer copy that will be submitted when Submit()
  // is called.  Retains a reference to the source until the submission's
  // CommandBuffer is retired.
  //
  // |region| specifies the buffer region which will be copied to the target
  // image.
  //   |region.bufferOffset| should be set to zero since target buffer is
  //   managed internally by the uploader; currently |imageOffset| requires to
  //   be zero and |imageExtent| requires to be the whole image.
  // The default value of |region| is vk::BufferImageCopy(), in which case we
  // create a default copy region which reads the color data from the whole
  // image with only one mipmap layer.
  //
  // |write_function| is a callback function which will be called at
  // GenerateCommands(), where we copy our data to the host-visible buffer.
  void ScheduleReadImage(const ImagePtr& source, Callback callback,
                         vk::BufferImageCopy region = vk::BufferImageCopy());

  // Submits all pending work to the given CommandBuffer. Users need to call
  // cmds->Submit() after calling this function. Returns a lambda function
  // which calls all the callback functions we passed in to ScheduleReadBuffer
  // and ScheduleReadImage functions; this function should be called after the
  // command buffer has finished execution, typically by passing it to
  // cmds->Submit().
  //
  // After this function is called, all the contents in |copy_info_records|,
  // staged |resources_| and semaphores will be moved to the lambda function
  // and the downloader will be cleaned and ready for reuse.
  //
  // The argument |cmds| cannot be nullptr if there is any pending work,
  // including writing to buffer/images and waiting on/signaling semaphores.
  CommandBufferFinishedCallback GenerateCommands(CommandBuffer* cmds);

  // Submits all pending work to the GPU.
  // The function |callback|, and all callback functions the user passed to
  // ScheduleReadBuffer and ScheduleReadImage, will be called after all work
  // is done.
  //
  // After this function is called, all the contents in |copy_info_records|,
  // staged |resources_| and semaphores will be moved to the lambda function
  // and the downloader will be cleaned and ready for reuse.
  void Submit(CommandBufferFinishedCallback client_callback = nullptr);

  // Submit() and GenerateCommands() will wait on all semaphores added by
  // AddWaitSemaphore().
  void AddWaitSemaphore(SemaphorePtr sema, vk::PipelineStageFlags flags) {
    wait_semaphores_.push_back({std::move(sema), flags});
  }

  // Submit() and GenerateCommands() will signals all semaphores added by
  // AddSignalSemaphore().
  void AddSignalSemaphore(SemaphorePtr sema) { signal_semaphores_.push_back(std::move(sema)); }

 private:
  enum class CopyType { COPY_IMAGE = 0, COPY_BUFFER = 1 };
  struct ImageCopyInfo {
    ImagePtr source;
    vk::BufferImageCopy region;
  };
  struct BufferCopyInfo {
    BufferPtr source;
    vk::BufferCopy region;
  };
  using CopyInfoVariant = std::variant<ImageCopyInfo, BufferCopyInfo>;
  struct CopyInfo {
    CopyType type;
    vk::DeviceSize offset;
    vk::DeviceSize size;
    Callback callback;
    // copy_info can either be a ImageCopyInfo or a BufferCopyInfo.
    CopyInfoVariant copy_info;
  };

  EscherWeakPtr escher_;

  CommandBuffer::Type command_buffer_type_ = CommandBuffer::Type::kTransfer;
  // The trace number for the frame. Cached to support lazy frame creation.
  const uint64_t frame_trace_number_;
  BufferCacheWeakPtr buffer_cache_;

  vk::DeviceSize current_offset_ = 0U;

  std::vector<CopyInfo> copy_info_records_;
  std::vector<ResourcePtr> resources_;

  std::vector<std::pair<SemaphorePtr, vk::PipelineStageFlags>> wait_semaphores_;
  std::vector<SemaphorePtr> signal_semaphores_;

  FXL_DISALLOW_COPY_AND_ASSIGN(BatchGpuDownloader);
};

}  // namespace escher

#endif  // SRC_UI_LIB_ESCHER_RENDERER_BATCH_GPU_DOWNLOADER_H_
