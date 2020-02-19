filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//test/core:__pkg__"],
)

cc_test(
    name = "content_based_test",
    srcs = ["content_based_test.cc"],
    deps = [
        "//core/retrieval:content_based",
        "//core/indexer:hash_indexer",
        "@com_google_googletest//:gtest_main",
    ],
)
