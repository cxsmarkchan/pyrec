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
"""Test cases of the package pyrec.indexer.hash_indexer."""

import unittest
import random
import threading

from pyrec.util.types import CsvFormat
from pyrec.service.ip import Address

from pyrec.proto.indexer_pb2 import ForwardIndexerRequest
from pyrec.proto.indexer_pb2 import InvertedIndexerRequest
from pyrec.proto.indexer_pb2 import SearchRequest

from pyrec.pywrap_core import IndexerClientInternal
from pyrec.pywrap_core import LocalIndexerClientInternal
from pyrec.pywrap_core import RemoteIndexerClientInternal

from pyrec.indexer.hash_indexer import HashIndexerServer
from pyrec.indexer.indexer_client import IndexerClient
from pyrec.indexer.indexer_client import make_internal_indexer_client


class TestHashIndexer(unittest.TestCase):
  """Test cases of class HashIndexerServer."""
  def setUp(self):
    """Initialized the server and the csv file."""
    self._server = HashIndexerServer()
    self._test_csv_file_name = 'test_%d.csv' % random.randint(0, 1000000)
    with open(self._test_csv_file_name, 'w') as fp_csv:
      fp_csv.write('key1,101_1,102_1:102_2:102_3,103_1\n')
      fp_csv.write('key2,101_2,102_3:102_4:102_5,103_2\n')
      fp_csv.write('key3,101_2,102_1:102_2:102_5,103_1\n')
      fp_csv.write('key4,101_1,102_3,103_2\n')
      fp_csv.write('key5,101_2,102_2:102_4\n')
      fp_csv.write(',101_2,102_2:102_4,103_1\n')
    self._csv_format = CsvFormat(field_ids=[100, 101, 102, 103],
                                 between_delimiter=',',
                                 inner_delimiter=':')

  def test_create(self):
    """Test cases of some create."""
    self._server.set_input_csv(self._test_csv_file_name + 'wrong',
                               self._csv_format) \
        .create_server()
    self.assertEqual(self._server.created, False)

    self._server.set_input_csv(self._test_csv_file_name,
                               self._csv_format) \
        .create_server()
    self.assertEqual(self._server.created, True)
    self.assertEqual(self._server.index_size(), 5)

  def test_entire_workflow(self):
    """Test the entire workflow of hash indexer."""
    address = Address('127.0.0.1', 12358)
    def start_server():
      self._server.set_input_csv(self._test_csv_file_name,
                                 self._csv_format) \
          .run(address)

    server_thread = threading.Thread(target=start_server)
    server_thread.setDaemon(True)
    server_thread.start()

    client = IndexerClient(address)

    forward_request = ForwardIndexerRequest(key_id=100)
    forward_request.keys.append('key1'.encode())
    forward_request.requested_fields.append(100)
    forward_request.requested_fields.append(101)

    response = client.request_forward(forward_request)
    self.assertEqual(len(response.items), 1)
    self.assertEqual(
        response.items[0].fields.map_items[100].bytes_list.values[0],
        'key1'.encode())
    self.assertEqual(response.items[0].fields.map_items[101]
                     .bytes_list.values[0], '101_1'.encode())

    inverted_request = InvertedIndexerRequest(max_num=10)
    search_request = SearchRequest()
    search_request.search_items[101].match_search_item \
        .bytes_value = '101_1'.encode()
    search_request.search_items[103].match_search_item \
        .bytes_value = '103_1'.encode()
    inverted_request.search_requests.append(search_request)
    inverted_request.requested_fields.append(100)
    inverted_request.requested_fields.append(101)
    response = client.request_inverted(inverted_request)
    self.assertEqual(len(response.items), 1)
    self.assertEqual(
        response.items[0].fields.map_items[100].bytes_list.values[0],
        'key1'.encode())
    self.assertEqual(
        response.items[0].fields.map_items[101].bytes_list.values[0],
        '101_1'.encode())


class TestClientInternal(unittest.TestCase):
  """Test cases of the interface to internal client."""
  def test_make_internal_indexer_client(self):  # pylint: disable=invalid-name
    """Test cases of make_internal_indexer_client."""
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
    server = HashIndexerServer()
    server.set_input_csv(test_csv_file_name, csv_format) \
        .create_server()

    internal_client = make_internal_indexer_client(server)
    self.assertTrue(isinstance(internal_client, LocalIndexerClientInternal))
    self.assertTrue(isinstance(internal_client, IndexerClientInternal))

    client = IndexerClient(Address('127.0.0.1', 12358))
    internal_client = make_internal_indexer_client(client)
    self.assertTrue(isinstance(internal_client, RemoteIndexerClientInternal))
    self.assertTrue(isinstance(internal_client, IndexerClientInternal))
