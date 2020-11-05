// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/storage/minfs/minfs_private.h"

namespace minfs {

[[nodiscard]] bool Minfs::IsErrored() { return false; }

std::vector<fbl::RefPtr<VnodeMinfs>> Minfs::GetDirtyVnodes() {
  std::vector<fbl::RefPtr<VnodeMinfs>> vnodes;
  return vnodes;
}

zx_status_t Minfs::ContinueTransaction(size_t reserve_blocks,
                                       std::unique_ptr<CachedTransaction> cached_transaction,
                                       std::unique_ptr<Transaction>* out) {
  // Reserve blocks from allocators before returning WritebackWork to client.
  *out = Transaction::FromCachedTransaction(this, std::move(cached_transaction));
  return (*out)->ExtendBlockReservation(reserve_blocks);
}

zx::status<> Minfs::AddDirtyBytes(uint64_t dirty_bytes, bool allocated) { return zx::ok(); }

void Minfs::RemoveDirtyBytes(uint64_t dirty_bytes, bool allocated) {}

}  // namespace minfs
