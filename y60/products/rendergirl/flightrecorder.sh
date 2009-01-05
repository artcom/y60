#! /bin/bash

MODELNAME="$1"
shift

if [ "$1" == "" ] ; then
    if [ "$USE_CG" == "1" ] ; then
        SHADERLIB=$PRO/src/Y60/shader/shaderlibrary.xml
    else
    	SHADERLIB=$PRO/src/Y60/shader/shaderlibrary_nocg.xml
    fi
else
    SHADERLIB="$1"
fi
shift

APPLICATION=g60
if [ "$DEBUG" == "1" ] ; then
    APPLICATION=${APPLICATION}DBG
fi

SCRIPT=SCRIPTS/FlightRecorder.js
BASENAME=${MODELNAME%.*}

#if [ -e ${BASENAME}.js ]; then
#    SCRIPT=${BASENAME}.js
#fi

COMMAND="$APPLICATION -I SCRIPTS;$PRO/src/Y60/js;$PRO/lib $SCRIPT $MODELNAME $SHADERLIB $*"
echo $COMMAND
$COMMAND
exit $?
