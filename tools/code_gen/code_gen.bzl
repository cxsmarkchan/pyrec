# Rules of code generators

load(
    "@com_google_protobuf//:protobuf.bzl",
    "cc_proto_library",
)

def _hlrpc_cc_proto_library_impl(ctx):
    inputs = ctx.files.srcs
    outputs = ctx.outputs.outputs
    if len(inputs) != len(outputs):
        fail("The number of inputs must be the same as that of outputs", "outputs")
    args = []
    for i in range(len(inputs)):
        args.append(inputs[i].path)
        args.append(outputs[i].path)
    ctx.actions.run(
        executable = ctx.executable._hlrpc,
        arguments = args,
        inputs = inputs,
        outputs = outputs,
        progress_message = "HLRPC translating " + ctx.attr.name,
        use_default_shell_env=True,
    )
    return struct(files = depset(outputs))

_hlrpc_cc_proto_library = rule(
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "_hlrpc": attr.label(
             default = Label("//tools/code_gen:hlrpc"),
             executable = True,
             cfg = "host",
         ),
        "outputs": attr.output_list(mandatory = True),
    },
    implementation = _hlrpc_cc_proto_library_impl,
)

def hlrpc_cc_proto_library(
        name,
        srcs,
        use_hlrpc_plugin=False,
        use_grpc_plugin=False,
        **kwargs):
    """Compiles a proto file with hybrid-local-remote-process-call supoort."""
    if use_hlrpc_plugin and not use_grpc_plugin:
        fail("Cannot set use_hlrpc_plugin=True with use_grpc_plugin=False!")

    if not use_hlrpc_plugin:
        cc_proto_library(
             name = name,
             srcs = srcs,
             use_grpc_plugin = use_grpc_plugin,
             **kwargs
        )
        return

    cc_proto_library(
        name = name + "_proto",
        srcs = srcs,
        use_grpc_plugin = use_grpc_plugin,
        **kwargs
    )

    output_names = [src.split("/")[-1][:-6] + ".hlrpc.pb.h" for src in srcs]
    _hlrpc_cc_proto_library(
        name = name + "_hlrpc",
        srcs = srcs,
        outputs = output_names
    )

    native.cc_library(
        name = name,
        hdrs = output_names,
        deps = [
            ":" + name + "_proto",
        ]
    )
