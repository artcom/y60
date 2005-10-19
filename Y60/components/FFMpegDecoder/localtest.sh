APP=y60
if [ "$DEBUG" = "1" ] ; then
    APP=$PRO/bin/y60DBG
fi


export AC_LOG_MODULE_VERBOSITY=TRACE/FFMpegDecoder.cpp
#Y60_FRAME_ANALYSER=5

ARGS="-I $PRO/lib;$PRO/src/Y60/js MovieTest.js $PRO/src/Y60/shader/shaderlibrary.xml"
COMMAND="$APP $ARGS"

if [ "$DEBUG" = "vc" ]; then
    echo Visual Studio Debuger Setup
    echo --------------------------------------------------------------------------------------
    echo $PRO/bin/acxpshellDBG.exe
    echo $ARGS
    echo `cmd /C cd`
    echo --------------------------------------------------------------------------------------
    exit 0
fi

echo $COMMAND
$COMMAND
exit $?
