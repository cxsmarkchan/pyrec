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

#ifndef PYREC_CORE_INDEXER_HASH_INDEXER_H_
#define PYREC_CORE_INDEXER_HASH_INDEXER_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <fstream>

#include "core/util/status.h"
#include "core/util/types.h"
#include "pyrec/proto/indexer.hlrpc.pb.h"
#include "core/indexer/indexer_interface.h"

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
  typedef std::unordered_map<pyrec::types::FieldId, IndexField> IndexItem;
  typedef std::vector<std::string> InvertedItems;
  typedef std::unordered_map<std::string, InvertedItems> InvertedIndex;

  static std::shared_ptr<HashIndexerServer> CreateFromCsv(
      std::istream& stream,
      const pyrec::types::CsvFormat& format);

 public:
  size_t IndexSize() const {
    return forward_index_.size();
  }

  pyrec::Status ForwardProcess(const ForwardIndexerRequest* request,
                               IndexerReply* reply) override;
  pyrec::Status InvertedProcess(const InvertedIndexerRequest* request,
                                  IndexerReply* reply) override;

 private:
  explicit HashIndexerServer(pyrec::types::FieldId key_id = 0)
      : key_id_(key_id) {}
  int InsertItemFromCsvLine(const std::string& line,
                            const pyrec::types::CsvFormat& format,
                            std::string* item_id,
                            const IndexItem** item);
  void InsertIntoInvertedIndexes(const std::string& item_key,
                                 const IndexItem* index_item);
  void FillReplyItem(
      const std::string& item_key,
      const IndexItem& index_item,
      const std::unordered_set<pyrec::types::FieldId>& requested_fields,
      IndexerReply* reply);
  void ProcessSingleSearchRequest(
      const pyrec::service::SearchRequest& search_request,
      std::unordered_set<std::string>* result_keys,
      int max_num);

 private:
  pyrec::types::FieldId key_id_;  // the field id of key
  std::unordered_map<std::string, IndexItem> forward_index_;
  std::unordered_map<pyrec::types::FieldId, InvertedIndex> inverted_indexes_;
};

class HashIndexerServerInterface : public IndexerServiceInterface {
 public:
  int CreateFromCsv(const char* file_name,
      const pyrec::types::CsvFormat& format) {
    std::ifstream stream(file_name);
    if (!stream)
      return -1;
    server_ = HashIndexerServer::CreateFromCsv(stream, format);
    if (server_ == nullptr)
      return -1;
    return 0;
  }

  size_t IndexSize() const {
    return std::dynamic_pointer_cast<HashIndexerServer>(server_)->IndexSize();
  }
};  // class HashIndexerServerInterface

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_INDEXER_HASH_INDEXER_H_
