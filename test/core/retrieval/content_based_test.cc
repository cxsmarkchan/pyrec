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

#include "pyrec/proto/recommend.pb.h"
#include "pyrec/core/util/types.h"
#include "pyrec/core/indexer/hash_indexer.h"
#include "pyrec/core/indexer/indexer_client.h"
#include "pyrec/core/retrieval/content_based.h"

using pyrec::service::HashIndexerServer;
using pyrec::service::LocalIndexerClientInternal;
using pyrec::service::CBRetrievalServer;
using pyrec::types::FieldId;
using pyrec::types::CsvFormat;

class CBRetrievalServerTest : public testing::Test {
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

    auto indexer_server = HashIndexerServer::CreateFromCsv(ss, format);
    CBRetrievalServer::Param param = {
      std::shared_ptr<LocalIndexerClientInternal>(
          new LocalIndexerClientInternal(indexer_server)),
      std::make_pair(201, 103),
      {{102, "102_3"}}
    };
    cb_retrieval_server_ = CBRetrievalServer::Create(1045, 10, param);
  }

  void TearDown() {}

 private:
  std::shared_ptr<CBRetrievalServer> cb_retrieval_server_;
};

TEST_F(CBRetrievalServerTest, DoCreate) {
  ASSERT_TRUE(cb_retrieval_server_ != nullptr);
}

TEST_F(CBRetrievalServerTest, DoRetrieval) {
  pyrec::service::PyRecRequest request;
  request.set_request_id("123");
  auto* context = request.mutable_context();
  auto* feature_map = (*context)[201].mutable_feature_map();
  (*feature_map)[103].mutable_bytes_list()->add_values("103_2");

  pyrec::service::ItemReply reply;
  cb_retrieval_server_->RetrievalProcess(&request, &reply);
  ASSERT_EQ(reply.request_id(), "123");
  ASSERT_EQ(reply.items_size(), 2);
  std::unordered_set<std::string> expected_items = {"key2", "key4"};
  for (auto& item : reply.items()) {
    auto& item_id = item.item_id();
    auto it = item_id.find(100);
    ASSERT_TRUE(it != item_id.end());
    ASSERT_TRUE(expected_items.find(it->second.bytes_list().values(0))
                != expected_items.end());
    ASSERT_EQ(item.retrieval_infos_size(), 1);
    ASSERT_EQ(item.retrieval_infos(0).retrieval_id(), 1045);
    ASSERT_EQ(item.retrieval_infos(0).num_items(), 2);
  }
}
