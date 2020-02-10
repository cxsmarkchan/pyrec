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
"""Base class of all indexer services."""

from abc import abstractmethod
from pyrec.service.ip import Address


class IndexerServicePyBase:
  """Base class of all indexer services."""
  def run(self, address):
    """
    Start the server.
    :param address: the address to monitor.
    :return: An integer code, determined by self._server.Run()
    """
    assert isinstance(address, Address)
    return self.get_server().Run(address.ip, address.port)

  @abstractmethod
  def get_server(self):
    """
    Only used for transporting the server to a local client.
    :return: a reference of the server.
    """
    return
