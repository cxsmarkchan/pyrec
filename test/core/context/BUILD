filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//test/core:__pkg__"],
)

cc_test(
    name = "kv_context_test",
    srcs = ["kv_context_test.cc"],
    deps = [
        "//pyrec/core/context:kv_context",
        "@com_google_googletest//:gtest_main",
    ],
)
