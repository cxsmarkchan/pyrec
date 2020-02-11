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

#ifndef PYREC_CORE_UTIL_TYPES_H_
#define PYREC_CORE_UTIL_TYPES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

namespace pyrec {
namespace types {

typedef int32_t FieldId;

struct CsvFormat {
  std::vector<FieldId> field_ids;
  std::string between_delimiter = ",";
  std::string inner_delimiter = "";

  std::string DebugString() const {
    std::stringstream ss;
    ss << "between_delimiter: \"" << between_delimiter << "\", "
       << "inner_delimiter: \"" << inner_delimiter << "\", ";
    if (field_ids.size() == 0)
      return ss.str();

    ss << "field_ids: " << field_ids[0];
    for (int i = 1; i < field_ids.size(); ++i) {
      ss << " " << field_ids[i];
    }
    return ss.str();
  }
};

}  // namespace types
}  // namespace pyrec

#endif  // PYREC_CORE_UTIL_TYPES_H_
