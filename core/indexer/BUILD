package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//core:__pkg__"],
)

cc_library(
    name = "indexer",
    deps = [":indexer_base", ":hash_indexer"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "indexer_base",
    hdrs = [
        "indexer_interface.h",
        "indexer_client.h",
    ],
    deps = ["//pyrec/proto:indexer_cc_proto"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "hash_indexer",
    srcs = ["hash_indexer.cc"],
    hdrs = ["hash_indexer.h"],
    deps = [
        ":indexer_base",
        "//pyrec/proto:indexer_cc_proto",
        "//core/util:util",
        "@boost//:algorithm",
        "@boost//:lexical_cast",
    ],
    visibility = ["//visibility:public"],
)
