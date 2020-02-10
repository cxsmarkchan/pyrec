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
"""Interfaces of HashIndexerServer."""
from pyrec import pywrap_core
from pyrec.util.types import CsvFormat

from pyrec.indexer.indexer_base import IndexerServicePyBase

class HashIndexerServer(IndexerServicePyBase):
  """Server of the recommender service system."""
  def __init__(self):
    """Init function."""
    self._server = pywrap_core.HashIndexerServerInterface()
    self._created = False

  @property
  def created(self):
    """
    :return: whether the server has been created
    """
    return self._created

  def create_server(self, csv_name, csv_format):
    """
    Create the hash indexer server
    :param csv_name: the csv file which contains the item information.
    :param csv_format: the csv format
    :return:
    """
    assert isinstance(csv_name, str)
    assert isinstance(csv_format, CsvFormat)
    ret = self._server.CreateFromCsv(csv_name, csv_format.to_core_type())
    if ret == 0:
      self._created = True

  def index_size(self):
    """
    :return: Number of items.
    """
    return self._server.IndexSize()

  def get_server(self):
    """
    Only used for transporting the server to a local client.
    :return: a reference of the server.
    """
    return self._server
