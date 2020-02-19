package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//pyrec/core:__pkg__"],
)

cc_library(
    name = "feature",
    deps = [
        ":string_serialize",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "string_serialize",
    hdrs = [
        "string_serialize.h",
    ],
    deps = [
        "//pyrec/proto:feature_cc_proto",
        "@boost//:algorithm",
        "@boost//:format",
        "@boost//:lexical_cast",
    ],
    visibility = ["//visibility:public"],
)
