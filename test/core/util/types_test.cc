/*
 * Copyright 2020 The PyRec Authors. All Rights Reserved.
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

#include "pyrec/core/util/types.h"

class TypesTest : public testing::Test {
 public:
  void SetUp() {}
  void TearDown() {}
};

TEST(TypesTest, DoDebugStringTest) {
  pyrec::types::CsvFormat csv_format;
  csv_format.field_ids = {101, 102, 103};
  csv_format.between_delimiter = ";";
  csv_format.inner_delimiter = ":";
  ASSERT_EQ(csv_format.DebugString(),
            "between_delimiter: \";\", inner_delimiter: \":\", "
            "field_ids: 101 102 103");
}
