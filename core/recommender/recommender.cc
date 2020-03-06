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

#include "core/recommender/recommender.h"

#include <boost/format.hpp>

#include "core/util/types.h"

namespace pyrec {
namespace service {

pyrec::Status RecommenderServer::RecommendProcess(const PyRecRequest* request,
                                                  ItemReply* reply) {
  PyRecRequest request_with_context;
  request_with_context.CopyFrom(*request);

  for (auto& context_item : param_.context_map) {
    pyrec::types::FieldId context_id = context_item.first;
    auto& context_client = context_item.second;
    ContextReply context_reply;
    context_client->CallContext(request_with_context, &context_reply);
    auto* context_map = request_with_context.mutable_context();
    (*context_map)[context_id].CopyFrom(context_reply.context());
  }

  PyRecRequestWithItems request_with_items;
  for (auto& retrieval_item : param_.retrieval_vec) {
    ItemReply item_reply;
    retrieval_item->CallRetrieval(request_with_context, &item_reply);
    for (auto& item : item_reply.items()) {
      // TODO(cxsmarkchan): merge multiple recall reasons
      request_with_items.add_items()->CopyFrom(item);
    }
  }

  // TODO(cxsmarkchan): add scoring and reranking modules
  for (auto& item : request_with_items.items()) {
    if (reply->items_size() >= param_.request_num)
      break;
    reply->add_items()->CopyFrom(item);
  }
  reply->set_request_id(request->request_id());

  return pyrec::Status::OK;
}

}  // namespace service
}  // namespace pyrec
