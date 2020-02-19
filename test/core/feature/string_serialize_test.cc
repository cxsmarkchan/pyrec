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

#include "gtest/gtest.h"
#include "boost/algorithm/string.hpp"

#include "pyrec/core/feature/string_serialize.h"

using pyrec::feature::FeatureMap;

TEST(StringSerialize, DoFeatureToString) {
  FeatureMap feature_map;
  pyrec::feature::FeatureStringFormat format;
  std::unordered_set<std::string> expected_string;

  auto* map_item = feature_map.mutable_map_items();

  auto* bytes_list = (*map_item)[101].mutable_bytes_list();
  bytes_list->add_values("bytes1");
  bytes_list->add_values("bytes2");
  bytes_list->add_values("bytes3");
  bytes_list->add_values("bytes4");
  expected_string.insert("101:bytes1;bytes2;bytes3;bytes4");

  auto* float_list = (*map_item)[102].mutable_float_list();
  float_list->add_values(1.205);
  float_list->add_values(1.384);
  float_list->add_values(1.423);
  expected_string.insert("102:1.205;1.384;1.423");

  auto* int_list = (*map_item)[103].mutable_int_list();
  int_list->add_values(1);
  int_list->add_values(2);
  int_list->add_values(3);
  expected_string.insert("103:1;2;3");

  auto* weighted_list = (*map_item)[104].mutable_weighted_list();
  auto* id_weight_1 = weighted_list->add_values();
  id_weight_1->set_id_bytes("id_bytes");
  id_weight_1->set_weight(1.250);
  auto* id_weight_2 = weighted_list->add_values();
  id_weight_2->set_id_bytes("id_int");
  id_weight_2->set_weight(1.231);
  expected_string.insert("104:id_bytes=1.250;id_int=1.231");

  std::string feature_str = pyrec::feature::FeatureToString(
      feature_map, format);
  std::vector<std::string> feature_str_splitted;
  boost::algorithm::split(feature_str_splitted,
                          feature_str,
                          boost::algorithm::is_any_of("|"));
  for (auto& feature_str_item : feature_str_splitted) {
    ASSERT_TRUE(expected_string.find(feature_str_item)
                != expected_string.end());
    expected_string.erase(feature_str_item);
  }
}
