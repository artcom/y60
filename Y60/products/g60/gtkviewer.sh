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
# debug or release mode
#
SHELL="g60"
if [ ! -z "$DEBUG" -a "$DEBUG" != "0" ]; then
    SHELL="g60DBG"
fi

APP="$SCRIPTS_DIR/gtkviewer.js"
CMD="$SHELL -I $Y60_DIR/js;$PRO/lib $APP $SHADERLIB $ARG"

echo "### $CMD" "$@"
#exec $CMD
$CMD "$@"

