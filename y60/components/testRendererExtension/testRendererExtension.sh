#!/bin/bash

Y60_DIR="$PRO/src/Y60"
APP_DIR=`dirname $0`
PLUGIN_DIR="$PRO/lib"

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
SHELL="y60"
if [ ! -z "$DEBUG" -a "$DEBUG" != "0" ]; then
    SHELL="y60DBG"
fi

APP="$APP_DIR/testRendererExtension.tst.js"
CMD="$SHELL -I $Y60_DIR/js;$PLUGIN_DIR $APP $SHADERLIB $*"

echo "### $CMD"
#exec $CMD
$CMD
