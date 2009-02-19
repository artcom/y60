#!/bin/bash

if [ -z "$MAYA_SDK" ]; then
    echo "MAYA_SDK not set"
    exit 1
fi

if [ -z "$MAYA_VERSION" ]; then
    echo "MAYA_VERSION not set"
    exit 1
fi

SOURCE=$1
BINARY=$2

usage() {
    echo "Usage: install.sh SOURCE_DIR BINARY_DIR"
    exit 1
}

if [ -z "$SOURCE" ]; then
    usage
fi

if [ -z "$BINARY" ]; then
    usage
fi

case `uname` in
Linux*)
    DESTINATION="${HOME}/maya/${MAYA_VERSION}"

    mkdir -p "${DESTINATION}/plug-ins"

    cp -vf "${BINARY}/libmaya-xport.so" "${DESTINATION}/plug-ins/mayaY60-export.so"

    mkdir -p "${DESTINATION}/scripts"

    # XXX quoting
    cp -vf ${SOURCE}/*.mel "${DESTINATION}/scripts"

    ;;
esac
