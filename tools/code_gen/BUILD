package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "py_files",
    srcs = glob(["**/*.py"]),
    visibility = ["//tools:__pkg__"],
)

py_binary(
    name = "hlrpc",
    srcs = ["hlrpc.py"],
    data = ["proto.lark"],
    visibility = ["//visibility:public"],
)
