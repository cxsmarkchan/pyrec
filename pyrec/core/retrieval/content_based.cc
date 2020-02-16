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

#include "pyrec/core/retrieval/content_based.h"
#include "pyrec/proto/indexer.pb.h"

#include <boost/format.hpp>

namespace pyrec {
namespace service {

bool CBRetrievalServer::FillInvertedRequest(
    const pyrec::feature::FeatureList& feature_list,
    InvertedIndexerRequest* indexer_request) {
  if (feature_list.kind_case() != pyrec::feature::FeatureList::kBytesList)
    return false;

  for (auto& feature : feature_list.bytes_list().values()) {
    auto* search_request = indexer_request->add_search_requests();
    auto* search_items = search_request->mutable_search_items();
    (*search_items)[param_.extract_key.second] \
        .mutable_match_search_item() \
        ->set_bytes_value(feature);

    for (auto& filter_item : param_.filter_rules) {
      (*search_items)[filter_item.first] \
          .mutable_match_search_item() \
          ->set_bytes_value(filter_item.second);
    }
  }
  return true;
}

void CBRetrievalServer::FillReplyItems(const IndexerReply& indexer_reply,
                                       ItemReply* item_reply) {
  size_t num_items = indexer_reply.items_size();
  for (auto& index_item : indexer_reply.items()) {
    auto& field_map = index_item.fields().map_items();

    auto* reply_item = item_reply->add_items();
    auto* item_id_map = reply_item->mutable_item_id()->mutable_map_items();
    for (auto& key : param_.item_keys) {
      auto it_key = field_map.find(key);
      if (it_key == field_map.end())
        continue;

      (*item_id_map)[key].CopyFrom(it_key->second);
    }

    if (item_id_map->size() == 0) {
      item_reply->mutable_items()->RemoveLast();
      continue;
    }

    auto* retrieval_info = reply_item->add_retrieval_infos();
    retrieval_info->set_retrieval_id(retrieval_id_);
    retrieval_info->set_num_items(num_items);
  }
}

pyrec::Status CBRetrievalServer::RetrievalProcess(const PyRecRequest* request,
                                                  ItemReply* reply) {
  auto& context = request->context();
  auto it_context = context.find(param_.extract_key.first);
  if (it_context == context.end())
    return pyrec::Status::CANCELLED;
  auto& feature_map = it_context->second.map_items();

  auto it_feature = feature_map.find(param_.extract_key.second);
  if (it_feature == feature_map.end())
    return pyrec::Status::CANCELLED;
  auto& feature_list = it_feature->second;

  InvertedIndexerRequest indexer_request;
  indexer_request.set_max_num(request_num_);
  for (auto& key_id : param_.item_keys)
    indexer_request.add_requested_fields(key_id);
  FillInvertedRequest(feature_list, &indexer_request);
  IndexerReply indexer_reply;

  param_.indexer->CallInverted(indexer_request, &indexer_reply);

  reply->set_request_id(request->request_id());
  FillReplyItems(indexer_reply, reply);

  return pyrec::Status::OK;
}

}  // namespace service
}  // namespace pyrec
