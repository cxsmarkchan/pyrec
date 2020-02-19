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
"""PyRec context client interfaces."""
import grpc

from pyrec.proto.recommend_pb2 import PyRecRequest
from pyrec.proto.recommend_pb2_grpc import ContextServiceStub
from pyrec.pywrap_core import ContextClientInternal
from pyrec.pywrap_core import LocalContextClientInternal
from pyrec.pywrap_core import RemoteContextClientInternal
from pyrec.context.context_base import ContextServicePyBase

from pyrec.service.ip import Address


class ContextClient:
  """Client of the context service system."""
  def __init__(self, address):
    """
    :param address: the ip and port of server
    Currently no actions in the init function.
    """
    assert isinstance(address, Address)
    self._address = address

  @property
  def address(self):
    # pylint: disable=missing-function-docstring
    return self._address

  def request_context(self, request):
    """
    Send request to the context server, and obtain the response.
    :param request: the proto of request
    :return: the proto of response
    """
    assert isinstance(request, PyRecRequest)
    channel = grpc.insecure_channel(self._address.to_string())
    stub = ContextServiceStub(channel)
    response = stub.Context(request)
    return response

class InternalContextClientMaker:
  """Make the context client for internal c++ call."""
  @staticmethod
  def can_make(service):
    """
    :param service: Any object
    :return: whether this object can be transformed into
              an internal context client.
    """
    return isinstance(service, (ContextClientInternal, ContextClient)) \
           or (isinstance(service, ContextServicePyBase) and service.created)

  @staticmethod
  def make_client(service):
    """
    construct an internal indexer client for other pyrec services to call.
    :param service: A pyrec.context.ContextClient object
                    or a ContextService object
    :return: an ContextClientInternal object.
    """
    if isinstance(service, ContextClientInternal):
      return service
    if isinstance(service, ContextServicePyBase):
      return LocalContextClientInternal(service.get_server())
    if isinstance(service, ContextClient):
      address = service.address
      return RemoteContextClientInternal(address.ip, address.port)
    raise TypeError(service)
