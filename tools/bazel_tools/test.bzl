def _empty_script_impl(ctx):
    """Creates an empty bash script."""
    ctx.actions.write(
        output = ctx.outputs.script,
        content = "#!/bin/bash\necho %s\n" % ctx.workspace_name
    )

empty_script = rule(
    implementation = _empty_script_impl,
    outputs = {
        "script": "%{name}.sh"
    }
)

def _py_unittest_script_impl(ctx):
    """Creates a bash script for python unittest."""
    workspace_name = ctx.workspace_name

    ctx.actions.write(
        output = ctx.outputs.script,
        content = "#!/bin/bash\ncd $1.runfiles/%s\npython -m unittest $2" % workspace_name
    )

py_unittest_script = rule(
    implementation = _py_unittest_script_impl,
    outputs = {
        "script": "%{name}.sh"
    }
)

def cmd_test(name, cmd, srcs=[], deps=[]):
    """Runs test via a shell command."""
    cmd_out = " ".join([
        cmd,
        "&& echo success > $@"
    ])

    empty_script(
        name = name + "_script"
    )

    native.genrule(
        name = name + "_srcs",
        srcs = srcs,
        outs = [ name + ".log" ],
        tools = deps,
        cmd = cmd_out,
    )

    native.sh_test(
        name = name,
        srcs = [ name + "_script" ],
        data = [ name + ".log" ],
    )

def py_bulk_test(name, deps=[], srcs=[]):
    """Runs a bulk of python unittests."""
    env_name = name + "_env"
    py_unittest_script(
        name = env_name,
    )
    native.sh_binary(
        name = env_name + "_bin",
        srcs = [env_name],
        data = deps,
    )

    cmd = "bash $(location %s) $(location %s) \"$(SRCS)\"" % (env_name + "_bin", env_name + "_bin")

    cmd_test(
        name = name,
        cmd = cmd,
        srcs = srcs,
        deps = deps + [env_name + "_bin"],
    )

def pylint_test(name, deps=[], srcs=[], config=""):
    """Pylint check, requires pylint installed."""
    config_term = ""
    if len(config) > 0:
        config_term = "--rcfile=$(location %s)" % config

    cmd = " ".join([
        "pylint $(SRCS)",
        config_term,
    ])

    cmd_test(
        name = name,
        cmd = cmd,
        srcs = srcs,
        deps = deps + [config],
    )

def cpplint_test(name, deps=[], srcs=[]):
    """Cpplint check, requires cpplint installed."""
    cmd = "cpplint --root $$(pwd) $(SRCS)"

    cmd_test(
        name = name,
        cmd = cmd,
        srcs = srcs,
        deps = deps,
    )
