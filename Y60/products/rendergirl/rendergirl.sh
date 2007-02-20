#! /bin/bash

BASEDIR=`dirname $0`

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

APPLICATION="acgtkshell"
if [ "$DEBUG" == "1" ]; then
    APPLICATION="${APPLICATION}DBG"
fi

SCRIPTS="${BASEDIR}/SCRIPTS"
if [ ! -d ${SCRIPTS} ]; then
    SCRIPTS="${PRO}/src/Y60/products/rendergirl/SCRIPTS"
fi
SCRIPT="${SCRIPTS}/rendergirl.js"

#BASENAME=${MODELNAME%.*}
#if [ -e ${BASENAME}.js ]; then
#    SCRIPT=${BASENAME}.js
#fi

COMMAND="${APPLICATION} -I ${SCRIPTS};${PRO}/src/Y60/js;${PRO}/lib ${SCRIPT} ${MODELNAME} ${SHADERLIB} $*"
echo $COMMAND
$COMMAND
exit $?
