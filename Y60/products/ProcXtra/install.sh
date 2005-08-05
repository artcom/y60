#!/bin/bash

VAR=""
if [ $# -ge 1 ]; then
	VAR=$1
fi
if [ "$VAR" == "" ]; then
	for V in DBG OPT ; do
		if [ -f o.ANT.WIN.$V/acProcXtra$V.dll ]; then
			echo "Using $V variant"
			VAR=$V
			break
		fi
	done
fi
if [ "$VAR" != "OPT" -a "$VAR" != "DBG" ]; then
	echo "Usage: ./install.sh DBG|OPT"
	echo "Note: Make sure Director is installed where this script expects it. Or change the script."
	exit 1
fi

SRC="o.ANT.WIN.$VAR/acProcXtra$VAR.dll"
if [ ! -f $SRC ]; then
	echo "File does not exist: $SRC"
	exit 1
fi
DST="/cygdrive/c/Programme/Macromedia/Director 8.5G/Xtras/Proc.x32"

echo cp $SRC $DST
cp "$SRC" "$DST"
