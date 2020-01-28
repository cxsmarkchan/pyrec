load("@com_google_protobuf//:protobuf.bzl", "cc_proto_library", "py_proto_library")
package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

cc_proto_library(
    name = "service_cc_proto",
    srcs = ["service.proto"],
    use_grpc_plugin = True,
)

py_proto_library(
    name = "service_py_proto",
    srcs = ["service.proto"],
    use_grpc_plugin = True,
)

