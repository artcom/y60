#!/bin/bash
set -e
set -x
set -u

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


echo configuring cmake build

cd ..
if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
    MKDIR_RESULT=$?
    if [ $MKDIR_RESULT -gt 0 ]; then
        echo Cannot create $PWD/$BUILD_DIR
        exit 3
    fi
fi
cd $BUILD_DIR

#cmake -G "$CMAKE_GENERATOR" ../src
CMAKE_RESULT=$?
if [ $CMAKE_RESULT -gt 0 ]; then
    echo cmake failed
    exit 2
fi

$MAKE_COMMAND
MAKE_RESULT=$?
if [ $MAKE_RESULT -gt 0 ]; then
    echo make failed
    exit 1
fi

echo done

exit 0
