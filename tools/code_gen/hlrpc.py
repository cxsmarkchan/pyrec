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
"""
C++ code generator for hybrid local and remote process call (abbr. HLRPC).
It reads a proto file, extracts the rpc call,
and generates a class with hybrid local services and remote services.
"""

import sys
import os
from functools import reduce
from operator import add

import lark

# TODO(cxsmarkchan): An entire support of protobuf grammar.
# TODO(cxsmarkchan): Add comments in the generated code.

_HLRPC_PARSER = lark.Lark.open(
    os.path.join(os.path.dirname(__file__), 'proto.lark'),
    parser='lalr'
)

def _get_path_without_suffix(src_path):
  """
  Remove the .proto suffix.
  :param src_path: source file path, e.g. /xx/xx/xx.proto
  :return: file path without suffix, e.g. /xx/xx/xx
  """
  if src_path[-6:] != '.proto':
    return None
  return src_path[:-6]


def _add_indent(code, indent_num):
  """ Add indent to a bulk of code. """
  # TODO(cxsmarkchan): The efficiency of this function is low,
  #  try to improve it
  indent_str = ' ' * indent_num

  def _indented(code_line):
    return indent_str + code_line if len(code_line.strip()) > 0 else code_line

  code_lines = code.split('\n')
  code_lines = map(_indented, code_lines)
  return '\n'.join(code_lines)


class ScopedVariable:
  """Variable with namespaces."""
  def __init__(self, grammar_tree):
    """Init function."""
    if isinstance(grammar_tree, lark.lexer.Token):
      self._names = [str(grammar_tree)]
      return

    assert isinstance(grammar_tree, lark.tree.Tree)
    assert grammar_tree.data == 'variable'
    names = [ScopedVariable(child).name_list
             for child in grammar_tree.children]
    self._names = reduce(add, names)

  @property
  def name_list(self):
    # pylint: disable=missing-function-docstring
    return self._names

  def to_cpp_code(self):
    """
    from a.b.c to a::b::c
    :return: C++ formatted variable with namespace.
    """
    return '::'.join(self._names)


class RpcCodeGenerator:
  """Code generator of rpc statement.
  An rpc statement is:
    ``
    rpc RpcName(Request) returns(Response).
    ''
  The rpc statement will be transformed into C++ functions with:
  1. An rpc interface, i.e. RpcName in this example.
  2. A local interface, i.e., RpcNameLocal.
  3. A processing function shared by the remote and the local interface,
     i.e. RpcNameProcess.
  """
  def __init__(self, grammar_tree):
    """
    :param grammar_tree: grammar tree of an rpc statement.
    """
    assert isinstance(grammar_tree, lark.tree.Tree)
    assert grammar_tree.data == 'rpc'
    self._rpc_name = ScopedVariable(grammar_tree.children[0])
    self._request = ScopedVariable(grammar_tree.children[1])
    self._reply = ScopedVariable(grammar_tree.children[2])

  def to_cpp_code(self):
    """
    :return: C++ function of the rpc statement.
    """
    rpc = self._rpc_name.to_cpp_code()
    rpc_local = rpc + 'Local'
    rpc_process = rpc + 'Process'
    request = self._request.to_cpp_code()
    reply = self._reply.to_cpp_code()

    rpc_def = 'grpc::Status %s(' \
              'grpc::ServerContext* context, ' \
              'const %s* request, ' \
              '%s* reply) final {\n' \
              '  pyrec::Status status = %s(request, reply);\n' \
              '  return status.ToGrpcStatus();\n' \
              '}\n' % (rpc, request, reply, rpc_process)

    rpc_local_def = 'pyrec::Status %s(' \
                    'const %s* request, ' \
                    '%s* reply) {\n' \
                    '  return %s(request, reply);\n' \
                    '}\n' % (rpc_local, request, reply, rpc_process)

    rpc_process_def = 'virtual pyrec::Status %s(' \
                      'const %s* request, ' \
                      '%s* reply) = 0;\n' % (rpc_process, request, reply)

    return '\n'.join([rpc_def, rpc_local_def, rpc_process_def])


class ServiceCodeGenerator:
  """Code generator of a service definition.
  A service definition can be:
    ``
    service ServiceName {
      rpc Rpc1(Request1) returns(Reply1) {}
      rpc Rpc2(Request2) returns(Reply2) {}
    }
    ''
  The service class will be transformed into a C++ class with:
  1. A function Run in order to start the server.
  2. A rpc interface and a local interface for each rpc function,
     (see the doc of RpcCodeGenerator).

  """
  def __init__(self, grammar_tree):
    """
    :param grammar_tree: a grammar tree of service.
    """
    assert isinstance(grammar_tree, lark.tree.Tree)
    assert grammar_tree.data == 'service'

    self._service_name = ScopedVariable(grammar_tree.children[0])

    self._rpcs = []
    handler = {
        'rpc': lambda statement: self._rpcs.append(RpcCodeGenerator(statement))
    }
    for statement in grammar_tree.children[1:]:
      if statement.data in handler:
        handler[statement.data](statement)

  def to_cpp_code(self):
    """
    :return: The c++ class of the service.
    """
    service_name = self._service_name.to_cpp_code()
    class_name = service_name + 'HybridBase'
    class_def = 'class %s : public %s::Service {\n' \
                ' public:\n' % (class_name, service_name)
    func_run = '  int Run(const Address& address) {\n' \
               '    grpc::ServerBuilder builder;\n' \
               '    builder.AddListeningPort(address.ToString(), ' \
               'grpc::InsecureServerCredentials());\n' \
               '    builder.RegisterService(this);\n\n' \
               '    std::unique_ptr<grpc::Server> server(' \
               'builder.BuildAndStart());\n' \
               '    server->Wait();\n\n' \
               '    return 0;\n' \
               '  }\n'
    class_end = '};\n'
    rpcs = '\n'.join([_add_indent(rpc.to_cpp_code(), 2) for rpc in self._rpcs])
    return class_def + '\n'.join([func_run, rpcs]) + class_end


