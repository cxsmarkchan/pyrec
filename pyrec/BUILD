load("//tools/bazel_tools:swig.bzl", "py_wrap_cc")
package(
    licenses = ["notice"],  # Apache 2.0
)

py_wrap_cc(
    name = "pywrap_core",
    srcs = ["pyrec.i"],
    swig_includes = [
        "indexer/indexer.i",
        "retrieval/retrieval.i",
        "context/context.i",
        "//tools/swig:swig_tools",
    ],
    deps = ["//pyrec/core/recommend:recommend",
            "//pyrec/core/indexer:indexer",
            "//pyrec/core/retrieval:retrieval",
            "//pyrec/core/context:context",
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
    srcs = glob(["**/*.py"]),
    deps = [
        "//pyrec/proto:pyrec_py_proto",
        "//pyrec:pywrap_core",
    ],
    data = [
        "//pyrec:pywrap_core_shared_library",
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
