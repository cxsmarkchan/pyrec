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

#ifndef PYREC_CORE_RETRIEVAL_CONTENT_BASED_H_
#define PYREC_CORE_RETRIEVAL_CONTENT_BASED_H_

#include <string>
#include <memory>
#include <utility>
#include <unordered_map>

#include "pyrec/proto/recommend.hlrpc.pb.h"
#include "pyrec/core/util/status.h"
#include "pyrec/core/util/types.h"
#include "pyrec/core/indexer/indexer_client.h"
#include "pyrec/core/retrieval/retrieval_interface.h"

namespace pyrec {
namespace service {

// This class implements the content-based retrieval algorithm.
class CBRetrievalServer final :
    public RetrievalServiceHybridBase {
 public:
  virtual ~CBRetrievalServer() {}
  CBRetrievalServer(const CBRetrievalServer&) = delete;
  CBRetrievalServer& operator=(
      const CBRetrievalServer&) = delete;

  struct Param {
    std::shared_ptr<IndexerClientInternal> indexer;
    std::pair<pyrec::types::FieldId, pyrec::types::FieldId> extract_key;
    std::unordered_map<pyrec::types::FieldId, std::string> filter_rules;
  };

  static std::shared_ptr<CBRetrievalServer> Create(
      pyrec::types::FieldId retrieval_id,
      int request_num,
      const Param& param) {
    if (param.indexer == nullptr || request_num == 0)
      return nullptr;
    return std::shared_ptr<CBRetrievalServer>(
        new CBRetrievalServer(retrieval_id, request_num, param));
  }

 public:
  pyrec::Status RetrievalProcess(const PyRecRequest* request,
                                 ItemReply* reply) override;


 private:
  explicit CBRetrievalServer(pyrec::types::FieldId retrieval_id,
                             int request_num,
                             const Param& param)
      : retrieval_id_(retrieval_id), request_num_(request_num),
        param_(param) {}

  bool FillInvertedRequest(const pyrec::feature::FeatureList& feature_list,
                           InvertedIndexerRequest* indexer_request);

  void FillReplyItems(const IndexerReply& indexer_reply, ItemReply* item_reply);

 private:
  pyrec::types::FieldId retrieval_id_;
  int request_num_;
  Param param_;
};  // class CBRetrievalServer

class CBRetrievalServerInterface : public RetrievalServiceInterface {
 public:
  CBRetrievalServerInterface() : retrieval_id_(0), request_num_(0) {
    param_.indexer = nullptr;
  }

  CBRetrievalServerInterface& SetIndexer(
      std::shared_ptr<IndexerClientInternal> indexer) {
    param_.indexer = indexer;
    return *this;
  }

  CBRetrievalServerInterface& SetRetrievalId(
      pyrec::types::FieldId retrieval_id) {
    retrieval_id_ = retrieval_id;
    return *this;
  }

  CBRetrievalServerInterface& SetRequestNum(int request_num) {
    request_num_ = request_num;
    return *this;
  }

  CBRetrievalServerInterface& AddFilterRule(
      pyrec::types::FieldId key, const std::string& value) {
    param_.filter_rules[key] = value;
    return *this;
  }

  CBRetrievalServerInterface& SetExtractKey(
      pyrec::types::FieldId scope, pyrec::types::FieldId key) {
    param_.extract_key = std::make_pair(scope, key);
    return *this;
  }

  int Create() {
    server_ = CBRetrievalServer::Create(retrieval_id_, request_num_, param_);
    if (server_)
      return 0;
    return -1;
  }

 private:
  pyrec::types::FieldId retrieval_id_;
  int request_num_;
  CBRetrievalServer::Param param_;
};  // class CBRetrievalServerInterface

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_RETRIEVAL_CONTENT_BASED_H_
