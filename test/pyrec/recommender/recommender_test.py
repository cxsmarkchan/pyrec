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
"""Test cases of the package pyrec.recommender.recommender."""

import unittest
import random
import threading

from pyrec.pywrap_core import RecommenderClientInternal
from pyrec.pywrap_core import LocalRecommenderClientInternal
from pyrec.pywrap_core import RemoteRecommenderClientInternal

from pyrec.proto.recommend_pb2 import PyRecRequest

from pyrec.service.ip import Address
from pyrec.util.types import CsvFormat

from pyrec.indexer.hash_indexer import HashIndexerServer
from pyrec.retrieval.content_based import CBRetrievalServer
from pyrec.context.kv_context import KvContextServer

from pyrec.recommender.recommender import RecommenderServer
from pyrec.recommender.recommender_client import RecommenderClient
from pyrec.recommender.recommender_client import InternalRecommenderClientMaker


class TestRecommenderServer(unittest.TestCase):
  """Test cases of class RecommenderServer."""
  def setUp(self):
    """Initialize the server and the csv file."""
    def get_retrieval_server():
      indexer = HashIndexerServer()
      test_csv_file_name = 'indexer_%d.csv' % random.randint(0, 1000000)
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
      indexer.set_input_csv(test_csv_file_name, csv_format) \
          .create_server()

      cb_retrieval_server = CBRetrievalServer()
      cb_retrieval_server.set_filter_rule({102: '102_1'}) \
          .set_retrieval_id(1045) \
          .set_request_num(10) \
          .set_extract_key([201, 101]) \
          .set_item_keys([100]) \
          .set_indexer(indexer) \
          .create_server()
      return cb_retrieval_server

    def get_context_server():
      context_server = KvContextServer()
      context_csv_file_name = 'context_%d.csv' % random.randint(0, 1000000)
      with open(context_csv_file_name, 'w') as fp_csv:
        fp_csv.write('user1,101_1,102_1:102_2:102_3,103_1\n')
        fp_csv.write('user2,101_2,102_3:102_4:102_5,103_2\n')
        fp_csv.write('user3,101_2,102_1:102_2:102_5,103_1\n')
        fp_csv.write('user4,101_1,102_3,103_2\n')
        fp_csv.write('user5,101_2,102_2:102_4\n')
      csv_format = CsvFormat(field_ids=[100, 101, 102, 103],
                             between_delimiter=',',
                             inner_delimiter=':')
      indexer_server = HashIndexerServer()
      indexer_server.set_input_csv(context_csv_file_name, csv_format) \
          .create_server()

      context_server.set_key_id([100, 100]) \
        .set_fields([101, 102]) \
        .set_kv_server(indexer_server) \
        .create_server()
      return context_server

    self._context_server = get_context_server()
    self._retrieval_server = get_retrieval_server()
    self._server_address = Address('127.0.0.1', 21368)
    self._recommender_server = RecommenderServer()

  def test_entire_workflow(self):
    """Test cases of the entire workflow."""
    def start_server():
      self._recommender_server.add_context(201, self._context_server) \
          .add_retrieval(self._retrieval_server) \
          .set_request_num(1) \
          .run(self._server_address)

    server_thread = threading.Thread(target=start_server)
    server_thread.setDaemon(True)
    server_thread.start()

    client = RecommenderClient(self._server_address)
    request = PyRecRequest(request_id='123'.encode())
    feature_map = request.context[100].map_items
    feature_list = feature_map[100].bytes_list
    feature_list.values.append('user1'.encode())
    response = client.request_recommend(request)

    self.assertEqual(response.request_id, '123'.encode())
    self.assertEqual(len(response.items), 1)
    expected_items = [key.encode() for key in ['key1']]
    for item in response.items:
      self.assertTrue(item.item_id.map_items[100].bytes_list.values[0]
                      in expected_items)
      self.assertEqual(len(item.retrieval_infos), 1)
      self.assertEqual(item.retrieval_infos[0].retrieval_id, 1045)
      self.assertEqual(item.retrieval_infos[0].num_items, 1)


class TestClientInternal(unittest.TestCase):
  """Test cases of the interface to internal client."""
  def test_make_internal_retrieval_client(self):  # pylint: disable=invalid-name
    """Test cases of make_internal_retrieval_client."""
    address = Address('127.0.0.1', 12368)
    server = RecommenderServer()
    server.set_request_num(1)

    self.assertFalse(InternalRecommenderClientMaker.can_make(server))
    server.create_server()
    self.assertTrue(InternalRecommenderClientMaker.can_make(server))
    internal_client = InternalRecommenderClientMaker.make_client(server)
    self.assertTrue(isinstance(internal_client, LocalRecommenderClientInternal))
    self.assertTrue(isinstance(internal_client, RecommenderClientInternal))

    client = RecommenderClient(address)
    self.assertTrue(InternalRecommenderClientMaker.can_make(client))
    internal_client = InternalRecommenderClientMaker.make_client(client)
    self.assertTrue(isinstance(internal_client,
                               RemoteRecommenderClientInternal))
    self.assertTrue(isinstance(internal_client, RecommenderClientInternal))
