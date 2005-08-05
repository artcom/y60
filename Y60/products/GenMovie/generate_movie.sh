#!/bin/bash

if [ "$1" == "" ]; then
    echo Usage: \"$0 directoryname\"
    exit 1
fi

SRC_DIRECTORY=$1

EXTENSION=OPT
if [ "$DEBUG" == "1" ] ; then
    EXTENSION=DBG
fi

generateCompressTex() {
    if [ -e ${SRC_DIRECTORY}_i60 ]; then
        rm -rf ${SRC_DIRECTORY}_i60        
    fi

    mkdir ${SRC_DIRECTORY}_i60

    GenCompressedTex$EXTENSION --img-dir $SRC_DIRECTORY --out-dir ${SRC_DIRECTORY}_i60
}

generateMovie() {
    ENCODING=$1
    MOVIENAME=$SRC_DIRECTORY.m60

    if [ -e $MOVIENAME ]; then
        rm $MOVIENAME
    fi

    GenMovie$EXTENSION --source-dir ${SRC_DIRECTORY}_i60 --outfile $MOVIENAME --encoding $ENCODING
}

generateCompressTex
generateMovie DRLE

exit 0;