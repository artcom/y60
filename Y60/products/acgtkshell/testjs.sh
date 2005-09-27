#!/bin/bash
if [[ "$1" == "" ]] ; then
    JSCRIPT=../../testGTK.tst.js
else
    JSCRIPT=../../$1
fi

PLUGIN_DIR="$PRO/lib"

UNAME=`uname -o`
PLATFORM=LINUX
if [ $UNAME == "Cygwin" ]; then
    PLATFORM=WIN
fi

APPLICATION=acgtkshellOPT
if [ "$DEBUG" == "1" ] ; then
    mkdir -p o.ANT.${PLATFORM}.DBG/tests
    cd o.ANT.${PLATFORM}.DBG/tests
    APPLICATION=acgtkshellDBG
else
    mkdir -p o.ANT.${PLATFORM}.OPT/tests
    cd o.ANT.${PLATFORM}.OPT/tests
fi

echo $APPLICATION -I "$PRO/src/Y60/js;$PLUGIN_DIR" $JSCRIPT $*
$APPLICATION -I "$PRO/src/Y60/js;$PLUGIN_DIR" $JSCRIPT $*

if [ "$?" != "0" ] ; then
echo "test {$JSCRIPT} failed"
exit -1
fi
