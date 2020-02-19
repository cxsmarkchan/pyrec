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
"""Test cases of tools.code_gen.hlrpc."""

import os
import unittest

from tools.code_gen import hlrpc


class TestHlrpcCodeGenerator(unittest.TestCase):
  """Test of HlrpcCodeGenerator."""
  def test_code_generator(self):
    """test of code generator."""
    resource_path = os.path.join(os.path.dirname(__file__), 'resources')
    with open(os.path.join(resource_path, 'hlrpc_test.proto')) as fp_src:
      proto_code = fp_src.read()
    with open(os.path.join(resource_path, 'hlrpc_test.hlrpc.pb.h')) as fp_dst:
      object_code = fp_dst.read()
    grammar_tree = hlrpc._HLRPC_PARSER.parse(proto_code)  # pylint: disable=protected-access
    code_generator = hlrpc.HlrpcCodeGenerator(
        grammar_tree,
        'path/to/proto/hlrpc_test')

    generated_code = code_generator.to_cpp_code()
    generated_code_lines = generated_code.split('\n')
    object_code_lines = object_code.split('\n')
    if len(object_code_lines) > len(generated_code_lines):
      generated_code_lines.extend(
          [''] * (len(object_code_lines) - len(generated_code_lines))
      )
    for i, object_code_line in enumerate(object_code_lines):
      self.assertEqual(generated_code_lines[i], object_code_line)


class TestAuxiliaryFunctions(unittest.TestCase):
  """Test of some auxiliary functions of the package."""
  def test_get_path_without_suffix(self):
    """test of _get_path_without_suffix."""
    # pylint: disable=protected-access
    self.assertEqual(
        hlrpc._get_path_without_suffix('/path/to/proto'),
        None
    )
    self.assertEqual(
        hlrpc._get_path_without_suffix('/path/to/proto.proto'),
        '/path/to/proto'
    )
    # pylint: enable=protected-access

  def test_add_indent(self):
    """Test of _add_indent."""
    origin_code = '  code line a;\n' \
                  '    code line b;\n' \
                  'code line c;\n' \
                  '\n' \
                  '  code line d;\n'
    dst_code = '    code line a;\n' \
                  '      code line b;\n' \
                  '  code line c;\n' \
                  '\n' \
                  '    code line d;\n'
    # pylint: disable=protected-access
    self.assertEqual(hlrpc._add_indent(origin_code, 2), dst_code)
    # pylint: enable=protected-access
