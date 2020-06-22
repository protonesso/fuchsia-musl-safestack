// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SRC_DEVELOPER_FEEDBACK_FEEDBACK_DATA_SYSTEM_LOG_RECORDER_READER_H_
#define SRC_DEVELOPER_FEEDBACK_FEEDBACK_DATA_SYSTEM_LOG_RECORDER_READER_H_

#include <string>
#include <vector>

#include "src/developer/feedback/feedback_data/system_log_recorder/encoding/decoder.h"

namespace feedback {

bool Concatenate(const std::vector<const std::string>& input_file_paths, Decoder* decoder,
                 const std::string& output_file_path);

}  // namespace feedback

#endif  // SRC_DEVELOPER_FEEDBACK_FEEDBACK_DATA_SYSTEM_LOG_RECORDER_READER_H_
