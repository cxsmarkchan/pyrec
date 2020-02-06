load("@com_google_protobuf//:protobuf.bzl", "cc_proto_library", "py_proto_library")
load("//tools/code_gen:code_gen.bzl", "hlrpc_cc_proto_library")
package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

hlrpc_cc_proto_library(
    name = "recommend_cc_proto",
    srcs = ["recommend.proto"],
    deps = [":feature_cc_proto"],
    use_grpc_plugin = True,
    use_hlrpc_plugin = True,
)

py_proto_library(
    name = "recommend_py_proto",
    srcs = ["recommend.proto"],
    deps = [":feature_py_proto"],
    use_grpc_plugin = True,
)

cc_proto_library(
    name = "feature_cc_proto",
    srcs = ["feature.proto"],
)

py_proto_library(
    name = "feature_py_proto",
    srcs = ["feature.proto"],
)

hlrpc_cc_proto_library(
    name = "index_cc_proto",
    srcs = ["index.proto"],
    deps = [":feature_cc_proto"],
    use_grpc_plugin = True,
    use_hlrpc_plugin = True,
)

py_proto_library(
    name = "index_py_proto",
    srcs = ["index.proto"],
    deps = [":feature_py_proto"],
    use_grpc_plugin = True,
)

