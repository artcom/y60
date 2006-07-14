#!/bin/bash
USAGE="usage: createTestMovies.sh MOVIE_WIDTH MOVIE_HEIGHT FRAMERATE\n"
USAGE=${USAGE}"    Movie length is exactly one frame longer than the number of seconds given."
ARGS=3        # Number of arguments expected.
E_BADARGS=65  # Exit value if incorrect number of args passed.

test $# -ne $ARGS && echo -e $USAGE && exit $E_BADARGS

MOVIE_WIDTH=$1
MOVIE_HEIGHT=$2
FRAMERATE=$3

function calculateSingleFrameSeconds {
    FRAMERATE=$1
    # calculate seconds for a single frame
    if [ ${FRAMERATE} -lt 10 ]
    then 
        ZERO_STRG=""
    else
        ZERO_STRG="0"
    fi
    SINGLE_FRAME=".${ZERO_STRG}$((10000/${FRAMERATE}))"
    #echo "result is ${SINGLE_FRAME}"
}

function createImages {
    y60 CreateTestMovie.js $MOVIE_WIDTH $MOVIE_HEIGHT $FRAMERATE 60
}

function createMovie {
    # Note that MOVIE_LENGTH_IN_SECONDS must be an integer!
    MOVIE_LENGTH_IN_SECONDS=$1
    USE_AUDIO=$2
    USE_ONLY_IFRAMES=$3
 
    MOVIE_FILENAME="testmovies/${MOVIE_WIDTH}x${MOVIE_HEIGHT}_${FRAMERATE}_${MOVIE_LENGTH_IN_SECONDS}"
    if [ "$USE_AUDIO" = "true" ]
    then
        if [ ${MOVIE_LENGTH_IN_SECONDS} -ge 2 ]
        then  
            sox testmovies/OneSecondClick.wav testmovies/tmp/tmp.wav repeat ${MOVIE_LENGTH_IN_SECONDS}
        else
            cp testmovies/OneSecondClick.wav testmovies/tmp/tmp.wav
        fi
        AUDIO_PARAMS="-i testmovies/tmp/tmp.wav"
        MOVIE_FILENAME=${MOVIE_FILENAME}_audio
    else
        AUDIO_PARAMS=""
        MOVIE_FILENAME=${MOVIE_FILENAME}_noaudio
    fi
    if [ "$USE_ONLY_IFRAMES" = "true" ]
    then
        echo using iframes only
        IFRAME_PARAMS="-intra"
        MOVIE_FILENAME=${MOVIE_FILENAME}_iframes
    else
        IFRAME_PARAMS=""
    fi
    MOVIE_FILENAME=${MOVIE_FILENAME}.mpg
    calculateSingleFrameSeconds ${FRAMERATE}
    echo "--- Generating ${MOVIE_FILENAME}"
    FFMPEG_PARAMS="${AUDIO_PARAMS} -t ${MOVIE_LENGTH_IN_SECONDS}${SINGLE_FRAME} -b 500 -i testmovies/tmp/${MOVIE_WIDTH}x${MOVIE_HEIGHT}_frame%07d.png ${IFRAME_PARAMS} -y -r ${FRAMERATE} -vcodec mpeg2video ${MOVIE_FILENAME}"
    echo "ffmpeg ${FFMPEG_PARAMS}"
    ffmpeg ${FFMPEG_PARAMS}
}

createImages 

# syntax: createMovie LENGTH USEAUDIO IFRAMESONLY
createMovie 1 true true
#createMovie 10 true true
#createMovie 60 true true
#createMovie 1 true false
#createMovie 10 true false
#createMovie 60 true false

#createMovie 1 false true
#createMovie 10 false true
#createMovie 60 false true
#createMovie 1 false false
#createMovie 10 false false
#createMovie 60 false false


# Cleanup temporary files
filecount=`ls testmovies/tmp | wc -l`
if [ $filecount -ge 1 ]
then
    echo "removing current images"
    rm testmovies/tmp/*
fi
echo done

