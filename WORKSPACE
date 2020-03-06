workspace(name = "pyrec")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

# cpp rules
git_repository(
    name = "rules_cc",
    commit = "0489ba308b2e1fe458dea5a3e6efebd25087a339",
    remote = "https://github.com/bazelbuild/rules_cc",
    shallow_since = "1579022185 -0800"
)

# python rules
http_archive(
    name = "rules_python",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.0.1/rules_python-0.0.1.tar.gz",
    sha256 = "aa96a691d3a8177f3215b14b0edc9641787abaaa30363a080165d06ab65e1161",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()
# Only needed if using the packaging rules.
load("@rules_python//python:pip.bzl", "pip_repositories")
pip_repositories()

# needed by protobuf
http_archive(
    name = "pypi_six",
    build_file = "//third_party/pypi:six.BUILD",
    sha256 = "d16a0141ec1a18405cd4ce8b4613101da75da0e9a7aec5bdd4fa804d0e0eba73",
    strip_prefix = "six-1.12.0",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/pypi.python.org/packages/source/s/six/six-1.12.0.tar.gz",
        "https://pypi.python.org/packages/source/s/six/six-1.12.0.tar.gz",
    ],
)

# grpc dependencies
http_archive(
    name = "com_github_grpc_grpc",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.26.0.tar.gz"
    ],
    sha256 = "2fcb7f1ab160d6fd3aaade64520be3e5446fc4c6fa7ba6581afdc4e26094bd81",
    strip_prefix = "grpc-1.26.0"
)
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()

# load dependencies in workspace
load("//tools/bazel_tools:pyrec.bzl", "pyrec_repositories", "pyrec_bind")
pyrec_repositories()
pyrec_bind()

# protobuf dependencies
git_repository(
    name = "com_google_protobuf",
    commit = "fe1790ca0df67173702f70d5646b82f48f412b99",
    remote = "https://github.com/protocolbuffers/protobuf",
    shallow_since = "1576187991 -0800"
)

# boost dependencies
git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
    remote = "https://github.com/nelhage/rules_boost",
    shallow_since = "1570056263 -0700",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

# pcre dependencies
http_archive(
    name = "pcre",
    build_file = "//third_party:pcre.BUILD",
    sha256 = "69acbc2fbdefb955d42a4c606dfde800c2885711d2979e356c0636efde9ec3b5",
    strip_prefix = "pcre-8.42",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/ftp.exim.org/pub/pcre/pcre-8.42.tar.gz",
        "https://ftp.exim.org/pub/pcre/pcre-8.42.tar.gz",
    ],
)

# swig dependencies
http_archive(
    name = "swig",
    build_file = "//third_party:swig.BUILD",
    sha256 = "58a475dbbd4a4d7075e5fe86d4e54c9edde39847cdb96a3053d87cb64a23a453",
    strip_prefix = "swig-3.0.8",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/ufpr.dl.sourceforge.net/project/swig/swig/swig-3.0.8/swig-3.0.8.tar.gz",
        "https://ufpr.dl.sourceforge.net/project/swig/swig/swig-3.0.8/swig-3.0.8.tar.gz",
        "https://pilotfiber.dl.sourceforge.net/project/swig/swig/swig-3.0.8/swig-3.0.8.tar.gz",
    ],
)

# gtest rules
http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-release-1.10.0",
    sha256 = "9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb",
    urls = [
        "https://github.com/google/googletest/archive/release-1.10.0.tar.gz",
    ],
)

# librdkafka
new_git_repository(
    name = "librdkafka",
    remote = "https://github.com/edenhill/librdkafka.git",
    commit = "4ffe54b4f59ee5ae3767f9f25dc14651a3384d62",
    build_file = "//third_party:librdkafka.BUILD",
    shallow_since = "1575312008 +0100",
)

# hiredis
new_git_repository(
    name = "hiredis",
    remote = "https://github.com/redis/hiredis",
    commit = "685030652cd98c5414ce554ff5b356dfe8437870",
    build_file = "//third_party:hiredis.BUILD",
    shallow_since = "1537916374 -0400",
)
