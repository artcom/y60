APP=acxpshellOPT.exe
if [ "$DEBUG" = "1" ] ; then
    APP=acxpshellDBG.exe
fi
#AC_LOG_MODULE_VERBOSITY=TRACE/FFMpegDecoder.cpp
$APP -I "$PRO/lib;$PRO/src/Y60/js" MovieTest.js $PRO/src/Y60/shader/shaderlibrary.xml