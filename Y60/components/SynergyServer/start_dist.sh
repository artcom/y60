unset PRO
unset Y60_PATH

export PATH=/opt/ART+COM/SynergyServer:$PATH
export LD_LIBRARY_PATH=/opt/ART+COM/SynergyServer

y60 ./demo.js $* &
synergyc --no-daemon localhost
