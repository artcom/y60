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
CYGWIN*)

# install in $MAYA_SDK-DIR
#   mkdir -p $MAYA_SDK/plug-ins
#   mkdir -p $MAYA_SDK/scripts/others
#   # XXX is it ok that maya is located in lib while the others are in bin?
#   cp -vf ${BINARY}/lib/Release/maya-xport.dll "$MAYA_SDK/bin/plug-ins/mayaY60export.mll"
#   cp -vf ${BINARY}/bin/Release/aslbase.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/asldom.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/aslmath.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/aslzip.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/y60base.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/y60image.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/y60scene.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/y60video.dll "$MAYA_SDK/bin"
#   cp -vf ${BINARY}/bin/Release/paintlib.dll "$MAYA_SDK/bin"
#   cp -vf ${SOURCE}/*.mel "$MAYA_SDK/scripts/others"

    # ToDo: not tested yet (due to broken tree)
    # install in $home dir (does only work if the dll's are in the path)
    DESTINATION=$USERPROFILE/"Eigene Dateien"/maya/${MAYA_VERSION}
    mkdir -p "${DESTINATION}/plug-ins"
    cp -vf ${BINARY}/lib/Release/maya-xport.dll "${DESTINATION}/plug-ins/mayaY60export.mll"
    mkdir -p "${DESTINATION}/scripts"
    cp -vf ${SOURCE}/*.mel "${DESTINATION}/scripts"
    ;;
esac



