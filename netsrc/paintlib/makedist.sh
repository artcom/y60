#!/bin/bash
export CPPFLAGS=-D_DEBUG
./reconfig && \
./configure &&\
make clean && \
make && \
common/test/testpaintlib && \
make dist && \
make dist-zip && \
tar -czf testpic.tar.gz testpic/* && \
zip testpic.zip testpic/*
