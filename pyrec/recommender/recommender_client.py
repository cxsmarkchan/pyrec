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
"""PyRec recommender client interfaces."""
import grpc

from pyrec.proto.recommend_pb2 import PyRecRequest
from pyrec.proto.recommend_pb2_grpc import RecommenderServiceStub
from pyrec.pywrap_core import RecommenderClientInternal
from pyrec.pywrap_core import LocalRecommenderClientInternal
from pyrec.pywrap_core import RemoteRecommenderClientInternal
from pyrec.recommender.recommender_base import RecommenderServicePyBase

from pyrec.service.ip import Address


class RecommenderClient:
  """Client of the recommender service system."""
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

  def request_recommend(self, request):
    """
    Send request to the recommender server, and obtain the response.
    :param request: the proto of request
    :return: the proto of response
    """
    assert isinstance(request, PyRecRequest)
    channel = grpc.insecure_channel(self._address.to_string())
    stub = RecommenderServiceStub(channel)
    response = stub.Recommend(request)
    return response


class InternalRecommenderClientMaker:
  """Make the recommender client for internal C++ call."""
  @staticmethod
  def can_make(service):
    """
    :param service: Any object
    :return: whether this object can be transformed into
              an internal recommender client.
    """
    return isinstance(service, (RecommenderClientInternal, RecommenderClient)) \
           or (isinstance(service, RecommenderServicePyBase)
               and service.created)

  @staticmethod
  def make_client(recommender):
    """
    construct an internal recommender client for other pyrec services to call.
    :param indexer: A pyrec.recommender.RecommenderClient object
                    or a RecommenderService object
    :return: an RecommenderClientInternal object.
    """
    if isinstance(recommender, RecommenderClientInternal):
      return recommender
    if isinstance(recommender, RecommenderServicePyBase):
      return LocalRecommenderClientInternal(recommender.get_server())
    if isinstance(recommender, RecommenderClient):
      address = recommender.address
      return RemoteRecommenderClientInternal(address.ip, address.port)
    raise TypeError(recommender)
