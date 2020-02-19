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

#include "core/indexer/hash_indexer.h"

#include <fstream>
#include <algorithm>
#include <unordered_set>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace pyrec {
namespace service {

using pyrec::types::FieldId;
using pyrec::types::CsvFormat;

// Currently we only support inserting an item, rather than updating an item,
// which means this function returns null if the id exists in the index.
int HashIndexerServer::InsertItemFromCsvLine(
    const std::string& line,
    const CsvFormat& format,
    std::string* pt_item_id,
    const HashIndexerServer::IndexItem** pt_index_item) {
  // TODO(cxsmarkchan): design of the ret code
  // currently: 0 - normal, -1 - empty line, -2 - empty id, -3 - existed id.
  if (line.empty())
    return -1;

  std::vector<std::string> field_values;
  boost::algorithm::split(
      field_values, line,
      boost::algorithm::is_any_of(format.between_delimiter));

  int field_size = std::min(field_values.size(), format.field_ids.size());
  if (field_size == 0 || field_values[0].empty())
    return -2;

  auto& item_id = field_values[0];
  if (forward_index_.find(item_id) != forward_index_.end())
    return -3;

  auto& index_item = forward_index_[item_id];
  for (int i = 1; i < field_size; ++i) {
    if (field_values[i].empty())
      continue;
    if (format.inner_delimiter.empty()) {
      index_item[format.field_ids[i]] = {field_values[i]};
    } else {
      std::vector<std::string> field_inner_values;
      boost::algorithm::split(
          field_inner_values,
          field_values[i],
          boost::algorithm::is_any_of(format.inner_delimiter));
      std::copy_if(field_inner_values.begin(), field_inner_values.end(),
                   std::back_inserter(index_item[format.field_ids[i]]),
                   [](const std::string& s){return !s.empty();});
    }
  }

  *pt_item_id = item_id;
  *pt_index_item = &index_item;

  return 0;
}

void HashIndexerServer::InsertIntoInvertedIndexes(
    const std::string& item_id,
    const HashIndexerServer::IndexItem* index_item) {
  for (auto& field_item : *index_item) {
    auto& inverted_index = inverted_indexes_[field_item.first];
    for (auto& field_inner_item : field_item.second) {
      inverted_index[field_inner_item].push_back(item_id);
    }
  }
}

std::shared_ptr<HashIndexerServer> HashIndexerServer::CreateFromCsv(
    std::istream& stream,
    const CsvFormat& format) {
  // TODO(cxsmarkchan): exception when field_ids contains duplicated keys.

  if (!stream || format.field_ids.size() == 0)
    return nullptr;

  auto server = std::shared_ptr<HashIndexerServer>(
      new HashIndexerServer(format.field_ids[0]));

  std::string line;
  while (std::getline(stream, line)) {
    boost::algorithm::trim(line);
    std::string item_key;
    const IndexItem* index_item;
    if (server->InsertItemFromCsvLine(
            line, format, &item_key, &index_item) != 0)
      continue;
    server->InsertIntoInvertedIndexes(item_key, index_item);
  }

  return server;
}

void HashIndexerServer::FillReplyItem(
    const std::string& item_key,
    const HashIndexerServer::IndexItem& index_item,
    const std::unordered_set<FieldId>& requested_fields,
    IndexerReply* reply) {
  auto* reply_item = reply->add_items();

  auto* reply_item_field_map = reply_item->mutable_fields() \
                                         ->mutable_map_items();

  // Here we need to separately deal with key since
  //  it does not exist in the values of the forward map.
  if (requested_fields.empty() ||
      requested_fields.find(key_id_) != requested_fields.end()) {
    auto& reply_item_field = (*reply_item_field_map)[key_id_];
    reply_item_field.mutable_bytes_list()->add_values(item_key);
  }

  for (auto& item : index_item) {
    auto field_id = item.first;
    if (!requested_fields.empty() &&
        requested_fields.find(field_id) == requested_fields.end())
      continue;
    auto& reply_item_field = (*reply_item_field_map)[field_id];
    auto& index_field = item.second;
    for (auto& index_field_term : index_field) {
      reply_item_field.mutable_bytes_list()->add_values(index_field_term);
    }
  }
}

pyrec::Status HashIndexerServer::ForwardProcess(
    const ForwardIndexerRequest* request,
    IndexerReply* reply) {
  if (request->key_id() != key_id_) {
    // TODO(cxsmarkchan): modify error code.
    return pyrec::Status::CANCELLED;
  }

  std::unordered_set<FieldId> requested_fields;
  for (auto& field_id : request->requested_fields()) {
    requested_fields.insert(field_id);
  }

  // Search for each key.
  for (auto& key : request->keys()) {
    if (forward_index_.find(key) == forward_index_.end())
      continue;
    auto& index_item = forward_index_[key];
    FillReplyItem(key, index_item, requested_fields, reply);
  }

  return pyrec::Status::OK;
}

namespace {

std::string getStringSearchKey(
    const pyrec::service::SearchItem& search_item) {
  if (search_item.kind_case() != pyrec::service::SearchItem::kMatchSearchItem)
    return "";

  auto& match_search_item = search_item.match_search_item();
  switch (match_search_item.kind_case()) {
  case pyrec::service::MatchSearchItem::kBytesValue:
    return match_search_item.bytes_value();
  case pyrec::service::MatchSearchItem::kIntValue:
    return boost::lexical_cast<std::string>(match_search_item.int_value());
  default:
    return "";
  }
}

class IntersectedResult {
 public:
  IntersectedResult() : initialized_(false) {}

