#!/bin/bash -e

if [ "$1" == "" ]; then
    echo Usage: \"$0 directoryname\"
    exit 1
fi

# Remove trailing slash
SRC_DIRECTORY=`dirname $1`/`basename $1`
I60_DIRECTORY=${SRC_DIRECTORY}_i60
echo "Generating m60-movie from source directory $SRC_DIRECTORY"

EXTENSION=""
if [ "$DEBUG" == "1" ] ; then
    EXTENSION=DBG
fi

generateCompressTex() {
    if [ -e ${I60_DIRECTORY} ]; then
        rm -rf ${I60_DIRECTORY}
    fi

    mkdir ${I60_DIRECTORY}

    GenCompressedTex$EXTENSION --img-dir $SRC_DIRECTORY --out-dir ${I60_DIRECTORY}
}

generateMovie() {
    ENCODING=$1
    MOVIENAME=$SRC_DIRECTORY.m60

    if [ -e $MOVIENAME ]; then
        rm $MOVIENAME
    fi

    GenMovie$EXTENSION --source-dir ${I60_DIRECTORY} --outfile $MOVIENAME --encoding DRLE
}

generateCompressTex
generateMovie

exit 0;