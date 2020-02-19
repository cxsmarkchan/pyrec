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
"""PyRec Indexer client interfaces."""
import grpc

from pyrec.proto.indexer_pb2 import ForwardIndexerRequest
from pyrec.proto.indexer_pb2 import InvertedIndexerRequest
from pyrec.proto.indexer_pb2_grpc import IndexerServiceStub
from pyrec.pywrap_core import IndexerClientInternal
from pyrec.pywrap_core import LocalIndexerClientInternal
from pyrec.pywrap_core import RemoteIndexerClientInternal
from pyrec.indexer.indexer_base import IndexerServicePyBase

from pyrec.service.ip import Address


class IndexerClient:
  """Client of the retrieval service system."""
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

  def request_forward(self, request):
    """
    Send request to the indexer server, and obtain the response.
    :param request: the proto of request
    :return: the proto of response
    """
    assert isinstance(request, ForwardIndexerRequest)
    channel = grpc.insecure_channel(self._address.to_string())
    stub = IndexerServiceStub(channel)
    response = stub.Forward(request)
    return response

  def request_inverted(self, request):
    """
    Send request to the indexer server, and obtain the response.
    :param request: the proto of request
    :return: the proto of response
    """
    assert isinstance(request, InvertedIndexerRequest)
    channel = grpc.insecure_channel(self._address.to_string())
    stub = IndexerServiceStub(channel)
    response = stub.Inverted(request)
    return response


class InternalIndexerClientMaker:
  """Make the indexer client for internal C++ call."""
  @staticmethod
  def can_make(service):
    """
    :param service: Any object
    :return: whether this object can be transformed into
              an internal indexer client.
    """
    return isinstance(service, (IndexerClientInternal, IndexerClient)) \
           or (isinstance(service, IndexerServicePyBase) and service.created)

  @staticmethod
  def make_client(indexer):
    """
    construct an internal indexer client for other pyrec services to call.
    :param indexer: A pyrec.indexer.IndexerClient object
                    or a IndexerService object
    :return: an IndexerClientInternal object.
    """
    if isinstance(indexer, IndexerClientInternal):
      return indexer
    if isinstance(indexer, IndexerServicePyBase):
      return LocalIndexerClientInternal(indexer.get_server())
    if isinstance(indexer, IndexerClient):
      address = indexer.address
      return RemoteIndexerClientInternal(address.ip, address.port)
    raise TypeError(indexer)
