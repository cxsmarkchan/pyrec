filegroup(
    name = "cc_files",
    srcs = glob(["**/*.cc", "**/*.h"]),
    visibility = ["//test/core:__pkg__"],
)

cc_test(
    name = "hash_indexer_test",
    srcs = ["hash_indexer_test.cc"],
    deps = [
        "//pyrec/core/indexer:hash_indexer",
        "@com_google_googletest//:gtest_main",
    ],
)
