#! /bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: `basename $0` <model> [<shaderlibrary>]"
    exit 1
fi

MODELNAME="$1"
shift

if [ $# -eq 0 ]; then
    if [ "$USE_CG" == "1" ] ; then
        SHADERLIB="$PRO/src/Y60/shader/shaderlibrary.xml"
    else
    	SHADERLIB="$PRO/src/Y60/shader/shaderlibrary_nocg.xml"
    fi
else
    SHADERLIB="$1"
    shift
fi

APPLICATION="acgtkshell"
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
    SCRIPTS="${PRO}/src/Y60/products/rendergirl/SCRIPTS"
fi
SCRIPT="${SCRIPTS}/rendergirl.js"

COMMAND="${APPLICATION} -I ${SCRIPTS};${PRO}/src/Y60/js;${PRO}/lib ${SCRIPT} ${MODELNAME} ${SHADERLIB} $*"
echo $COMMAND
$COMMAND
exit $?
