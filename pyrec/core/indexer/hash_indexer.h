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

#ifndef PYREC_CORE_INDEXER_HASH_INDEXER_H_
#define PYREC_CORE_INDEXER_HASH_INDEXER_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <istream>

#include "pyrec/core/util/status.h"
#include "pyrec/core/util/type.h"
#include "pyrec/proto/indexer.hlrpc.pb.h"

namespace pyrec {
namespace service {

// HashIndexerServer is a simple indexer which use hash tables for
// forward and inverted index.
class HashIndexerServer final :
    public IndexerServiceHybridBase {
 public:
  ~HashIndexerServer() {}
  HashIndexerServer(const HashIndexerServer&) = delete;
  HashIndexerServer& operator=(const HashIndexerServer&) = delete;

  typedef std::vector<std::string> IndexField;
  // TODO(cxsmarkchan): IndexItem can be a vector rather than a hash map.
  typedef std::unordered_map<pyrec::FieldIdType, IndexField> IndexItem;
  typedef std::vector<std::string> InvertedItems;
  typedef std::unordered_map<std::string, InvertedItems> InvertedIndex;
  struct CsvFormat {
    std::vector<pyrec::FieldIdType> field_ids;
    std::string between_delimiter = ",";
    std::string inner_delimiter = "";
  };

  static std::unique_ptr<HashIndexerServer> CreateFromCsv(
      std::istream& stream,
      const CsvFormat& format);

 public:
  pyrec::Status OnForwardProcess(const ForwardIndexerRequest* request,
                                 IndexerReply* reply) override;
  pyrec::Status OnInvertedProcess(const InvertedIndexerRequest* request,
                                  IndexerReply* reply) override;

 private:
  explicit HashIndexerServer(pyrec::FieldIdType key_id = 0)
      : key_id_(key_id) {}
  int InsertItemFromCsvLine(const std::string& line,
                            const CsvFormat& format,
                            std::string* item_id,
                            const IndexItem** item);
  void InsertIntoInvertedIndexes(const std::string& item_key,
                                 const IndexItem* index_item);
  void FillReplyItem(
      const std::string& item_key,
      const IndexItem& index_item,
      const std::unordered_set<pyrec::FieldIdType>& requested_fields,
      IndexerReply* reply);
  void ProcessSingleSearchRequest(
      const pyrec::service::SearchRequest& search_request,
      std::unordered_set<std::string>* result_keys,
      int max_num);

 private:
  pyrec::FieldIdType key_id_;  // the field id of key
  std::unordered_map<std::string, IndexItem> forward_index_;
  std::unordered_map<pyrec::FieldIdType, InvertedIndex> inverted_indexes_;
};

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_INDEXER_HASH_INDEXER_H_
