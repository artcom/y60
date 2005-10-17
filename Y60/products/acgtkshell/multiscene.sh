#! /bin/bash

Y60_DIR="$PRO/src/Y60"
SCRIPTS_DIR=`dirname $0`/SCRIPTS

#
# shaderlib
#
if [ "$USE_CG" == "1" ]; then
SHADERLIB="$Y60_DIR/shader/shaderlibrary.xml"
else
SHADERLIB="$Y60_DIR/shader/shaderlibrary_nocg.xml"
fi

unset __GL_FSAA_MODE
export __GL_SYNC_TO_VBLANK=1

#
# spidermonkey and kernel 2.6 pthreads need this to
# avoid a floating point exception on loading
#
if uname -r | grep -q "^2.6" ; then
    export LD_ASSUME_KERNEL="2.4.99"
    echo "setting LD_ASSUME_KERNEL=$LD_ASSUME_KERNEL"
fi

#
# debug or release mode
#
SHELL="acgtkshell"
if [ ! -z "$DEBUG" -a "$DEBUG" != "0" ]; then
    SHELL="acgtkshellDBG"
fi

APP="$SCRIPTS_DIR/multiscene.js"
CMD="$SHELL -I $Y60_DIR/js;$PRO/lib $APP $SHADERLIB $ARG"

echo "### $CMD" "$@"
#exec $CMD
$CMD "$@"

