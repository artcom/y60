#!/bin/bash
if [[ "$1" == "" ]] ; then
    FILES=`ls *.tst.js`
else
    FILES=$1
fi


for FILE in $FILES; do
    PLUGIN_DIR="$PRO/lib"
    
    UNAME=`uname -o`
    PLATFORM=LINUX
    if [ $UNAME == "Cygwin" ]; then
        PLATFORM=WIN
    fi
    
    APPLICATION=acxpshellOPT
    if [ "$DEBUG" == "1" ] ; then
        mkdir -p o.ANT.${PLATFORM}.DBG/tests
        cd o.ANT.${PLATFORM}.DBG/tests
        APPLICATION=acxpshellDBG
    else
        mkdir -p o.ANT.${PLATFORM}.OPT/tests
        cd o.ANT.${PLATFORM}.OPT/tests
    fi
    
    echo $APPLICATION -I "$PRO/src/Y60/js;$PLUGIN_DIR" ../../$FILE "$PRO/src/Y60/shader/shaderlibrary_nocg.xml"
    $APPLICATION -I "$PRO/src/Y60/js;$PLUGIN_DIR" ../../$FILE "$PRO/src/Y60/shader/shaderlibrary_nocg.xml"
    cd ../..
done
exit 0