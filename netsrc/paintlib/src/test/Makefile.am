
INCLUDES=-I$(top_srcdir)/common/ -I$(top_srcdir)/common/Filter

ALL_H = pltest.h pltester.h testfilterresizebilinear.h testpsddecoder.h \
        pltestdecoders.h pltestfilters.h pltestcountedpointer.h \
        pltestexif.h pltestpixelformat.h

ALL_CPP = pltester.cpp testfilterresizebilinear.cpp testpsddecoder.cpp \
          pltest.cpp pltestdecoders.cpp pltestfilters.cpp pltestcountedpointer.cpp \
          pltestexif.cpp pltestpixelformat.cpp

# Libraries to generate
# noinst_LTLIBRARIES = libfilter.la
# -no-undefinef needed for libtool for contruct dlls correctly with windows
# using cygwin tools.
# libfilter_la_LDGFLAGS = -no-undefined
# libfilter_la_SOURCES = $(ALL_CPP) $(ALL_H)

paintlibdir = $(includedir)/paintlib
testdir = $(paintlibdir)/test
test_HEADERS = $(ALL_H)

noinst_PROGRAMS=testpaintlib
testpaintlib_SOURCES=testpaintlib.cpp $(ALL_CPP) $(ALL_H)
# testpaintlib_CFLAGS=$(SYS_CFLAGS_LIBSDL)
testpaintlib_LDFLAGS= ../libcommon.la ../Filter/libfilter.la $(PAINTLIB_LIBS)

