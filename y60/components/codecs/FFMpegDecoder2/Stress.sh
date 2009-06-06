APP=y60
if [ "$DEBUG" = "1" ] ; then
    APP=$PRO/bin/y60DBG
fi

#export AC_LOG_MODULE_VERBOSITY=TRACE/FFMpegDecoder.cpp

ARGS="-I $PRO/lib;$PRO/src/y60/js StressTest.js $PRO/src/y60/shader/shaderlibrary.xml"
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
exit $?
