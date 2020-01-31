package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//pyrec/core:__pkg__"],
)

cc_library(
    name = "service",
    srcs = ["recommend.cc"],
    hdrs = ["recommend.h",
            "base.h"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "//pyrec/proto:recommend_cc_proto",
        "//pyrec/core/util:util",
    ],
    visibility = ["//visibility:public"],
)
