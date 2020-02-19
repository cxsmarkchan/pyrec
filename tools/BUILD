package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "tools_cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "tools_py_files",
    srcs = glob(["**/*.py"]) + [
        "//tools/code_gen:py_files",
    ],
    visibility = ["//visibility:public"],
)
