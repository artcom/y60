#!/bin/bash
USAGE="usage: createMovieTest.sh MOVIE_WIDTH MOVIE_HEIGHT FRAMERATE MOVIE_LENGTH_IN_SECONDS"
ARGS=4        # Number of arguments expected.
E_BADARGS=65  # Exit value if incorrect number of args passed.

test $# -ne $ARGS && echo $USAGE && exit $E_BADARGS

# Cleanup existing files
filecount=`ls testmovies | wc -l`
if [ $filecount -ge 1 ]
then
    echo "removing current images"
    rm testmovies/*
fi

y60 CreateTestMovie.js $1 $2 $3 $4