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
"""PyRec Retrieval client interfaces."""
import grpc

from pyrec.pywrap_core import RetrievalClientInternal
from pyrec.pywrap_core import LocalRetrievalClientInternal
from pyrec.pywrap_core import RemoteRetrievalClientInternal

from pyrec.proto.recommend_pb2 import PyRecRequest
from pyrec.proto import recommend_pb2_grpc

from pyrec.retrieval.retrieval_base import RetrievalServicePyBase
from pyrec.service.ip import Address


class RetrievalClient:
  """Client of the retrieval service system."""
  def __init__(self, address):
    """
    :param address: the ip and port of server
    """
    assert isinstance(address, Address)
    self._address = address

  @property
  def address(self):
    # pylint: disable=missing-function-docstring
    return self._address

  def request_retrieval(self, request):
    """
    Send request to the recommend server, and obtain the response.
    :param request: the proto of request
    :return: the proto of response
    """
    assert isinstance(request, PyRecRequest)
    channel = grpc.insecure_channel(self._address.to_string())
    stub = recommend_pb2_grpc.RetrievalServiceStub(channel)
    response = stub.Retrieval(request)
    return response


def make_internal_retrieval_client(retrieval):
  """
  construct an internal retrieval client for other pyrec services to call.
  :param indexer: A pyrec.retrieval.RetrievalClient object
                  or a RetrievalService object
  :return: an RetrievalClientInternal object.
  """
  if isinstance(retrieval, RetrievalClientInternal):
    return retrieval
  if isinstance(retrieval, RetrievalServicePyBase):
    return LocalRetrievalClientInternal(retrieval.get_server())
  if isinstance(retrieval, RetrievalClient):
    address = retrieval.address
    return RemoteRetrievalClientInternal(address.ip, address.port)
  raise TypeError(retrieval)
