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

#ifndef PYREC_CORE_INDEXER_INDEXER_CLIENT_H_
#define PYREC_CORE_INDEXER_INDEXER_CLIENT_H_

#include <memory>

#include "grpcpp/grpcpp.h"

#include "pyrec/core/util/ip.h"
#include "pyrec/proto/indexer.hlrpc.pb.h"

namespace pyrec {
namespace service {

class IndexerClientInternal {
 public:
  virtual ~IndexerClientInternal() {}
  virtual pyrec::Status CallForward(
      const pyrec::service::ForwardIndexerRequest& request,
      pyrec::service::IndexerReply* reply) = 0;
  virtual pyrec::Status CallInverted(
      const pyrec::service::InvertedIndexerRequest& request,
      pyrec::service::IndexerReply* reply) = 0;
};

class LocalIndexerClientInternal : public IndexerClientInternal {
 public:
  explicit LocalIndexerClientInternal(
      const pyrec::service::IndexerServiceInterface& server_interface)
      : server_(server_interface.GetServer()) {}
  explicit LocalIndexerClientInternal(
      std::shared_ptr<pyrec::service::IndexerServiceHybridBase> server)
      : server_(server) {}

  pyrec::Status CallForward(
      const pyrec::service::ForwardIndexerRequest& request,
      pyrec::service::IndexerReply* reply) override {
    return server_->ForwardLocal(&request, reply);
  }

  pyrec::Status CallInverted(
      const pyrec::service::InvertedIndexerRequest& request,
      pyrec::service::IndexerReply* reply) override {
    return server_->InvertedLocal(&request, reply);
  }

 private:
  std::shared_ptr<IndexerServiceHybridBase> server_;
};

class RemoteIndexerClientInternal : public IndexerClientInternal {
 public:
  RemoteIndexerClientInternal(const char* ip, int port)
      : remote_address_({ip, port}) {
    Connect();
  }
  explicit RemoteIndexerClientInternal(const pyrec::Address remote_address)
      : remote_address_(remote_address) {
    Connect();
  }

  ~RemoteIndexerClientInternal() {
    Disconnect();
  }

  pyrec::Status CallForward(
      const pyrec::service::ForwardIndexerRequest& request,
      pyrec::service::IndexerReply* reply) override {
    if (!Ping())
      return pyrec::Status::CANCELLED;

    grpc::ClientContext context;
    std::unique_ptr<IndexerService::Stub> stub(
        IndexerService::NewStub(channel_));
    grpc::Status status = stub->Forward(&context, request, reply);
    return pyrec::Status(status);
  }

  pyrec::Status CallInverted(
      const pyrec::service::InvertedIndexerRequest& request,
      pyrec::service::IndexerReply* reply) override {
    if (!Ping())
      return pyrec::Status::CANCELLED;

    grpc::ClientContext context;
    std::unique_ptr<IndexerService::Stub> stub(
        IndexerService::NewStub(channel_));
    grpc::Status status = stub->Inverted(&context, request, reply);
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

#endif  // PYREC_CORE_INDEXER_INDEXER_CLIENT_H_
