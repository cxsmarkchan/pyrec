package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//core:__pkg__"],
)

cc_library(
    name = "recommend",
    srcs = ["recommend.cc"],
    hdrs = ["recommend.h"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "//pyrec/proto:recommend_cc_proto",
        "//core/util:util",
        "//core/service:base",
    ],
    visibility = ["//visibility:public"],
)
