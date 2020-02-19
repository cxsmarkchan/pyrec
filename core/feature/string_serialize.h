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

#ifndef PYREC_CORE_FEATURE_STRING_SERIALIZE_H_
#define PYREC_CORE_FEATURE_STRING_SERIALIZE_H_

#include <string>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "pyrec/proto/feature.pb.h"

namespace pyrec {
namespace feature {

struct FeatureStringFormat {
  std::string between_delimiter = "|";
  std::string inner_delimiter = ";";
  std::string field_id_delimiter = ":";
  std::string weighted_id_delimiter = "=";
};

template<typename T>
inline std::string FeatureToString(
    const T& feature, const FeatureStringFormat& format) {
  return boost::lexical_cast<std::string>(feature);
}

template<>
inline std::string FeatureToString(
    const float& feature, const FeatureStringFormat& format) {
  return boost::str(boost::format("%.3f") % feature);
}

template<>
inline std::string FeatureToString(
    const IdWeight& feature,
    const FeatureStringFormat& format) {
  switch (feature.kind_case()) {
    case IdWeight::KindCase::kIdBytes:
      return boost::str(boost::format("%s%s%.3f")
                        % feature.id_bytes()
                        % format.weighted_id_delimiter
                        % feature.weight());
    case IdWeight::KindCase::kIdInt:
      return boost::str(boost::format("%d%s%.3f")
                        % feature.id_int()
                        % format.weighted_id_delimiter
                        % feature.weight());
    default:
      return "";
  }
}

#define DEFINE_FEATURE_LIST_TO_STRING(list_type) \
template<> \
inline std::string FeatureToString( \
    const list_type& feature, \
    const FeatureStringFormat& format) { \
  std::vector<std::string> vec_str; \
  for (auto& value : feature.values()) { \
    vec_str.push_back(FeatureToString(value, format)); \
  } \
  return boost::algorithm::join(vec_str, format.inner_delimiter); \
}

DEFINE_FEATURE_LIST_TO_STRING(WeightedList)
DEFINE_FEATURE_LIST_TO_STRING(BytesList)
DEFINE_FEATURE_LIST_TO_STRING(FloatList)
DEFINE_FEATURE_LIST_TO_STRING(IntList)

template<>
inline std::string FeatureToString(
    const FeatureList& feature,
    const FeatureStringFormat& format) {
  switch (feature.kind_case()) {
    case FeatureList::KindCase::kBytesList:
      return FeatureToString(feature.bytes_list(), format);
    case FeatureList::KindCase::kFloatList:
      return FeatureToString(feature.float_list(), format);
    case FeatureList::KindCase::kIntList:
      return FeatureToString(feature.int_list(), format);
    case FeatureList::KindCase::kWeightedList:
      return FeatureToString(feature.weighted_list(), format);
    default:
      return "";
  }
}

template<>
inline std::string FeatureToString(
    const FeatureMap& feature,
    const FeatureStringFormat& format) {
  std::vector<std::string> vec_str;
  for (auto item : feature.map_items()) {
    vec_str.push_back(boost::str(boost::format("%d%s%s")
                                 % item.first
                                 % format.field_id_delimiter
                                 % FeatureToString(item.second, format)));
  }
  return boost::algorithm::join(vec_str, format.between_delimiter);
}

}  // namespace feature
}  // namespace pyrec

#endif  // PYREC_CORE_FEATURE_STRING_SERIALIZE_H_
