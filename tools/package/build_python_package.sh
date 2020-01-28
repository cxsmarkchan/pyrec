#!/usr/bin/env bash
RUNFILES="bazel-bin/tools/package/build_python_package.runfiles/pyrec/pyrec"

function usage() {
  echo "usage: $0 --dst dest_dir"
  exit 1
}

DEST_DIR=""
while :; do
  case $1 in
    --dst)
      DEST_DIR=$2
      shift 2
      ;;
    *)
      break
  esac
done

if [ -z $DEST_DIR ]; then
  usage
fi

cp -r $RUNFILES $DEST_DIR
