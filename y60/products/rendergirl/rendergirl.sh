#! /bin/bash

if [ $# -gt 0 ]; then
    MODELNAME="$1"
    shift
fi

if [ $# -gt 0 ]; then
    SHADERLIB="$1"
    shift
else
    if [ "$USE_CG" == "1" ] ; then
        SHADERLIB="$PRO/src/y60/shader/shaderlibrary.xml"
    else
    	SHADERLIB="$PRO/src/y60/shader/shaderlibrary_nocg.xml"
    fi
fi

APPLICATION="g60"
if [ "$DEBUG" == "1" ]; then
    APPLICATION="${APPLICATION}DBG"
fi

BASEDIR=`dirname $0`
case `uname` in
CYGWIN*)
    BASEDIR=`cygpath -d "${BASEDIR}"`
    ;;
esac

SCRIPTS="${BASEDIR}/SCRIPTS"
if [ ! -d ${SCRIPTS} ]; then
    SCRIPTS="${PRO}/src/y60/products/rendergirl/SCRIPTS"
fi
SCRIPT="${SCRIPTS}/rendergirl.js"

COMMAND="${APPLICATION} -I ${SCRIPTS};${PRO}/src/y60/js;${PRO}/lib ${SCRIPT} ${MODELNAME} ${SHADERLIB} $*"
echo $COMMAND
$COMMAND
exit $?
