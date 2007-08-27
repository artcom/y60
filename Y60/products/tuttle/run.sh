#!/bin/sh

export CLISH_PATH=`pwd`/xml

if [ ! -f tuttle ]; then
  make depend
  make tuttle
fi

./tuttle

