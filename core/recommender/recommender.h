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

#ifndef PYREC_CORE_RECOMMENDER_RECOMMENDER_H_
#define PYREC_CORE_RECOMMENDER_RECOMMENDER_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include "pyrec/proto/recommend.hlrpc.pb.h"
#include "core/util/status.h"
#include "core/util/types.h"

#include "core/recommender/recommender_interface.h"
#include "core/context/context_client.h"
#include "core/retrieval/retrieval_client.h"

namespace pyrec {
namespace service {

class RecommenderServer final :
    public RecommenderServiceHybridBase {
 public:
  ~RecommenderServer() {}

  struct Param {
    std::unordered_map<pyrec::types::FieldId,
        std::shared_ptr<ContextClientInternal>> context_map;
    std::vector<std::shared_ptr<RetrievalClientInternal>> retrieval_vec;
    int request_num;
  };

  static std::shared_ptr<RecommenderServer> Create(Param param) {
    return std::shared_ptr<RecommenderServer>(new RecommenderServer(param));
  }

 public:
  pyrec::Status RecommendProcess(const PyRecRequest* request,
                                 ItemReply* reply) override;
 private:
  explicit RecommenderServer(Param param) : param_(param) {}
  Param param_;
};

class RecommenderServerInterface : public RecommenderServiceInterface {
 public:
  RecommenderServerInterface& AddContext(
      pyrec::types::FieldId context_id,
      std::shared_ptr<ContextClientInternal> context) {
    if (context)
      param_.context_map[context_id] = context;
    return *this;
  }

  RecommenderServerInterface& AddRetrieval(
      std::shared_ptr<RetrievalClientInternal> retrieval) {
    if (retrieval)
      param_.retrieval_vec.push_back(retrieval);
    return *this;
  }

  RecommenderServerInterface& SetRequestNum(int request_num) {
    param_.request_num = request_num;
    return *this;
  }

  int Create() {
    server_ = RecommenderServer::Create(param_);
    if (server_)
      return 0;
    return -1;
  }

 private:
  RecommenderServer::Param param_;
};

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_RECOMMENDER_RECOMMENDER_H_
