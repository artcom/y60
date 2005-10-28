#! /bin/bash

Y60_DIR="$PRO/src/Y60"
APP_DIR=`dirname $0`
PLUGIN_DIR="$PRO/lib"

#
# debug or release mode
#
SHELL="y60"
if [ ! -z "$DEBUG" -a "$DEBUG" != "0" ]; then
    SHELL="y60DBG"
fi

APP="$APP_DIR/testWMP.js"
CMD="$SHELL -I $Y60_DIR/js;$PLUGIN_DIR $APP $*"

echo "### $CMD"
$CMD
