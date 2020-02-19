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
"""Test cases of the package pyrec.service.ip."""

import unittest
import threading
from pyrec.service.ip import Address
from pyrec.service.server import RecommendServer
from pyrec.service.client import RecommendClient
from pyrec.proto.recommend_pb2 import PyRecRequest

class TestService(unittest.TestCase):
  """Test cases of RecommendServer and RecommendClient"""
  def test_service(self):
    """Test cases of RecommendServer and RecommendClient"""
    address = Address('127.0.0.1', 14659)

    def server_start():
      server = RecommendServer()
      server.run(address)
    server_thread = threading.Thread(
        target=server_start,
        name='RecommendServer')
    server_thread.setDaemon(True)
    server_thread.start()

    client = RecommendClient()
    request = PyRecRequest(request_id='123'.encode())
    response = client.request_recommend(address, request)
    self.assertEqual(response.request_id, '123-123'.encode())
