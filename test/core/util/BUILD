filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//test/core:__pkg__"],
)

cc_test(
    name = "ip_test",
    srcs = ["ip_test.cc"],
    deps = [
        "//pyrec/core/util:util",
        "@com_google_googletest//:gtest_main",
    ],
)
