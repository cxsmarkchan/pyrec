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
#include "boost/format.hpp"

#include "pyrec/proto/recommend.pb.h"

#include "core/util/types.h"
#include "core/context/kv_context.h"

using pyrec::types::FieldId;
using pyrec::service::KvContextServer;
using pyrec::service::PyRecRequest;
using pyrec::service::ContextReply;

namespace {

class MockKvServer : public pyrec::service::kv::KvServer {
 public:
  bool GetProto(
      int key_id,
      const std::vector<std::string>& keys,
      const std::vector<pyrec::types::FieldId>& fields,
      std::vector<pyrec::feature::FeatureMap>* results) override {
    if (key_id != 100)
      return false;
    std::unordered_set<pyrec::types::FieldId> field_set;
    for (auto& field : fields)
      field_set.insert(field);

    for (auto& key : keys) {
      results->push_back(pyrec::feature::FeatureMap());
      auto& feature_map = results->back();
      auto* map_items = feature_map.mutable_map_items();
      if (field_set.find(100) != field_set.end()) {
        auto* bytes_list = (*map_items)[100].mutable_bytes_list();
        bytes_list->add_values(boost::str(boost::format("%s") % key % key));
      }
      if (field_set.find(101) != field_set.end()) {
        auto* bytes_list = (*map_items)[101].mutable_bytes_list();
        bytes_list->add_values(boost::str(boost::format("%s-%s") % key % key));
      }
    }

    return true;
  }

  bool GetString(
      int key_id,
      const std::vector<std::string>& keys,
      const std::vector<pyrec::types::FieldId>& fields,
      std::vector<std::string>* results) override {
    return true;
  }
};

}  // namespace

TEST(KvContextServerTest, DoContextProcess) {
  KvContextServer::Param param {
    std::make_pair(201, 100),
    {101},
    std::make_shared<MockKvServer>()
  };
  std::shared_ptr<KvContextServer> server = KvContextServer::Create(param);

  PyRecRequest request;
  ContextReply reply;
  request.set_request_id("123");
  auto* context = request.mutable_context();
  auto* map_items = (*context)[201].mutable_map_items();
  (*map_items)[100].mutable_bytes_list()->add_values("user1");
  server->ContextProcess(&request, &reply);

  ASSERT_EQ(reply.request_id(), "123");
  auto it_feature = reply.context().map_items().find(101);
  ASSERT_TRUE(it_feature != reply.context().map_items().end());
  auto& feature_list = it_feature->second;
  ASSERT_TRUE(feature_list.kind_case()
      == pyrec::feature::FeatureList::kBytesList);
  auto& bytes_list = feature_list.bytes_list();
  ASSERT_EQ(bytes_list.values_size(), 1);
  ASSERT_EQ(bytes_list.values(0), "user1-user1");
}
