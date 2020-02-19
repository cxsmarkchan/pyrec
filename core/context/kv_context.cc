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

#include "core/context/kv_context.h"

#include <fstream>
#include <algorithm>
#include <unordered_set>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "core/feature/string_serialize.h"

#include "pyrec/proto/indexer.pb.h"
#include "pyrec/proto/feature.pb.h"

namespace pyrec {
namespace service {

using pyrec::types::FieldId;

namespace kv {

bool IndexerKvServer::RequestIndexer(
    int key_id,
    const std::vector <std::string> &keys,
    const std::vector<pyrec::types::FieldId>& fields,
    pyrec::service::IndexerReply *reply) {
  pyrec::service::ForwardIndexerRequest request;
  request.set_key_id(key_id);

  for (auto& key : keys)
    request.add_keys(key);

  for (auto& field : fields)
    request.add_requested_fields(field);

  auto status = indexer_->CallForward(request, reply);
  return status.ok();
}

bool IndexerKvServer::GetProto(
    int key_id,
    const std::vector <std::string> &keys,
    const std::vector<pyrec::types::FieldId>& fields,
    std::vector <pyrec::feature::FeatureMap> *results) {
  pyrec::service::IndexerReply reply;
  if (!RequestIndexer(key_id, keys, fields, &reply))
    return false;

  for (auto &item : reply.items()) {
    results->push_back(item.fields());
  }

  return true;
}

bool IndexerKvServer::GetString(
    int key_id,
    const std::vector <std::string> &keys,
    const std::vector<pyrec::types::FieldId>& fields,
    std::vector <std::string> *results) {
  pyrec::service::IndexerReply reply;
  if (!RequestIndexer(key_id, keys, fields, &reply))
    return false;
  for (auto &item : reply.items()) {
    results->push_back(pyrec::feature::FeatureToString(
        item.fields(), pyrec::feature::FeatureStringFormat()));
  }
  return true;
}

}  // namespace kv

pyrec::Status KvContextServer::ContextProcess(
    const PyRecRequest* request, ContextReply* reply) {
  auto it_scope = request->context().find(param_.key_id.first);
  if (it_scope == request->context().end())
    return pyrec::Status::CANCELLED;
  auto feature_map = it_scope->second.map_items();

  auto it_key = feature_map.find(param_.key_id.second);
  if (it_key == feature_map.end())
    return pyrec::Status::CANCELLED;
  auto feature_list = it_key->second;

  std::vector<std::string> keys;
  // TODO(cxsmarkchan): Here exists many unneccessary assignments?
  switch (feature_list.kind_case()) {
    case pyrec::feature::FeatureList::KindCase::kBytesList:
      for (auto& feature : feature_list.bytes_list().values())
        keys.push_back(feature);
      break;
    case pyrec::feature::FeatureList::KindCase::kIntList:
      for (auto& feature : feature_list.int_list().values())
        keys.push_back(boost::lexical_cast<std::string>(feature));
      break;
    default:
      return pyrec::Status::CANCELLED;
  }

  if (keys.size() != 1)  // Should be only one key.
    return pyrec::Status::CANCELLED;

  //  from the vector response_map to reply
  std::vector<pyrec::feature::FeatureMap> kv_results;
  bool kv_state = param_.kv_server->GetProto(
      param_.key_id.second, keys, param_.fields, &kv_results);
  if (!kv_state)
    return pyrec::Status::CANCELLED;

  reply->set_request_id(request->request_id());
  if (kv_results.size() == 0)
    return pyrec::Status::OK;
  // TODO(cxsmarkchan): Should I use move assignment?
  reply->mutable_context()->CopyFrom(kv_results[0]);
  return pyrec::Status::OK;
}

}  // namespace service
}  // namespace pyrec
