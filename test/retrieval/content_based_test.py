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
"""Test cases of the package pyrec.retrieval.content_based."""

import unittest
import random
import threading

from pyrec.pywrap_core import RetrievalClientInternal
from pyrec.pywrap_core import LocalRetrievalClientInternal
from pyrec.pywrap_core import RemoteRetrievalClientInternal
from pyrec.service.ip import Address
from pyrec.util.types import CsvFormat
from pyrec.indexer.hash_indexer import HashIndexerServer
from pyrec.indexer.indexer_client import IndexerClient
from pyrec.retrieval.content_based import CBRetrievalServer
from pyrec.retrieval.retrieval_client import RetrievalClient
from pyrec.retrieval.retrieval_client import make_internal_retrieval_client
from pyrec.proto.recommend_pb2 import PyRecRequest


class TestCBRetrievalServer(unittest.TestCase):
  """Test cases of class HashIndexerServer."""
  def setUp(self):
    """Initialize the server and the csv file."""
    self._indexer = HashIndexerServer()
    test_csv_file_name = 'test_%d.csv' % random.randint(0, 1000000)
    with open(test_csv_file_name, 'w') as fp_csv:
      fp_csv.write('key1,101_1,102_1:102_2:102_3,103_1\n')
      fp_csv.write('key2,101_2,102_3:102_4:102_5,103_2\n')
      fp_csv.write('key3,101_2,102_1:102_2:102_5,103_1\n')
      fp_csv.write('key4,101_1,102_3,103_2\n')
      fp_csv.write('key5,101_2,102_2:102_4\n')
      fp_csv.write(',101_2,102_2:102_4,103_1\n')
    csv_format = CsvFormat(field_ids=[100, 101, 102, 103],
                           between_delimiter=',',
                           inner_delimiter=':')
    self._indexer.set_input_csv(test_csv_file_name, csv_format) \
        .create_server()
    self._cb_retrieval_server = CBRetrievalServer()
    self._server_address = Address('127.0.0.1', 21358)

  def test_entire_workflow(self):
    """Test cases of the entire workflow."""
    def start_server():
      self._cb_retrieval_server.set_filter_rule({
          102: '102_3'
      }).set_retrieval_id(1045) \
          .set_request_num(10) \
          .set_extract_key([201, 103]) \
          .set_item_keys([100]) \
          .set_indexer(self._indexer) \
          .run(self._server_address)

    server_thread = threading.Thread(target=start_server)
    server_thread.setDaemon(True)
    server_thread.start()

    client = RetrievalClient(self._server_address)
    request = PyRecRequest(request_id='123'.encode())
    feature_map = request.context[201].map_items
    feature_list = feature_map[103].bytes_list
    feature_list.values.append('103_2'.encode())
    response = client.request_retrieval(request)

    self.assertEqual(response.request_id, '123'.encode())
    self.assertEqual(len(response.items), 2)
    expected_items = [key.encode() for key in ['key2', 'key4']]
    for item in response.items:
      self.assertTrue(item.item_id.map_items[100].bytes_list.values[0]
                      in expected_items)
      self.assertEqual(len(item.retrieval_infos), 1)
      self.assertEqual(item.retrieval_infos[0].retrieval_id, 1045)
      self.assertEqual(item.retrieval_infos[0].num_items, 2)


class TestClientInternal(unittest.TestCase):
  """Test cases of the interface to internal client."""
  def test_make_internal_retrieval_client(self):  # pylint: disable=invalid-name
    """Test cases of make_internal_retrieval_client."""
    address = Address('127.0.0.1', 12358)
    server = CBRetrievalServer()
    server.set_filter_rule({
        102: '102_3'
    }).set_retrieval_id(1045) \
        .set_request_num(10) \
        .set_extract_key([201, 103]) \
        .set_item_keys([100]) \
        .set_indexer(IndexerClient(address)) \
        .create_server()

    internal_client = make_internal_retrieval_client(server)
    self.assertTrue(isinstance(internal_client, LocalRetrievalClientInternal))
    self.assertTrue(isinstance(internal_client, RetrievalClientInternal))

    client = RetrievalClient(address)
    internal_client = make_internal_retrieval_client(client)
    self.assertTrue(isinstance(internal_client, RemoteRetrievalClientInternal))
    self.assertTrue(isinstance(internal_client, RetrievalClientInternal))