class PackageCodeGenerator:
  """Code generator of a package.
  A package in proto is: "package a.b",
  while the corresponding C++ code is:
    ``
    namespace a {
    namespace b {
    ...  // all the codes in the package.
    }  // namespace a
    }  // namespace b
    ''
  """
  def __init__(self, grammar_tree=None):
    """
    :param grammar_tree: a grammar tree of package.
    """
    self._elements = []

    if grammar_tree is None:
      self._scopes = []
      return
    assert isinstance(grammar_tree, lark.tree.Tree)
    assert grammar_tree.data == 'package'
    self._scopes = ScopedVariable(grammar_tree.children[0]).name_list

  def add_to_package(self, element):
    """
    Add an element (e.g. a service) into the package.
    The code of the element will be between the beginning and the end of the
      package namespace.
    :param element: A code generator element.
    :return: None
    """
    self._elements.append(element)

  def to_cpp_code(self):
    """
    :return: The C++ code of the package including the code of the elements.
    """
    package_header = []
    package_tail = []
    for scope_name in self._scopes:
      package_header.append('namespace %s {\n' % scope_name)
      package_tail.append('}  // namespace %s\n' % scope_name)
    package_tail.reverse()

    package_body = [element.to_cpp_code() for element in self._elements]
    return '\n'.join(
        package_header + package_body + package_tail
    )


class MoreThanOnePackageError(Exception):
  """Exception when there are more than one package declaration."""
  def __init__(self):
    super(MoreThanOnePackageError, self).__init__(
        'More than one package declaration statement!'
    )


class HlrpcCodeGenerator:
  """
  The code generator to transform a proto file into a C++ header code with
  the support of hybrid-local-remote-process-call.
  """
  def __init__(self, grammar_tree, path_without_suffix):
    """
    :param grammar_tree: The full grammar tree generated by lark.
    :param path_without_suffix: path of the proto file,
      used in the including statement.
    """
    assert isinstance(grammar_tree, lark.tree.Tree)
    assert grammar_tree.data == 'start'

    self._path_without_suffix = path_without_suffix
    self._services = []
    self._package = None

    def _service_handler(grammar_tree):
      self._services.append(ServiceCodeGenerator(grammar_tree))

    def _package_handler(grammar_tree):
      if self._package is not None:
        raise MoreThanOnePackageError()
      self._package = PackageCodeGenerator(grammar_tree)

    handlers = {
        'service': _service_handler,
        'package': _package_handler
    }

    for statement in grammar_tree.children:
      assert statement.data == 'statement'
      for child in statement.children:
        if child.data in handlers:
          handlers[child.data](child)

    if self._package is None:
      self._package = PackageCodeGenerator()

    for service in self._services:
      self._package.add_to_package(service)

  def _header_define_string(self):
    """
    :return: A string used in the "#ifndef/#define/#endif" statement.
    """
    return self._path_without_suffix.upper().replace('/', '_') + '_HLRPC_PB_H_'

  def to_cpp_code(self):
    """
    :return: The C++ code of the corresponded proto. A complete file.
    """
    def_header = '#ifndef %s\n' \
                 '#define %s\n' % (self._header_define_string(),
                                   self._header_define_string())
    def_tail = '#endif  // %s' % self._header_define_string()

    include_path = '#include <memory>\n\n' \
                   '#include \"%s.grpc.pb.h\"\n' \
                   '#include \"core/util/ip.h\"\n' \
                   '#include \"core/util/status.h\"\n' \
                   % self._path_without_suffix
    return '\n'.join([def_header,
                      include_path,
                      self._package.to_cpp_code(),
                      def_tail])


def process_single_file(src_path, dst_path, parser):
  """
  Process a single source file.
  :param src_path: Source file path.
  :param dst_path: Destination file path.
  :param parser: The lark parser.
  :return: None
  """
  path_without_suffix = _get_path_without_suffix(src_path)
  if path_without_suffix is None:
    return

  with open(src_path) as fp_src, open(dst_path, 'w') as fp_dst:
    grammar_tree = parser.parse(fp_src.read())
    code_generator = HlrpcCodeGenerator(grammar_tree, path_without_suffix)
    fp_dst.write(code_generator.to_cpp_code())





def main(argv):
  """The entry of the hlrpc processor."""
  i = 1
  while i + 1 < len(argv):
    src_path = argv[i]
    dst_path = argv[i + 1]
    process_single_file(src_path, dst_path, _HLRPC_PARSER)
    i += 2


if __name__ == '__main__':
  main(sys.argv)
