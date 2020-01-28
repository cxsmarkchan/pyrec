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
"""Utils of IP address."""
class Address:
  """Deal with ip and port."""
  def __init__(self, ip, port):
    """
    :param ip: ip.
    :param port: port.
    """
    assert isinstance(ip, str)
    assert isinstance(port, int)
    self._ip = ip
    self._port = port

  @property
  def ip(self):  # pylint: disable=invalid-name
    # pylint: disable=missing-docstring
    return self._ip

  @property
  def port(self):
    # pylint: disable=missing-docstring
    return self._port

  def to_string(self):
    """
    :return: the "ip:port"-formatted string.
    """
    return '%s:%d' % (self._ip, self._port)
