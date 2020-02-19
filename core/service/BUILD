package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//core:__pkg__"],
)

cc_library(
    name = "base",
    hdrs = ["base.h"],
    deps = [
        "//core/util:util",
    ],
    visibility = ["//visibility:public"],
)
