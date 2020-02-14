package(
    licenses = ["notice"],  # BSD-3-Clause
)

filegroup(
    name = "Makefile_file",
    srcs = ["Makefile"],
)

genrule(
    name = "build",
    srcs = glob(["**/*.c", "**/*.cpp", "**/*.h", "**/*.*"]) + [":Makefile_file"],
    outs = [
       "hiredis/libhiredis.a",
       "hiredis/alloc.h",
       "hiredis/async.h",
       "hiredis/hiredis.h",
       "hiredis/read.h",
       "hiredis/sds.h",
       "hiredis/adapters/ae.h",
       "hiredis/adapters/ivykis.h",
       "hiredis/adapters/libevent.h",
       "hiredis/adapters/macosx.h",
       "hiredis/adapters/glib.h",
       "hiredis/adapters/libev.h",
       "hiredis/adapters/libuv.h",
       "hiredis/adapters/qt.h",
    ],
    cmd = ' '.join([
        "ROOT=$$(dirname $(rootpath Makefile_file)) &&",
        "pushd $$ROOT &&",
        "make &&",
        "popd &&",
        "cp $$ROOT/libhiredis.a $(execpath hiredis/libhiredis.a) &&",
        "cp $$ROOT/libhiredis.a $(execpath hiredis/libhiredis.a) &&",
        "cp $$ROOT/alloc.h $(execpath hiredis/alloc.h) &&",
        "cp $$ROOT/async.h $(execpath hiredis/async.h) &&",
        "cp $$ROOT/hiredis.h $(execpath hiredis/hiredis.h) &&",
        "cp $$ROOT/read.h $(execpath hiredis/read.h) &&",
        "cp $$ROOT/sds.h $(execpath hiredis/sds.h) &&",
        "cp $$ROOT/adapters/ae.h $(execpath hiredis/adapters/ae.h) &&",
        "cp $$ROOT/adapters/ivykis.h $(execpath hiredis/adapters/ivykis.h) &&",
        "cp $$ROOT/adapters/libevent.h $(execpath hiredis/adapters/libevent.h) &&",
        "cp $$ROOT/adapters/macosx.h $(execpath hiredis/adapters/macosx.h) &&",
        "cp $$ROOT/adapters/glib.h $(execpath hiredis/adapters/glib.h) &&",
        "cp $$ROOT/adapters/libev.h $(execpath hiredis/adapters/libev.h) &&",
        "cp $$ROOT/adapters/libuv.h $(execpath hiredis/adapters/libuv.h) &&",
        "cp $$ROOT/adapters/qt.h $(execpath hiredis/adapters/qt.h)",
    ]),
)

cc_library(
    name = "hiredis",
    srcs = ["hiredis/libhiredis.a"],
    hdrs = glob(["hiredis/**/*.h"]),
    visibility = ["//visibility:public"],
)
