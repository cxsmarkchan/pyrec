load("//third_party/py:python_configure.bzl", "python_configure")

def pyrec_repositories():
    python_configure(name = "local_config_python")

def pyrec_bind():
    # Needed by Protobuf
    native.bind(
        name = "six",
        actual = "@pypi_six//:six"
    )
    native.bind(
        name = "grpc_lib",
        actual = "@com_github_grpc_grpc//:grpc++"
    )
    native.bind(
        name = "grpc_python_plugin",
        actual = "@com_github_grpc_grpc//src/compiler:grpc_python_plugin"
    )
    native.bind(
        name = "grpc_cpp_plugin",
        actual = "@com_github_grpc_grpc//src/compiler:grpc_cpp_plugin"
    )
