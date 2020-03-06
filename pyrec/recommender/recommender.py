# Copyright 2020 cxsmarkchan. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Interfaces of RecommenderServer."""

from pyrec.pywrap_core import RecommenderServerInterface

from pyrec.retrieval.retrieval_client import InternalRetrievalClientMaker
from pyrec.context.context_client import InternalContextClientMaker

from pyrec.recommender.recommender_base import RecommenderServicePyBase


class RecommenderServer(RecommenderServicePyBase):
  """Server of the recommender service system."""
  INPUT_UNKNOWN = 0
  INPUT_CSV = 1
  def __init__(self):
    """Init function."""
    super().__init__()
    self._server = None
    self._retrievals = []
    self._context = dict()
    self._request_num = 0

  def set_request_num(self, request_num):
    """
    Set the number of requested items.
    :param request_num:
    :return:
    """
    assert isinstance(request_num, int)
    assert request_num > 0
    self._request_num = request_num
    return self

  def add_retrieval(self, retrieval_server):
    """
    Add retrieval modules
    :param retrieval_server: the retrieval server.
    :return:
    """
    assert InternalRetrievalClientMaker.can_make(retrieval_server)
    self._retrievals.append(
        InternalRetrievalClientMaker.make_client(retrieval_server))
    return self

  def add_context(self, context_id, context_server):
    """
    Add context modules
    :param context_id: the id of the context module.
    :param context_server: the context server.
    :return:
    """
    assert InternalContextClientMaker.can_make(context_server)
    assert context_id not in self._context
    self._context[context_id] = \
        InternalContextClientMaker.make_client(context_server)
    return self

  def _create_server_impl(self):
    """
    Create the recommender server
    :return:
    """
    assert self._request_num > 0

    self._server = RecommenderServerInterface()
    for retrieval in self._retrievals:
      self._server.AddRetrieval(retrieval)
    for context_id, context_server in self._context.items():
      self._server.AddContext(context_id, context_server)
    self._server.SetRequestNum(self._request_num)

    ret = self._server.Create()
    return ret == 0

  def get_server(self):
    """
    Only used for transporting the server to a local client.
    :return: a reference of the server.
    """
    return self._server
