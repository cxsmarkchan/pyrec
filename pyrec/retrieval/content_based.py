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
"""Interfaces of content-based retrieval server."""
from pyrec.pywrap_core import CBRetrievalServerInterface

from pyrec.retrieval.retrieval_base import RetrievalServicePyBase
from pyrec.indexer.indexer_client import make_internal_indexer_client

class CBRetrievalServer(RetrievalServicePyBase):
  """Server of the retrieval service system."""
  def __init__(self):
    """Init function."""
    super().__init__()
    self._server = None
    self._retrieval_id = 0
    self._request_num = 0
    self._filter_rule = dict()
    self._extract_key = None
    self._indexer = None

  def set_retrieval_id(self, retrieval_id):
    """
    The retrieval id is used to identify the retrieval algorithm.
    :param retrieval_id:
    :return:
    """
    assert isinstance(retrieval_id, int)
    self._retrieval_id = retrieval_id
    return self

  def set_request_num(self, request_num):
    """
    The maximal number of items returned.
    :param request_num:
    :return:
    """
    assert isinstance(request_num, int)
    self._request_num = request_num
    return self

  def set_item_keys(self, item_keys):
    """
    Set the filter rules, which are used to retrieve only part of the items
    from the index.
    :param filter_rule:
    :return:
    """
    if isinstance(item_keys, int):
      self._item_keys = [item_keys]
    elif isinstance(item_keys, list):
      self._item_keys = item_keys
    else:
      raise TypeError(item_keys)

    return self


  def set_filter_rule(self, filter_rule):
    """
    Set the filter rules, which are used to retrieve only part of the items
    from the index.
    :param filter_rule:
    :return:
    """
    assert isinstance(filter_rule, dict)
    self._filter_rule = filter_rule
    return self

  def set_extract_key(self, extract_key):
    """
    Set the key used to retrieve items from the inverted index.
    :param extract_key:
    :return:
    """
    assert isinstance(extract_key, list)
    self._extract_key = extract_key
    return self

  def set_indexer(self, indexer):
    """
    Set the indexer containing all the items.
    """
    self._indexer = make_internal_indexer_client(indexer)
    return self

  def _create_server_impl(self):
    """
    Create the content-based retrieval server
    :return:
    """
    assert self._indexer is not None
    assert self._extract_key is not None

    self._server = CBRetrievalServerInterface()
    self._server.SetRetrievalId(self._retrieval_id) \
        .SetRequestNum(self._request_num) \
        .SetExtractKey(self._extract_key[0], self._extract_key[1]) \
        .SetIndexer(self._indexer)

    for key, value in self._filter_rule.items():
      self._server.AddFilterRule(key, value)

    for key in self._item_keys:
      self._server.AddItemKey(key)

    return self._server.Create() == 0

  def get_server(self):
    """
    Only used for transporting the server to a local client.
    :return: a reference of the server.
    """
    return self._server
