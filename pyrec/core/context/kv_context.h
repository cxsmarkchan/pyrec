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

#ifndef PYREC_CORE_CONTEXT_KV_CONTEXT_H_
#define PYREC_CORE_CONTEXT_KV_CONTEXT_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <fstream>
#include <utility>

#include "pyrec/core/util/status.h"
#include "pyrec/core/util/types.h"
#include "pyrec/proto/indexer.hlrpc.pb.h"
#include "pyrec/proto/recommend.hlrpc.pb.h"
#include "pyrec/core/context/context_interface.h"
#include "pyrec/core/indexer/indexer_client.h"

namespace pyrec {
namespace service {

namespace kv {

class KvServer {
 public:
  virtual ~KvServer() {}
  virtual bool GetProto(
      int key_id,
      const std::vector<std::string>& keys,
      const std::vector<pyrec::types::FieldId>& fields,
      std::vector<pyrec::feature::FeatureMap>* results) = 0;
  virtual bool GetString(
      int key_id,
      const std::vector<std::string>& keys,
      const std::vector<pyrec::types::FieldId>& fields,
      std::vector<std::string>* results) = 0;
};

class IndexerKvServer final : public KvServer {
 public:
  explicit IndexerKvServer(
      std::shared_ptr<pyrec::service::IndexerClientInternal> indexer)
      : indexer_(indexer) {}

 public:
  bool GetProto(
      int key_id,
      const std::vector<std::string>& keys,
      const std::vector<pyrec::types::FieldId>& fields,
      std::vector<pyrec::feature::FeatureMap>* results) override;
  bool GetString(
      int key_id,
      const std::vector<std::string>& keys,
      const std::vector<pyrec::types::FieldId>& fields,
      std::vector<std::string>* results) override;

 private:
  bool RequestIndexer(int key_id,
                      const std::vector<std::string>& keys,
                      const std::vector<pyrec::types::FieldId>& fields,
                      pyrec::service::IndexerReply* reply);

 private:
  std::shared_ptr<IndexerClientInternal> indexer_;
};

}  // namespace kv

// KvContextServer uses a key to find context features
class KvContextServer final:
    public ContextServiceHybridBase {
 public:
  ~KvContextServer() {}
  KvContextServer(const KvContextServer&) = delete;
  KvContextServer& operator=(const KvContextServer&) = delete;

  struct Param {
    std::pair<pyrec::types::FieldId, pyrec::types::FieldId> key_id;
    std::vector<pyrec::types::FieldId> fields;
    std::shared_ptr<kv::KvServer> kv_server;
  };

  static std::shared_ptr<KvContextServer> Create(const Param& param) {
    return std::shared_ptr<KvContextServer>(new KvContextServer(param));
  }

 public:
  pyrec::Status ContextProcess(const PyRecRequest* request,
      ContextReply* reply) override;

 private:
  explicit KvContextServer(const Param& param) : param_(param) {}
  Param param_;
};

class KvContextServerInterface : public ContextServiceInterface {
 public:
  KvContextServerInterface& SetKeyId(
      pyrec::types::FieldId scope, pyrec::types::FieldId key) {
    param_.key_id = std::make_pair(scope, key);
    return *this;
  }

  KvContextServerInterface& AddField(
      pyrec::types::FieldId field) {
    param_.fields.push_back(field);
    return *this;
  }

  KvContextServerInterface& SetIndexerKvServer(
      std::shared_ptr<IndexerClientInternal> indexer) {
    param_.kv_server = std::make_shared<kv::IndexerKvServer>(indexer);
    return *this;
  }

  int Create() {
    server_ = KvContextServer::Create(param_);
    if (server_ == nullptr)
      return -1;
    return 0;
  }

 private:
  KvContextServer::Param param_;
};  // class KvContextServerInterface

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_CONTEXT_KV_CONTEXT_H_
