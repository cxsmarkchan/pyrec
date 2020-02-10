package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//pyrec/core:__pkg__"],
)

cc_library(
    name = "util",
    srcs = ["status.cc"],
    hdrs = ["status.h",
            "ip.h",
            "types.h"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@boost//:format",
    ],
    visibility = ["//visibility:public"],
)
