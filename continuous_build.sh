#!/bin/bash
set -e # abort on error
set -x # print all commands
#set -u

BUILD_DIR=_build
UNAME_OS=`uname -s`

case "$UNAME_OS" in
    Cygwin* | cygwin* | CYGWIN* | MINGW*)
		CMAKE_GENERATOR="Visual Studio 9 2008"
		MAKE_COMMAND='vcbuild PRO60.sln Release|Win32'
        ;;
    Darwin*)
		CMAKE_GENERATOR="Unix Makefiles"
		MAKE_COMMAND=make
        ;;
    Linux*)
		CMAKE_GENERATOR="Unix Makefiles"
		MAKE_COMMAND=make
        ;;
	*)
		echo What?
		;;
esac

echo "=== Running on ${UNAME_OS}"
echo "=== Configuring cmake build"

if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
    MKDIR_RESULT=$?
    if [ $MKDIR_RESULT -gt 0 ]; then
        echo Cannot create $PWD/$BUILD_DIR
        exit 3
    fi
fi
cd $BUILD_DIR

cmake -G "$CMAKE_GENERATOR" ..

$MAKE_COMMAND

echo "Build done"

exit 0
