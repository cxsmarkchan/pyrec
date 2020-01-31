/*
 * Copyright 2020 The PyRec Authors. All Rights Reserved.
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

#ifndef PYREC_CORE_SERVICE_BASE_H_
#define PYREC_CORE_SERVICE_BASE_H_

#include <iostream>
#include <memory>

#include "boost/format.hpp"

#include "pyrec/proto/recommend.grpc.pb.h"
#include "pyrec/core/util/status.h"
#include "pyrec/core/util/ip.h"

namespace pyrec {
namespace service {

template<class GrpcService, class Request, class Reply>
class BaseServer : public GrpcService::Service {
 public:
  grpc::Status OnServing(grpc::ServerContext* context,
                         const Request* request,
                         Reply* reply) final {
    pyrec::Status status = ServingProcess(request, reply);
    return status.ToGrpcStatus();
  }

  pyrec::Status OnLocalServing(const Request* request,
                               Reply* reply) {
    return ServingProcess(request, reply);
  }

  virtual pyrec::Status ServingProcess(const Request* request, Reply* reply) {
    return pyrec::Status::UNIMPLEMENTED;
  }

  int Run(const Address& address) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address.ToString(),
                             grpc::InsecureServerCredentials());
    builder.RegisterService(this);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
  }
};

template<class Server>
class ServerWrapper {
 public:
  int Run(const char* ip, int port) {
    return server_.Run({ip, port});
  }
 private:
  Server server_;
};

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_SERVICE_BASE_H_
