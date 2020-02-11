# Rules of wrap cc into python interfaces.
# Obtained and revised from [https://github.com/tensorflow/tensorflow]

load(
    "@rules_cc//cc:defs.bzl",
    "cc_binary",
    "cc_library",
    "cc_test",
)

SHARED_LIBRARY_NAME_PATTERNS = [
    "lib%s.so%s",  # On Linux, shared libraries are usually named as libfoo.so
    "lib%s%s.dylib",  # On macos, shared libraries are usually named as libfoo.dylib
    "%s%s.dll",  # On Windows, shared libraries are usually named as foo.dll
]

def _get_repository_roots(ctx, files):
    """Returns abnormal root directories under which files reside.
      When running a ctx.action, source files within the main repository are all
      relative to the current directory; however, files that are generated or exist
      in remote repositories will have their root directory be a subdirectory,
      e.g. bazel-out/local-fastbuild/genfiles/external/jpeg_archive. This function
      returns the set of these devious directories, ranked and sorted by popularity
      in order to hopefully minimize the number of I/O system calls within the
      compiler, because includes have quadratic complexity.
      """
    result = {}
    for f in files.to_list():
        root = f.root.path
        if root:
            if root not in result:
                result[root] = 0
            result[root] -= 1
        work = f.owner.workspace_root
        if work:
            if root:
                root += "/"
            root += work
        if root:
            if root not in result:
                result[root] = 0
            result[root] -= 1
    return [k for v, k in sorted([(v, k) for k, v in result.items()])]

def _get_transitive_headers(hdrs, deps):
    """Obtain the header files for a target and its transitive dependencies.
      Args:
        hdrs: a list of header files
        deps: a list of targets that are direct dependencies
      Returns:
        a collection of the transitive headers
      """
    return depset(
        hdrs,
        transitive = [dep[CcInfo].compilation_context.headers for dep in deps],
    )

# Bazel rules for building swig files.
def _py_wrap_cc_impl(ctx):
    srcs = ctx.files.srcs
    if len(srcs) != 1:
        fail("Exactly one SWIG source file label must be specified.", "srcs")
    module_name = ctx.attr.module_name
    src = ctx.files.srcs[0]
    inputs = _get_transitive_headers([src] + ctx.files.swig_includes, ctx.attr.deps)
    inputs = depset(ctx.files._swiglib, transitive = [inputs])
    inputs = depset(ctx.files.toolchain_deps, transitive = [inputs])
    swig_include_dirs = depset(_get_repository_roots(ctx, inputs))
    swig_include_dirs = depset(sorted([f.dirname for f in ctx.files._swiglib]), transitive = [swig_include_dirs])
    args = [
        "-c++",
        "-python",
        "-threads",
        "-module",
        module_name,
        "-o",
        ctx.outputs.cc_out.path,
        "-outdir",
        ctx.outputs.py_out.dirname,
    ]
    args += ["-l" + f.path for f in ctx.files.swig_includes]
    args += ["-I" + i for i in swig_include_dirs.to_list()]
    args += [src.path]
    outputs = [ctx.outputs.cc_out, ctx.outputs.py_out]
    ctx.actions.run(
        executable = ctx.executable._swig,
        arguments = args,
        inputs = inputs.to_list(),
        outputs = outputs,
        mnemonic = "PythonSwig",
        progress_message = "SWIGing " + src.path,
    )
    return struct(files = depset(outputs))

_py_wrap_cc = rule(
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "swig_includes": attr.label_list(
            allow_files = True,
        ),
        "deps": attr.label_list(
            allow_files = True,
            providers = [CcInfo],
        ),
        "toolchain_deps": attr.label_list(
            allow_files = True,
        ),
        "module_name": attr.string(mandatory = True),
        "py_module_name": attr.string(mandatory = True),
        "_swig": attr.label(
            default = Label("@swig//:swig"),
            executable = True,
            cfg = "host",
        ),
        "_swiglib": attr.label(
            default = Label("@swig//:templates"),
            allow_files = True,
        ),
    },
    outputs = {
        "cc_out": "%{module_name}.cc",
        "py_out": "%{py_module_name}.py",
    },
    implementation = _py_wrap_cc_impl,
)

def py_wrap_cc(
        name,
        srcs,
        swig_includes = [],
        deps = [],
        copts = [],
        version_script = None,
        **kwargs):
    """Builds a Python extension module."""
    module_name = name.split("/")[-1]

    # Convert a rule name such as foo/bar/baz to foo/bar/_baz.so
    # and use that as the name for the rule producing the .so file.
    cc_library_base = "/".join(name.split("/")[:-1] + ["_" + module_name])

    cc_library_name = cc_library_base + ".so"
    cc_library_pyd_name = "/".join(
        name.split("/")[:-1] + ["_" + module_name + ".pyd"],
    )
    extra_deps = []
    _py_wrap_cc(
        name = name + "_py_wrap",
        srcs = srcs,
        module_name = module_name,
        py_module_name = name,
        swig_includes = swig_includes,
        toolchain_deps = ["@bazel_tools//tools/cpp:current_cc_toolchain"],
        deps = deps + extra_deps,
    )
    native.cc_binary(
        name = cc_library_name,
        srcs = [module_name + ".cc"],
        linkstatic = 1,
        linkshared = 1,
        deps = deps,
        **kwargs
    )

    native.genrule(
        name = "gen_" + cc_library_pyd_name,
        srcs = [":" + cc_library_name],
        outs = [cc_library_pyd_name],
        cmd = "cp $< $@",
    )
    native.py_library(
        name = name,
        srcs = [":" + name + ".py"],
        srcs_version = "PY2AND3",
        **kwargs
    )
