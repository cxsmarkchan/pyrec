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
"""PyRec client interfaces."""
import grpc

from pyrec.proto.service_pb2 import PyRecRequest
from pyrec.proto import service_pb2_grpc

from pyrec.service.ip import Address


class RecommendClient:
  """Client of the recommender service system."""
  def __init__(self):
    """
    Currently no actions in the init function.
    """
    return

  def send_request(self, address, request):
    """
    Send request to the recommend server, and obtain the response.
    :param address: the ip and port of server
    :param request: the proto of request
    :return: the proto of response
    """
    assert isinstance(address, Address)
    assert isinstance(request, PyRecRequest)
    channel = grpc.insecure_channel(address.to_string())
    stub = service_pb2_grpc.RecommendServiceStub(channel)
    response = stub.OnServing(request)
    return response
