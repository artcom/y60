#! /bin/bash
MODELNAME="$1"
if [ "$2" == "" ] ; then
    SHADERLIB=shader/shaderlibrary_nocg.xml 
else
    SHADERLIB="$2"
fi

shift
shift

./rendergirl js/sv.js $MODELNAME $SHADERLIB $*
