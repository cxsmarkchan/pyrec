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
"""Test cases of the package pyrec.util.types."""

import unittest
from pyrec.util.types import CsvFormat

class TestCsvFormat(unittest.TestCase):
  """Test cases of class CsvFormat."""
  def test_to_core_type(self):
    """Test cases of function to_core_type."""
    csv_format = CsvFormat(field_ids=[101, 102, 103],
                           between_delimiter=';',
                           inner_delimiter=':')
    csv_format_core = csv_format.to_core_type()
    self.assertEqual(csv_format_core.DebugString(),
                     'between_delimiter: ";", inner_delimiter: ":", '
                     'field_ids: 101 102 103')
