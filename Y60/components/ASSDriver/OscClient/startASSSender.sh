#!/bin/sh

Y60=`pwd`/..
export PATH=$Y60:$PATH
export LD_LIBRARY_PATH=$Y60:$LD_LIBRARY_PATH

echo $LD_LIBRARY_PATH

y60 client.js
