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

#ifndef PYREC_CORE_CONTEXT_CONTEXT_CLIENT_H_
#define PYREC_CORE_CONTEXT_CONTEXT_CLIENT_H_

#include <memory>

#include "grpcpp/grpcpp.h"

#include "pyrec/proto/recommend.hlrpc.pb.h"
#include "core/util/ip.h"
#include "core/context/context_interface.h"

namespace pyrec {
namespace service {

class ContextClientInternal {
 public:
  virtual ~ContextClientInternal() {}
  virtual pyrec::Status CallContext(
      const pyrec::service::PyRecRequest& request,
      pyrec::service::ContextReply* reply) = 0;
};

class LocalContextClientInternal : public ContextClientInternal {
 public:
  explicit LocalContextClientInternal(
      const pyrec::service::ContextServiceInterface& server_interface)
      : server_(server_interface.GetServer()) {}
  explicit LocalContextClientInternal(
      std::shared_ptr<pyrec::service::ContextServiceHybridBase> server)
      : server_(server) {}

  pyrec::Status CallContext(
      const pyrec::service::PyRecRequest& request,
      pyrec::service::ContextReply* reply) override {
    return server_->ContextLocal(&request, reply);
  }

 private:
  std::shared_ptr<ContextServiceHybridBase> server_;
};

class RemoteContextClientInternal : public ContextClientInternal {
 public:
  RemoteContextClientInternal(const char* ip, int port)
      : remote_address_({ip, port}) {
    Connect();
  }
  explicit RemoteContextClientInternal(const pyrec::Address remote_address)
      : remote_address_(remote_address) {
    Connect();
  }

  ~RemoteContextClientInternal() {
    Disconnect();
  }

  pyrec::Status CallContext(
      const pyrec::service::PyRecRequest& request,
      pyrec::service::ContextReply* reply) override {
    if (!Ping())
      return pyrec::Status::CANCELLED;

    grpc::ClientContext context;
    std::unique_ptr<ContextService::Stub> stub(
        ContextService::NewStub(channel_));
    grpc::Status status = stub->Context(&context, request, reply);
    return pyrec::Status(status);
  }

 private:
  bool IsConnected() const {
    return channel_ && channel_->GetState(true) == GRPC_CHANNEL_READY;
  }

  bool Connect() {
    channel_ = grpc::CreateChannel(
        remote_address_.ToString(),
        grpc::InsecureChannelCredentials());
    return static_cast<bool>(channel_);
  }

  void Disconnect() {
    if (channel_)
      channel_.reset();
  }

  bool Ping() {
    if (IsConnected())
      return true;
    if (Connect())
      return true;
    return false;
  }

 private:
  pyrec::Address remote_address_;
  std::shared_ptr<grpc::Channel> channel_;
};

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_CONTEXT_CONTEXT_CLIENT_H_
