package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//core:__pkg__"],
)

cc_library(
    name = "recommender",
    srcs = ["recommender.cc"],
    hdrs = ["recommender.h"],
    deps = [
        ":recommender_base",
        "//core/context:context_base",
        "//core/retrieval:retrieval_base",
        "@com_github_grpc_grpc//:grpc++",
        "//pyrec/proto:recommend_cc_proto",
        "//core/util:util",
        "//core/service:base",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "recommender_base",
    hdrs = ["recommender_interface.h",
            "recommender_client.h"],
    deps = ["//pyrec/proto:recommend_cc_proto"],
    visibility = ["//visibility:public"],
    )
