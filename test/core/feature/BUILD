filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//test/core:__pkg__"],
)

cc_test(
    name = "feature_test",
    srcs = glob(["**/*.cc"]),
    deps = [
        "//pyrec/core/feature:string_serialize",
        "@com_google_googletest//:gtest_main",
    ],
)
