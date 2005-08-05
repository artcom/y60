#!/bin/bash
if [[ !(-n $1) ]]
then
  echo "Usage: ./install.sh DBG|OPT"
  echo "Note: Make sure director is installed where this script expects it. Or change the script."
else  
  cp -v $PRO/src/asl/net/DirectorIP/o.ANT.WIN.$1/aslDirectorIP$1.dll "/cygdrive/c/Programme/Macromedia/Director 8.5G/Xtras/DirectorIP.x32"
fi
