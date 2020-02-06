load("//tools/bazel_tools:test.bzl", "py_bulk_test")
package(
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "test_cc_files",
    srcs = ["//test/core:cc_files"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "test_py_files",
    srcs = glob(["**/*.py"]),
    visibility = ["//visibility:public"],
)

py_bulk_test(
    name = "py_test",
    srcs = glob(["**/*_test.py"]),
    deps = ["//pyrec:pyrec",
            ":test_py_files",
            "//tools:tools_py_files",
            "//tools/code_gen:hlrpc"]
            + glob(["**/resources/*"]),
)
