package(default_visibility = ["//visibility:private"])

COMMON_DEPS = [
    "//pyrec:pyrec",
]

sh_binary(
    name = "build_python_package",
    srcs = ["build_python_package.sh"],
    data = COMMON_DEPS,
)
