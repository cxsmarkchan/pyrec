/*
 * Copyright 2020 cxsmarkchan. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PYREC_CORE_UTIL_STATUS_H_
#define PYREC_CORE_UTIL_STATUS_H_

#include <string>

#include "grpcpp/grpcpp.h"

namespace pyrec {

enum StatusCode {
  OK = 0,
  CANCELLED = 1,
  UNKNOWN = 2,
  UNIMPLEMENTED = 12
};

class Status {
 public:
  Status() : code_(StatusCode::OK), error_message_("") {}
  Status(StatusCode code, std::string error_message) :
      code_(code),
      error_message_(error_message) {}
  explicit Status(grpc::Status grpc_status) :
      code_(static_cast<pyrec::StatusCode>(grpc_status.error_code())),
      error_message_(grpc_status.error_message()) {}

  grpc::Status ToGrpcStatus() {
    return grpc::Status(grpc::StatusCode(code_), grpc::string(error_message_));
  }

  static const Status& OK;
  static const Status& CANCELLED;
  static const Status& UNIMPLEMENTED;

  bool ok() { return code_ == StatusCode::OK; }

 private:
  StatusCode code_;
  std::string error_message_;
};

}  // namespace pyrec

#endif  // PYREC_CORE_UTIL_STATUS_H_
