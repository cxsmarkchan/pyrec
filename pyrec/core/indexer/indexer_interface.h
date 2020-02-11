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

#ifndef PYREC_CORE_INDEXER_INDEXER_INTERFACE_H_
#define PYREC_CORE_INDEXER_INDEXER_INTERFACE_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <fstream>

#include "pyrec/proto/indexer.hlrpc.pb.h"

namespace pyrec {
namespace service {

class IndexerServiceInterface {
 public:
  IndexerServiceInterface() : server_(nullptr) {}

  int Run(const char* ip, int port) {
    if (!server_)
      return -1;
    return server_->Run({ip, port});
  }

  std::shared_ptr<IndexerServiceHybridBase> GetServer() const {
    return server_;
  }

 protected:
  std::shared_ptr<IndexerServiceHybridBase> server_;
};  // class IndexerServiceInterface

}  // namespace service
}  // namespace pyrec

#endif  // PYREC_CORE_INDEXER_INDEXER_INTERFACE_H_
