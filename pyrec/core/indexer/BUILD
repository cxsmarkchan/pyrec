package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//pyrec/core:__pkg__"],
)

cc_library(
    name = "hash_indexer",
    srcs = ["hash_indexer.cc"],
    hdrs = ["hash_indexer.h"],
    deps = [
        "//pyrec/proto:indexer_cc_proto",
        "//pyrec/core/util:util",
        "@boost//:algorithm",
        "@boost//:lexical_cast",
    ],
    visibility = ["//visibility:public"],
)
