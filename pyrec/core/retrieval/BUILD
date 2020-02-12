package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//pyrec/core:__pkg__"],
)

cc_library(
    name = "retrieval_base",
    hdrs = [
        "retrieval_interface.h",
        "retrieval_client.h",
    ],
    deps = ["//pyrec/proto:recommend_cc_proto"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "content_based",
    srcs = ["content_based.cc"],
    hdrs = ["content_based.h"],
    deps = [
        ":retrieval_base",
        "//pyrec/core/indexer:indexer_base",
        "//pyrec/proto:recommend_cc_proto",
        "//pyrec/core/util:util",
        "@boost//:algorithm",
        "@boost//:lexical_cast",
    ],
    visibility = ["//visibility:public"],
)
