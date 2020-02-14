package(
    licenses = ["notice"],  # Copyright (c) 2012-2018, Magnus Edenhill
)

filegroup(
    name = "configure_file",
    srcs = ["configure"],
)

genrule(
    name = "build",
    srcs = glob(["**/*.c", "**/*.cpp", "**/*.h", "**/*.*", "**/Makefile"]) + [":configure_file"],
    outs = [
       "librdkafka/librdkafka++.a",
       "librdkafka/librdkafka.a",
       "librdkafka/rdkafka.h",
       "librdkafka/rdkafka_mock.h",
       "librdkafka/rdkafkacpp.h"],
    cmd = ' '.join([
        "ROOT=$$(dirname $(rootpath configure_file)) &&",
        "pushd $$ROOT &&",
        "./configure && make &&",
        "popd &&",
        "cp $$ROOT/src/librdkafka.a $(execpath librdkafka/librdkafka.a) &&",
        "cp $$ROOT/src-cpp/librdkafka++.a $(execpath librdkafka/librdkafka++.a) &&",
        "cp $$ROOT/src/rdkafka.h $(execpath librdkafka/rdkafka.h) &&",
        "cp $$ROOT/src/rdkafka_mock.h $(execpath librdkafka/rdkafka_mock.h) &&",
        "cp $$ROOT/src-cpp/rdkafkacpp.h $(execpath librdkafka/rdkafkacpp.h)",
    ])
)

cc_library(
    name = "librdkafka",
    srcs = ["librdkafka/librdkafka++.a", "librdkafka/librdkafka.a"],
    hdrs = ["librdkafka/rdkafka.h", "librdkafka/rdkafka_mock.h", "librdkafka/rdkafkacpp.h"],
    visibility = ["//visibility:public"],
)
