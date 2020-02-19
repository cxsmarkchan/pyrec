package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//pyrec/core:__pkg__"],
)

cc_library(
    name = "context",
    deps = [":context_base", ":kv_context"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "context_base",
    hdrs = [
        "context_interface.h",
        "context_client.h",
    ],
    deps = ["//pyrec/proto:recommend_cc_proto"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "kv_context",
    srcs = ["kv_context.cc"],
    hdrs = ["kv_context.h"],
    deps = [
        ":context_base",
        "//pyrec/core/feature:string_serialize",
        "//pyrec/proto:recommend_cc_proto",
        "//pyrec/core/util:util",
        "//pyrec/core/indexer:hash_indexer",
        "@boost//:algorithm",
        "@boost//:lexical_cast",
    ],
    visibility = ["//visibility:public"],
)