  bool IsEmpty() {
    return initialized_ && result_set_.empty();
  }

  void Insert(const std::vector<std::string>& items) {
    if (!initialized_) {
      initialized_ = true;
      for (auto& item : items)
        result_set_.insert(item);
      return;
    }

    std::unordered_set<std::string> new_result_set_;
    for (auto& item : items) {
      if (result_set_.find(item) != result_set_.end()) {
        new_result_set_.insert(item);
      }
    }
    result_set_ = new_result_set_;
  }

  const std::unordered_set<std::string>& GetResults() const {
    return result_set_;
  }

 private:
  std::unordered_set<std::string> result_set_;
  bool initialized_;
};

}  // namespace

void HashIndexerServer::ProcessSingleSearchRequest(
    const pyrec::service::SearchRequest& search_request,
    std::unordered_set<std::string>* result_keys,
    int max_num) {
  if (result_keys->size() >= max_num)
    return;

  IntersectedResult current_result_set;

  for (auto& item : search_request.search_items()) {
    auto field_id = item.first;
    if (inverted_indexes_.find(field_id) == inverted_indexes_.end())
      break;
    auto& inverted_index = inverted_indexes_[field_id];

    std::string inverted_search_key = getStringSearchKey(item.second);
    if (inverted_search_key.empty())
      continue;

    // one of the search item not satisfied, return now
    if (inverted_index.find(inverted_search_key) == inverted_index.end())
      return;
    auto& items = inverted_index[inverted_search_key];
    if (items.size() == 0)
      return;
    current_result_set.Insert(items);
    if (current_result_set.IsEmpty())
      return;
  }

  for (auto& result_item : current_result_set.GetResults()) {
    result_keys->insert(result_item);
    if (result_keys->size() >= max_num)
      return;
  }
}

pyrec::Status HashIndexerServer::InvertedProcess(
    const InvertedIndexerRequest* request,
    IndexerReply* reply) {
  std::unordered_set<std::string> result_keys;
  for (auto& search_request : request->search_requests()) {
    // TODO(cxsmarkchan): obtain just max_num items is inappropriate if the
    // inverted index and the forward index are temporally inconsistent.
    ProcessSingleSearchRequest(search_request,
                               &result_keys,
                               request->max_num());
  }

  std::unordered_set<FieldId> requested_fields;
  for (auto& field_id : request->requested_fields()) {
    requested_fields.insert(field_id);
  }
  for (auto& key : result_keys) {
    FillReplyItem(key, forward_index_[key], requested_fields, reply);
  }
  return pyrec::Status::OK;
}

}  // namespace service
}  // namespace pyrec
