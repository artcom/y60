#! /bin/bash
MODELNAME="$1"
if [ "$2" == "" ] ; then
    if [ "$USE_CG" == "1" ] ; then
        SHADERLIB=$PRO/src/Y60/shader/shaderlibrary.xml
    else
    	SHADERLIB=$PRO/src/Y60/shader/shaderlibrary_nocg.xml
    fi
else
    SHADERLIB="$2"
fi

shift
shift

APPLICATION=acgtkshellOPT

if [ "$DEBUG" == "1" ] ; then
    APPLICATION=acgtkshellDBG
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
