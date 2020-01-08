// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/developer/debug/zxdb/console/format_frame.h"

#include <inttypes.h>

#include "src/developer/debug/zxdb/client/frame.h"
#include "src/developer/debug/zxdb/client/process.h"
#include "src/developer/debug/zxdb/client/thread.h"
#include "src/developer/debug/zxdb/console/command_utils.h"
#include "src/developer/debug/zxdb/console/console.h"
#include "src/developer/debug/zxdb/console/format_location.h"
#include "src/developer/debug/zxdb/console/format_node_console.h"
#include "src/developer/debug/zxdb/console/output_buffer.h"
#include "src/developer/debug/zxdb/console/string_util.h"
#include "src/developer/debug/zxdb/symbols/function.h"
#include "src/developer/debug/zxdb/symbols/location.h"
#include "src/developer/debug/zxdb/symbols/value.h"
#include "src/lib/fxl/logging.h"
#include "src/lib/fxl/strings/string_printf.h"

namespace zxdb {

namespace {

fxl::RefPtr<AsyncOutputBuffer> ListCompletedFrames(Thread* thread, const FormatStackOptions& opts) {
  int active_frame_id = Console::get()->context().GetActiveFrameIdForThread(thread);

  auto out = fxl::MakeRefCounted<AsyncOutputBuffer>();

  // This doesn't use table output since the format of the stack frames is usually so unpredictable.
  const Stack& stack = thread->GetStack();
  if (stack.empty()) {
    if (thread->GetState() != debug_ipc::ThreadRecord::State::kSuspended &&
        !(thread->GetState() == debug_ipc::ThreadRecord::State::kBlocked &&
          thread->GetBlockedReason() == debug_ipc::ThreadRecord::BlockedReason::kException)) {
      // Make a nicer error message for the common case of requesting stack frames when the thread
      // is in the wrong state.
      out->Append(
          "Stack frames are only available when the thread is either suspended "
          "or blocked\nin an exception. Use \"pause\" to suspend it.");
    } else {
      out->Append("No stack frames.\n");
    }
    out->Complete();
    return out;
  }

  for (int i = 0; i < static_cast<int>(stack.size()); i++) {
    if (i == active_frame_id)
      out->Append(GetCurrentRowMarker() + " ");
    else
      out->Append("  ");

    out->Append(OutputBuffer(Syntax::kSpecial, fxl::StringPrintf("%d ", i)));

    // Supply "-1" for the frame index to suppress printing (we already did it above).
    out->Append(FormatFrame(stack[i], opts.frame, -1));
    out->Append("\n");
  }

  out->Complete();
  return out;
}

}  // namespace

fxl::RefPtr<AsyncOutputBuffer> FormatStack(Thread* thread, bool force_update,
                                           const FormatStackOptions& opts) {
  auto out = fxl::MakeRefCounted<AsyncOutputBuffer>();
  if (!force_update && thread->GetStack().has_all_frames()) {
    out->Complete(ListCompletedFrames(thread, opts));
    return out;
  }

  // Request a stack update.
  thread->GetStack().SyncFrames([thread = thread->GetWeakPtr(), opts, out](const Err& err) {
    if (!err.has_error() && thread)
      out->Complete(ListCompletedFrames(thread.get(), opts));
    else
      out->Complete("Thread exited, no frames.\n");
  });
  return out;
}

fxl::RefPtr<AsyncOutputBuffer> FormatFrame(const Frame* frame, const FormatFrameOptions& opts,
                                           int id) {
  auto out = fxl::MakeRefCounted<AsyncOutputBuffer>();

  if (id >= 0)
    out->Append(fxl::StringPrintf("Frame %d ", id));

  const Location& location = frame->GetLocation();
  out->Append(FormatLocation(location, opts.loc));

  if (frame->IsInline())
    out->Append(Syntax::kComment, " (inline)");

  // IP address and stack pointers.
  if (opts.detail == FormatFrameOptions::kVerbose) {
    out->Append(Syntax::kComment, fxl::StringPrintf("\n      IP = 0x%" PRIx64 ", SP = 0x%" PRIx64,
                                                    frame->GetAddress(), frame->GetStackPointer()));

    // TODO(brettw) make this work when the frame base is asynchronous.
    if (auto bp = frame->GetBasePointer())
      out->Append(Syntax::kComment, fxl::StringPrintf(", base = 0x%" PRIx64, *bp));
  }

  if (opts.detail != FormatFrameOptions::kSimple && location.symbol()) {
    const Function* func = location.symbol().Get()->AsFunction();
    if (func) {
      // Always list function parameters in the order specified.
      for (const auto& param : func->parameters()) {
        const Variable* value = param.Get()->AsVariable();
        if (!value)
          continue;  // Symbols are corrupt.

        out->Append("\n      ");  // Indent.
        out->Append(FormatVariableForConsole(value, opts.variable, frame->GetEvalContext()));
      }
    }
  }

  out->Complete();
  return out;
}

}  // namespace zxdb
