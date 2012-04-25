#!/bin/sh

#=============================================================================
# Copyright (C) 2004-2012, ART+COM AG Berlin
#
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information of ART+COM AG Berlin, and
# are copy protected by law. They may not be disclosed to third parties
# or copied or duplicated in any form, in whole or in part, without the
# specific, prior written permission of ART+COM AG Berlin.
#=============================================================================

if [ $# -eq 0 ]; then
    echo "Usage: `basename $0` [-m] [-b] [-i] <file.mb> ...]"
    echo "-b Export as binary"
    echo "-i Export with inline textures"
    exit 0
fi

# parse arguments
IF_MODIFIED=0
BINARY=0
INLINE_TEXTURES=0
FILES=""

while [ $# -gt 0 ]; do
    ARG=$1
    shift

    case "$ARG" in
    -b)
        BINARY=1
        ;;
    -i)
        INLINE_TEXTURES=1
        ;;
    *)
        FILES="${FILES} ${ARG}"
        ;;
    esac
done

# determine location of maya exporter binary
if [ -z "$MAYA_SDK" ]; then
    echo "### ERROR: MAYA_SDK is not set"
    exit 1
fi

PROJ=`pwd` 


case `uname` in
CYGWIN*)
    MAYA_SDK=`cygpath -m ${MAYA_SDK}`
    # Escape spaces
    MAYA_SDK=`echo $MAYA_SDK | sed -e 's/\\\\/\\\\\\\\/g' -e 's/ /\\\\ /g'`
    MAYA_EXPORTER=`cygpath -m $USERPROFILE/"Eigene Dateien"/maya/${MAYA_VERSION}/plug-ins/mayaY60export.mll`
    PROJ=`cygpath -m $PROJ`
    MAYA_APP="$MAYA_SDK/bin/mayabatch"
    ;;
Linux*)
    if [ -z "$MAYA_VERSION" ]; then
        echo "### WARNING: MAYA_VERSION is not set"
    fi
    MAYA_DIR="${HOME}/maya/${MAYA_VERSION}"
    MAYA_EXPORTER="${MAYA_DIR}/plug-ins/mayaY60export.so"
    MAYA_APP="$MAYA_SDK/bin/maya"
    ;;
Darwin*)
    if [ -z "$MAYA_VERSION" ]; then
        MAYA_VERSION="8.5"
#        echo "### WARNING: MAYA_VERSION is not set, assuming $MAYA_VERSION"
    fi
    MAYA_APP="$MAYA_SDK/../Maya.app/Contents/bin/maya"
    MAYA_EXPORTER="/Users/Shared/Autodesk/maya/${MAYA_VERSION}/plug-ins/mayaY60export.bundle"
    ;;
*)
    ;;
esac

if [ ! -f "${MAYA_EXPORTER}" ]; then
    echo "### WARNING: Maya exporter not found in $MAYA_EXPORTER"
    #look for a distribution installation
    case `uname` in
    CYGWIN*)
        #MAYA_EXPORTER=`cygpath -m ${PROGRAMFILES}/ART+COM/mayaY60export/plug-ins/mayaY60export.mll`
        MAYA_EXPORTER="${MAYA_SDK}/bin/plug-ins/mayaY60export.mll"
        ;;
    Linux*)
        MAYA_EXPORTER="/opt/ART+COM/mayaY60export/plug-ins/mayaY60export.so"
        ;;
    *)
        ;;
    esac
    echo "             searching in $MAYA_EXPORTER ..."
    if [ ! -f "${MAYA_EXPORTER}" ]; then
        echo "### ERROR: Maya exporter not found in $MAYA_EXPORTER"
        exit 1
    fi
    echo "             ... found it!"
fi

for F in $FILES ; do

    infile="${F%.*}"
    infile="${infile}.mb"
    if [ ! -f "${infile}" ]; then
        echo "### WARNING: File not found ${infile}"
        continue
    fi

    outfile="${F%.*}"
    if [ "$BINARY" == "1" ]; then
        outfile="${PROJ}/${outfile}.b60"
    else
        outfile="${PROJ}/${outfile}.x60"
    fi

   
    echo "$MAYA_APP" -file "${infile}" -proj "$PROJ" -script "acExportAll.mel" -command "acExportAll \"${outfile}\" \"${BINARY}\" \"${INLINE_TEXTURES}\" "
    "$MAYA_APP" -file "${infile}" -proj "$PROJ" -script "acExportAll.mel" -command "acExportAll \"${outfile}\" \"${BINARY}\" \"${INLINE_TEXTURES}\" "

    if [ ! -f "${outfile}" ]; then
        echo "### ERROR: Failed to export ${outfile}"
        exit 1
        fi
done
