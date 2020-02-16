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

#define private public
#define protected public

#include <memory>
#include <sstream>

#include "gtest/gtest.h"

#include "pyrec/core/util/types.h"
#include "pyrec/core/indexer/hash_indexer.h"

using pyrec::service::HashIndexerServer;
using pyrec::types::FieldId;
using pyrec::types::CsvFormat;

namespace {

std::string ExtractKeyFromProto(
    const pyrec::service::IndexItem& index_item_proto,
    FieldId key_id) {
  auto& field_map = index_item_proto.fields().map_items();
  auto it = field_map.find(key_id);
  if (it == field_map.end())
    return "";
  auto& field_list = it->second.bytes_list();
  if (field_list.values_size() > 1)
    return "";
  return field_list.values(0);
}

HashIndexerServer::IndexItem ExtractIndexItemFromProto(
    const pyrec::service::IndexItem& index_item_proto) {
  HashIndexerServer::IndexItem index_item;
  for (auto& item : index_item_proto.fields().map_items()) {
    auto field_id = item.first;
    auto& value_list = item.second.bytes_list().values();
    for (auto& value : value_list) {
      index_item[field_id].push_back(value);
    }
  }
  return index_item;
}

void CheckReplyItem(const pyrec::service::IndexItem& index_item,
                    FieldId key_id,
                    const std::string& expected_key,
                    const HashIndexerServer::IndexItem& expected_index_item) {
  std::string key = ExtractKeyFromProto(index_item, key_id);
  ASSERT_EQ(key, expected_key);
  auto item = ExtractIndexItemFromProto(index_item);
  ASSERT_EQ(item, expected_index_item);
}

}  // namespace

class HashIndexerTest : public testing::Test {
 public:
  void SetUp() {
    std::stringstream ss;
    ss << "key1,101_1,102_1:102_2:102_3,103_1" << std::endl;
    ss << "key2,101_2,102_3:102_4:102_5,103_2" << std::endl;
    ss << "key3,101_2,102_1:102_2:102_5,103_1" << std::endl;
    ss << "key4,101_1,102_3,103_2" << std::endl;
    ss << "key5,101_2,102_2:102_4" << std::endl;
    ss << ",101_2,102_2:102_4,103_1" << std::endl;
    CsvFormat format;
    format.field_ids = {100, 101, 102, 103};
    format.between_delimiter = ",";
    format.inner_delimiter = ":";

    server_ = HashIndexerServer::CreateFromCsv(ss, format);
  }

  void TearDown() {}

 private:
  std::shared_ptr<HashIndexerServer> server_;
};

TEST_F(HashIndexerTest, DoCreateTest) {
  ASSERT_EQ(server_->key_id_, 100);

  std::unordered_map<std::string,
      HashIndexerServer::IndexItem> expected_forward_index = {
      {"key1", {{101, {"101_1"}},
                {102, {"102_1", "102_2", "102_3"}}, {103, {"103_1"}}}},
      {"key2", {{101, {"101_2"}},
                {102, {"102_3", "102_4", "102_5"}}, {103, {"103_2"}}}},
      {"key3", {{101, {"101_2"}},
                {102, {"102_1", "102_2", "102_5"}}, {103, {"103_1"}}}},
      {"key4", {{101, {"101_1"}},
                {102, {"102_3"}}, {103, {"103_2"}}}},
      {"key5", {{101, {"101_2"}},
                {102, {"102_2", "102_4"}}}}
  };

  std::unordered_map<FieldId,
      HashIndexerServer::InvertedIndex> expected_inverted_indexes = {
      {101, {{"101_1", {"key1", "key4"}},
             {"101_2", {"key2", "key3", "key5"}}}},
      {102, {{"102_1", {"key1", "key3"}},
             {"102_2", {"key1", "key3", "key5"}},
             {"102_3", {"key1", "key2", "key4"}},
             {"102_4", {"key2", "key5"}},
             {"102_5", {"key2", "key3"}}}},
      {103, {{"103_1", {"key1", "key3"}},
             {"103_2", {"key2", "key4"}}}}
  };

  ASSERT_EQ(server_->forward_index_, expected_forward_index);
  ASSERT_EQ(server_->inverted_indexes_, expected_inverted_indexes);
}


TEST_F(HashIndexerTest, DoForward) {
  pyrec::service::ForwardIndexerRequest request;
  pyrec::service::IndexerReply reply;
  request.set_key_id(100);
  request.add_keys("key1");
  request.add_keys("key6");
  request.add_keys("key5");
  request.add_requested_fields(100);
  request.add_requested_fields(101);
  request.add_requested_fields(103);
  request.add_requested_fields(104);

  server_->ForwardProcess(&request, &reply);
  ASSERT_EQ(reply.items_size(), 2);
  CheckReplyItem(reply.items(0), 100, "key1",
                   {{100, {"key1"}},
                    {101, {"101_1"}},
                    {103, {"103_1"}}});
  CheckReplyItem(reply.items(1), 100, "key5",
                   {{100, {"key5"}},
                    {101, {"101_2"}}});

  reply.Clear();
  request.set_key_id(200);
  server_->ForwardProcess(&request, &reply);
  ASSERT_EQ(reply.items_.size(), 0);
}

TEST_F(HashIndexerTest, DoInverted) {
  pyrec::service::InvertedIndexerRequest request;
  pyrec::service::IndexerReply reply;
  std::unordered_set<std::string> key_set;

  request.set_max_num(100);

  auto* search_request = request.add_search_requests();
  auto* search_items = search_request->mutable_search_items();
  (*search_items)[102].mutable_match_search_item()->set_bytes_value("102_3");
  (*search_items)[101].mutable_match_search_item()->set_bytes_value("101_1");

  search_request = request.add_search_requests();
  search_items = search_request->mutable_search_items();
  (*search_items)[102].mutable_match_search_item()->set_bytes_value("102_2");
  (*search_items)[101].mutable_match_search_item()->set_bytes_value("101_2");

  server_->InvertedProcess(&request, &reply);
  ASSERT_EQ(reply.items().size(), 4);
  key_set = {"key1", "key3", "key4", "key5"};
  for (auto& item : reply.items()) {
    std::string key = ExtractKeyFromProto(item, 100);
    ASSERT_TRUE(key_set.find(key) != key_set.end());
    key_set.erase(key);
    HashIndexerServer::IndexItem copy_index = server_->forward_index_[key];
    copy_index[100] = {key};
    CheckReplyItem(item, 100, key, copy_index);
  }

  reply.Clear();
  request.set_max_num(3);
  server_->InvertedProcess(&request, &reply);
  ASSERT_EQ(reply.items().size(), 3);
  key_set = {"key1", "key3", "key4", "key5"};
  for (auto& item : reply.items()) {
    std::string key = ExtractKeyFromProto(item, 100);
    ASSERT_TRUE(key_set.find(key) != key_set.end());
    key_set.erase(key);
    HashIndexerServer::IndexItem copy_index = server_->forward_index_[key];
    copy_index[100] = {key};
    CheckReplyItem(item, 100, key, copy_index);
  }

  reply.Clear();
  request.set_max_num(2);
  server_->InvertedProcess(&request, &reply);
  ASSERT_EQ(reply.items().size(), 2);
  key_set = {"key1", "key4"};
  for (auto& item : reply.items()) {
    std::string key = ExtractKeyFromProto(item, 100);
    ASSERT_TRUE(key_set.find(key) != key_set.end());
    key_set.erase(key);
    HashIndexerServer::IndexItem copy_index = server_->forward_index_[key];
    copy_index[100] = {key};
    CheckReplyItem(item, 100, key, copy_index);
  }
}
