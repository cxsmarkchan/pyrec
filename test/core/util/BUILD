filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//test/core:__pkg__"],
)

cc_test(
    name = "util_test",
    srcs = glob(["**/*.cc"]),
    deps = [
        "//pyrec/core/util:util",
        "@com_google_googletest//:gtest_main",
    ],
)
