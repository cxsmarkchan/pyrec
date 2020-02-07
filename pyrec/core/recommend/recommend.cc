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

#include "pyrec/core/recommend/recommend.h"

#include <boost/format.hpp>

namespace pyrec {
namespace service {

pyrec::Status RecommendServer::OnRecommendProcess(const PyRecRequest* request,
                                                  ItemReply* reply) {
  reply->set_request_id(boost::str(boost::format("%s-%s")
        % request->request_id()
        % request->request_id()));
  return pyrec::Status::OK;
}

}  // namespace service
}  // namespace pyrec
