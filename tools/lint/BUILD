load("//tools/bazel_tools:test.bzl", "pylint_test", "cpplint_test")
package(default_visibility = ["//visibility:private"])

filegroup(
    name = "pylint_config",
    srcs = ["pylint.config"],
)

pylint_test(
    name = "pylint",
    srcs = ["//pyrec:pyrec_py_files", "//test:test_py_files"],
    config = ":pylint_config",
)

cpplint_test(
    name = "cpplint",
    srcs = ["//pyrec:pyrec_cc_files", "//test:test_cc_files"],
)
