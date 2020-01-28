load("//tools/bazel_tools:swig.bzl", "py_wrap_cc")
package(
    licenses = ["notice"],  # Apache 2.0
)

py_wrap_cc(
    name = "pywrap_core",
    srcs = ["pyrec.i"],
    deps = ["//pyrec/core/service:service",
            "@local_config_python//:python_headers"],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "pywrap_core_shared_library",
    srcs = [":_pywrap_core.so"],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "pyrec_cc_files",
    srcs = ["//pyrec/core:cc_files"],
    visibility = ["//visibility:public"]
)

filegroup(
    name = "pyrec_py_files",
    srcs = glob(["**/*.py"]),
    visibility = ["//visibility:public"]
)

py_library(
    name = "service",
    srcs = glob(["service/*.py"]),
    deps = [
        "//pyrec/proto:service_py_proto",
        "//pyrec:pywrap_core",
    ],
)

py_library(
    name = "pyrec",
    srcs = ["__init__.py"],
    deps = [
        ":service"
    ],
    visibility = ["//visibility:public"]
)
