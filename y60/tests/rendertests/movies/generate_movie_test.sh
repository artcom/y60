#!/bin/bash

TESTMOVIE=testmovie

rm -rf ${TESTMOVIE}_i60
mkdir ${TESTMOVIE}_i60

EXTENSION=OPT

if [ "$DEBUG" == "1" ] ; then
    EXTENSION=DBG
fi

GenCompressedTex$EXTENSION --img-dir ${TESTMOVIE}_png --out-dir ${TESTMOVIE}_i60 --xsize 64 --ysize 16

generateMovie() {
    IMAGETYPE=$1
    ENCODING=$2
    FILENAME=${TESTMOVIE}_${IMAGETYPE}_${ENCODING}.m60

    if [ -e $FILENAME ]; then
        rm $FILENAME
    fi

    GenMovie$EXTENSION --source-dir ${TESTMOVIE}_$IMAGETYPE --outfile $FILENAME --encoding $ENCODING
}

generateMovie i60 RLE
generateMovie i60 DRLE
generateMovie png RLE
generateMovie png DRLE

exit 0;