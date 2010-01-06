#!/bin/sh

if [ -z "$C4D_SDK" ]; then
    echo "### WARNING: $C4D_SDK environment variable not set - skipping installation"
    exit 0
fi

C4D_OS="$C4D_SDK/_api/OperatingSystem.h"
if [ ! -f "$C4D_OS" ]; then
    echo "### Unable to find $C4D_OS"
    exit 1
fi

PATCHFILE_H="c4d_R11_memory.h.patch"
PATCHFILE_CPP="c4d_R11_memory.cpp.patch"
if [ ! -f $PATCHFILE_H ]; then
    echo "### Patch $PATCHFILE_H does not exist"
    exit 1
else
    echo "Using patch $PATCHFILE_H"
fi


echo "*** Copying $PATCHFILE_H to $C4D_SDK"
cp -a "$PATCHFILE_H" "$C4D_SDK/_api" || exit 1
echo "*** Copying $PATCHFILE_CPP to $C4D_SDK"
cp -a "$PATCHFILE_CPP" "$C4D_SDK/_api" || exit 1

echo "*** Patching"
cd "$C4D_SDK/_api"
pwd
/usr/bin/patch --forward -i "$PATCHFILE_H" || exit 1
rm $PATCHFILE_H
/usr/bin/patch --forward -i "$PATCHFILE_CPP" || exit 1
rm $PATCHFILE_CPP
cd -