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
"""Interfaces of KVContextServer."""

from pyrec.pywrap_core import KvContextServerInterface

from pyrec.context.context_base import ContextServicePyBase
from pyrec.indexer.indexer_client import InternalIndexerClientMaker


class KvContextServer(ContextServicePyBase):
  """Context server based on kv stores."""
  KV_TYPE_UNKNOWN = 0
  KV_TYPE_INDEXER = 1
  def __init__(self):
    """Init function."""
    super().__init__()
    self._server = None
    self._kv_type = self.KV_TYPE_UNKNOWN
    self._kv_indexer = None
    self._key_id = None
    self._fields = []

  def set_kv_server(self, server):
    """
    set the kv server, which can be an indexer currently.
    :param server:
    :return:
    """
    if InternalIndexerClientMaker.can_make(server):
      self._kv_indexer = InternalIndexerClientMaker.make_client(server)
      self._kv_type = self.KV_TYPE_INDEXER
    else:
      raise TypeError(server)
    return self

  def set_key_id(self, key_id):
    """
    The key id used to obtain user context.
    :param key_id:
    :return:
    """
    assert isinstance(key_id, list)
    assert len(key_id) == 2
    self._key_id = key_id
    return self

  def set_fields(self, fields):
    """
    The fields requested
    :param fields:
    :return:
    """
    assert isinstance(fields, list)
    self._fields = fields
    return self

  def _create_server_impl(self):
    """
    Create the kv context server
    :return:
    """
    assert self._key_id is not None

    self._server = KvContextServerInterface()
    self._server.SetKeyId(self._key_id[0], self._key_id[1])
    for field in self._fields:
      self._server.AddField(field)
    self._set_kv_server_internal()

    return self._server.Create() == 0

  def _set_kv_server_internal(self):
    """
    call the internal C++ code to set the server for kv store.
    :return: None.
    """
    if self._kv_type == self.KV_TYPE_INDEXER:
      self._server.SetIndexerKvServer(self._kv_indexer)
    else:
      raise ValueError(self._kv_type)


  def get_server(self):
    """
    Only used for transporting the server to a local client.
    :return: a reference of the server.
    """
    return self._server
