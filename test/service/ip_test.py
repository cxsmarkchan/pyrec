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
"""Test cases of the package pyrec.service.ip."""

import unittest
from pyrec.service.ip import Address

class TestAddress(unittest.TestCase):
  """Test cases of class Address."""
  def test_address(self):
    """Test cases of some simple functions."""
    address = Address('0.1.2.3', 4567)
    self.assertEqual(address.ip, '0.1.2.3')
    self.assertEqual(address.port, 4567)
    self.assertEqual(address.to_string(), '0.1.2.3:4567')
