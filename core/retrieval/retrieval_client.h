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

#ifndef PYREC_CORE_RETRIEVAL_RETRIEVAL_CLIENT_H_
#define PYREC_CORE_RETRIEVAL_RETRIEVAL_CLIENT_H_

#include <memory>

#include "grpcpp/grpcpp.h"

#include "pyrec/proto/recommend.hlrpc.pb.h"
#include "core/util/ip.h"
#include "core/retrieval/retrieval_interface.h"

namespace pyrec {
namespace service {

class RetrievalClientInternal {
 public:
  virtual ~RetrievalClientInternal() {}
  virtual pyrec::Status CallRetrieval(
      const pyrec::service::PyRecRequest& request,
      pyrec::service::ItemReply* reply) = 0;
};

class LocalRetrievalClientInternal : public RetrievalClientInternal {
 public:
  explicit LocalRetrievalClientInternal(
      const pyrec::service::RetrievalServiceInterface& server_interface)
      : server_(server_interface.GetServer()) {}
  explicit LocalRetrievalClientInternal(
      std::shared_ptr<pyrec::service::RetrievalServiceHybridBase> server)
      : server_(server) {}

  pyrec::Status CallRetrieval(
      const pyrec::service::PyRecRequest& request,
      pyrec::service::ItemReply* reply) override {
    return server_->RetrievalLocal(&request, reply);
  }

 private:
  std::shared_ptr<RetrievalServiceHybridBase> server_;
};

class RemoteRetrievalClientInternal : public RetrievalClientInternal {
 public:
  RemoteRetrievalClientInternal(const char* ip, int port)
      : remote_address_({ip, port}) {
    Connect();
  }
  explicit RemoteRetrievalClientInternal(const pyrec::Address& remote_address)
      : remote_address_(remote_address) {
    Connect();
  }

  ~RemoteRetrievalClientInternal() {
    Disconnect();
  }

  pyrec::Status CallRetrieval(
      const pyrec::service::PyRecRequest& request,
      pyrec::service::ItemReply* reply) override {
    if (!Ping())
      return pyrec::Status::CANCELLED;

    grpc::ClientContext context;
    std::unique_ptr<RetrievalService::Stub> stub(
        RetrievalService::NewStub(channel_));
    grpc::Status status = stub->Retrieval(&context, request, reply);
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

#endif  // PYREC_CORE_RETRIEVAL_RETRIEVAL_CLIENT_H_
