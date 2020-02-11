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

from pyrec.pywrap_core import HashIndexerServerInterface
from pyrec.util.types import CsvFormat

from pyrec.indexer.indexer_base import IndexerServicePyBase


class HashIndexerServer(IndexerServicePyBase):
  """Server of the recommender service system."""
  INPUT_UNKNOWN = 0
  INPUT_CSV = 1
  def __init__(self):
    """Init function."""
    super().__init__()
    self._server = None
    self._input_type = self.INPUT_UNKNOWN
    self._csv_name = ''
    self._csv_format = CsvFormat(field_ids=[])

  def set_input_csv(self, csv_name, csv_format):
    """;"""
    assert isinstance(csv_name, str)
    assert isinstance(csv_format, CsvFormat)
    self._csv_name = csv_name
    self._csv_format = csv_format
    self._input_type = self.INPUT_CSV
    return self

  def _create_server_from_csv(self):
    """
    Create a hash indexer from a csv file.
    :return: True if create successfully, and False otherwise.
    """
    assert self._input_type == self.INPUT_CSV
    assert len(self._csv_name) > 0
    self._server = HashIndexerServerInterface()
    ret = self._server.CreateFromCsv(self._csv_name,
                                     self._csv_format.to_core_type())
    return ret == 0

  def _create_server_impl(self):
    """
    Create the hash indexer server
    :return:
    """
    create_function_dict = {
        self.INPUT_CSV: self._create_server_from_csv
    }
    assert self._input_type in create_function_dict
    ret = create_function_dict[self._input_type]()
    self._created = ret
    return ret

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
