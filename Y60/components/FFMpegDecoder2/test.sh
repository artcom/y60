Y60_DIR="$PRO/src/Y60"
APP_DIR=`dirname $0`
PLUGIN_DIR="$PRO/lib"

if [ "$USE_CG" == "1" ]; then
SHADERLIB="$Y60_DIR/shader/shaderlibrary.xml"
else
SHADERLIB="$Y60_DIR/shader/shaderlibrary_nocg.xml"
fi

APP=y60

if [ "$DEBUG" = "1" ] ; then
    APP=$PRO/bin/y60DBG
fi


#export AC_LOG_MODULE_VERBOSITY=TRACE/FFMpegDecoder.cpp
#export Y60_FRAME_ANALYSER=5

ARGS="-I $PRO/lib;$PRO/src/Y60/js MovieTest.js $PRO/src/Y60/shader/shaderlibrary.xml"
COMMAND="$APP $ARGS"

if [ "$DEBUG" = "vc" ]; then
    echo Visual Studio Debuger Setup
    echo --------------------------------------------------------------------------------------
    echo $PRO/bin/y60DBG.exe
    echo $ARGS
    echo `cmd /C cd`
    echo --------------------------------------------------------------------------------------
    exit 0
fi

echo $COMMAND
$COMMAND


JS_APP="$APP_DIR/testImageMovieLeak.js"
ARGS="-I $Y60_DIR/js;$PLUGIN_DIR $JS_APP $SHADERLIB"
COMMAND="$APP $ARGS"
echo $COMMAND
$COMMAND

exit $?
