# Copyright 2020 The PyRec Authors. All Rights Reserved.
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

from pyrec.indexer.hash_indexer import HashIndexerServer
from pyrec.util.types import CsvFormat

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
    self._csv_format = CsvFormat(field_ids=[101, 102, 103],
                                 between_delimiter=',',
                                 inner_delimiter=':')

  def test_create(self):
    """Test cases of some create."""
    self._server.create_server(self._test_csv_file_name + 'wrong',
                               self._csv_format)
    self.assertEqual(self._server.created, False)

    self._server.create_server(self._test_csv_file_name,
                               self._csv_format)
    self.assertEqual(self._server.created, True)
    self.assertEqual(self._server.index_size(), 5)
