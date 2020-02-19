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
"""Test cases of the package pyrec.context.kv_context."""

import unittest
import random
import threading

from pyrec.service.ip import Address
from pyrec.util.types import CsvFormat

from pyrec.proto.recommend_pb2 import PyRecRequest

from pyrec.pywrap_core import ContextClientInternal
from pyrec.pywrap_core import LocalContextClientInternal
from pyrec.pywrap_core import RemoteContextClientInternal

from pyrec.context.kv_context import KvContextServer
from pyrec.context.context_client import ContextClient
from pyrec.context.context_client import InternalContextClientMaker

from pyrec.indexer.hash_indexer import HashIndexerServer
from pyrec.indexer.indexer_client import IndexerClient

class TestKvContextServer(unittest.TestCase):
  """Test cases of class KvContextServer."""
  def setUp(self):
    """Initialized the server."""
    self._server = KvContextServer()
    self._test_csv_file_name = 'test_%d.csv' % random.randint(0, 1000000)
    with open(self._test_csv_file_name, 'w') as fp_csv:
      fp_csv.write('user1,101_1,102_1:102_2:102_3,103_1\n')
      fp_csv.write('user2,101_2,102_3:102_4:102_5,103_2\n')
      fp_csv.write('user3,101_2,102_1:102_2:102_5,103_1\n')
      fp_csv.write('user4,101_1,102_3,103_2\n')
      fp_csv.write('user5,101_2,102_2:102_4\n')
    self._csv_format = CsvFormat(field_ids=[100, 101, 102, 103],
                                 between_delimiter=',',
                                 inner_delimiter=':')
    self._server_address = Address('127.0.0.1', 12359)
    self._indexer_server = HashIndexerServer()
    self._indexer_server \
        .set_input_csv(self._test_csv_file_name, self._csv_format) \
        .create_server()


  def test_entire_workflow(self):
    """Test the entire workflow of kv context."""
    def start_server():
      self._server.set_key_id([100, 100]) \
        .set_fields([101, 102]) \
        .set_kv_server(self._indexer_server) \
        .run(self._server_address)

    server_thread = threading.Thread(target=start_server)
    server_thread.setDaemon(True)
    server_thread.start()

    client = ContextClient(self._server_address)

    request = PyRecRequest(request_id='123'.encode())
    feature_map = request.context[100].map_items
    feature_list = feature_map[100].bytes_list
    feature_list.values.append('user1'.encode())
    response = client.request_context(request)

    self.assertEqual(response.request_id, '123'.encode())
    context_map = response.context.map_items
    self.assertEqual(len(context_map), 2)
    self.assertEqual(context_map[101].bytes_list.values, ['101_1'.encode()])
    self.assertEqual(context_map[102].bytes_list.values,
                     [s.encode() for s in ['102_1', '102_2', '102_3']])


class TestClientInternal(unittest.TestCase):
  """Test cases of the interface to internal client."""
  def test_make_internal_context_client(self):  # pylint: disable=invalid-name
    """Test cases of make_internal_context_client."""
    address = Address('127.0.0.1', 12358)
    server = KvContextServer()
    server.set_key_id([100, 100]) \
        .set_kv_server(IndexerClient(address))

    self.assertFalse(InternalContextClientMaker.can_make(server))
    server.create_server()
    self.assertTrue(InternalContextClientMaker.can_make(server))
    internal_client = InternalContextClientMaker.make_client(server)
    self.assertTrue(isinstance(internal_client, LocalContextClientInternal))
    self.assertTrue(isinstance(internal_client, ContextClientInternal))

    client = ContextClient(address)
    self.assertTrue(InternalContextClientMaker.can_make(client))
    internal_client = InternalContextClientMaker.make_client(client)
    self.assertTrue(isinstance(internal_client, RemoteContextClientInternal))
    self.assertTrue(isinstance(internal_client, ContextClientInternal))
