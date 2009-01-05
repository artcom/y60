#!/bin/bash
USAGE="usage: createTestMovies.sh MOVIE_WIDTH MOVIE_HEIGHT FRAMERATE\n"
USAGE=${USAGE}"    Movie length is exactly one frame longer than the number of seconds given."
ARGS=3        # Number of arguments expected.
E_BADARGS=65  # Exit value if incorrect number of args passed.

MAX_MOVIE_LENGTH=60 # Length of longest movie in seconds

test $# -ne $ARGS && echo -e $USAGE && exit $E_BADARGS

MOVIE_WIDTH=$1
MOVIE_HEIGHT=$2
FRAMERATE=$3

function calculateSingleFrameSeconds {
    FRAMERATE=$1
    # calculate length of a single frame
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
    y60 CreateTestMovie.js ${MOVIE_WIDTH} ${MOVIE_HEIGHT} ${FRAMERATE} ${MAX_MOVIE_LENGTH}
}

function createMovie {
    # Note that MOVIE_LENGTH_IN_SECONDS must be an integer!
    MOVIE_LENGTH_IN_SECONDS=$1
    USE_AUDIO=$2
    ENCODER_NAME=$3
    ENCODER_PARAMS=$4
    ENCODER_EXTENSION=$5
    NUM_PASSES=$6
 
    MOVIE_FILENAME="testmovies/${ENCODER_NAME}_${MOVIE_WIDTH}x${MOVIE_HEIGHT}_${FRAMERATE}_${MOVIE_LENGTH_IN_SECONDS}"
    if [ "$USE_AUDIO" = "true" ]
    then
        if [ ${MOVIE_LENGTH_IN_SECONDS} -ge 2 ]
        then  
            sox testmovies/material/TenSecondClick.wav testmovies/tmp/tmp.wav repeat ${MOVIE_LENGTH_IN_SECONDS}
        else
            cp testmovies/material/OneSecondClick.wav testmovies/tmp/tmp.wav
        fi
        AUDIO_PARAMS="-i testmovies/tmp/tmp.wav"
        MOVIE_FILENAME=${MOVIE_FILENAME}_audio
    else
        AUDIO_PARAMS=""
        MOVIE_FILENAME=${MOVIE_FILENAME}_noaudio
    fi
    MOVIE_FILENAME=${MOVIE_FILENAME}.${ENCODER_EXTENSION}
    calculateSingleFrameSeconds ${FRAMERATE}
    echo "-------------------------------------------------------------------------"
    echo "   Generating ${MOVIE_FILENAME}"
    echo "-------------------------------------------------------------------------"
    FFMPEG_PARAMS="${AUDIO_PARAMS} -t ${MOVIE_LENGTH_IN_SECONDS}${SINGLE_FRAME} -i testmovies/tmp/${MOVIE_WIDTH}x${MOVIE_HEIGHT}_frame%07d.png -y -r ${FRAMERATE} -b 9000 ${ENCODER_PARAMS} ${MOVIE_FILENAME}"
    echo "ffmpeg ${FFMPEG_PARAMS}"
    if [ ${NUM_PASSES} -eq 1 ]
    then
        echo "Single pass encoding"
        ffmpeg ${FFMPEG_PARAMS}
    else
        echo "Two pass encoding"
        ffmpeg -pass 1 ${FFMPEG_PARAMS}
        ffmpeg -pass 2 ${FFMPEG_PARAMS}
    fi
}

function createMovies {
    ENCODER_NAME=$1
    ENCODER_PARAMS=$2
    ENCODER_EXTENSION=$3
    NUM_PASSES=$4
   
    createMovie 1 false ${ENCODER_NAME} "${ENCODER_PARAMS}" ${ENCODER_EXTENSION} ${NUM_PASSES}
    createMovie 10 false ${ENCODER_NAME} "${ENCODER_PARAMS}" ${ENCODER_EXTENSION} ${NUM_PASSES}
    createMovie ${MAX_MOVIE_LENGTH} false ${ENCODER_NAME} "${ENCODER_PARAMS}" ${ENCODER_EXTENSION} ${NUM_PASSES}
    
    createMovie 1 true ${ENCODER_NAME} "${ENCODER_PARAMS}" ${ENCODER_EXTENSION} ${NUM_PASSES}
    createMovie 10 true ${ENCODER_NAME} "${ENCODER_PARAMS}" ${ENCODER_EXTENSION} ${NUM_PASSES}
    createMovie ${MAX_MOVIE_LENGTH} true ${ENCODER_NAME} "${ENCODER_PARAMS}" ${ENCODER_EXTENSION} ${NUM_PASSES}
}

createImages 

rm testmovies/*.*


createMovies mpeg4 "-vcodec mpeg4" avi 2
#createMovies msmpeg4v1 "-vcodec msmpeg4v1" avi                # broken
createMovies msmpeg4v2 "-vcodec msmpeg4v2" avi 2
createMovies msmpeg4 "-vcodec msmpeg4" avi 2
createMovies xvid "-vcodec xvid -pix_fmt yuv420p" avi 2
createMovies mjpeg "-vcodec mjpeg" avi 2
createMovies huffyuv "-vcodec huffyuv -pix_fmt yuv422p" avi 1 

createMovies mpeg1 "-vcodec mpeg1video" mpg 2
createMovies mpeg2intra "-intra -vcodec mpeg2video" mpg 2
createMovies mpeg2 "-vcodec mpeg2video" mpg 2
createMovies mpeg4 "-vcodec mpeg4" mp4 2
#createMovies h264 "-vcodec h264" mp4 2                        # broken

createMovies mpeg4 "-vcodec mpeg4" mov 2
createMovies xvid "-vcodec xvid -pix_fmt yuv420p" mov 2
#createMovies h264 "-vcodec h264" mov 2                        # broken
#createMovies huffyuv "-vcodec huffyuv -pix_fmt yuv422p" mov 1 # broken

createMovies realvideo "-vcodec rv20" rm 2  

# Clean up temporary files
filecount=`ls testmovies/tmp | wc -l`
if [ $filecount -ge 1 ]
then
    echo "removing current images"
    find testmovies/tmp -name "*" -maxdepth 1 -exec rm {} \;
fi
echo done

