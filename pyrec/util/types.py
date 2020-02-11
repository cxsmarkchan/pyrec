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
"""Types used in PyRec."""
from pyrec import pywrap_core


class CsvFormat:
  """
  Format used when parsing a csv file.
  A corresponding C++ class is defined in pyrec/core/util/type.h
  """
  def __init__(self, field_ids, between_delimiter=',', inner_delimiter=''):
    """
    :param field_ids: a list showing the id(i.e. the name) of each fields.
                      Each item should be a positive integer
    :param between_delimiter: delimiter to separate different fields.
    :param inner_delimiter: delimiter to separate items within one field.
                            An empty string means no separator.
    """
    assert isinstance(field_ids, list)
    assert isinstance(between_delimiter, str)
    assert len(between_delimiter) > 0
    assert isinstance(inner_delimiter, str)

    self.field_ids = field_ids
    self.between_delimiter = between_delimiter
    self.inner_delimiter = inner_delimiter

  def to_core_type(self):
    """
    :return: a pywrap_core.CsvFormat type object for cpp codes.
    """
    core_csv_format = pywrap_core.CsvFormat()
    core_csv_format.field_ids = self.field_ids
    core_csv_format.between_delimiter = self.between_delimiter
    core_csv_format.inner_delimiter = self.inner_delimiter
    return core_csv_format
