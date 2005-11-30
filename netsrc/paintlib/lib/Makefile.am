
MAINTAINERCLEANFILES = Makefile.in 
EXTRADIST = paintlib_version.h.in

#
# Build the libpaintlib library
#
lib_LTLIBRARIES = libpaintlib.la
libpaintlib_la_SOURCES = version.cpp

# -no-undefinef needed for libtool for contruct dlls correctly con windows
# using cygwin tools.

#libpaintlib_la_LDFLAGS = -no-undefined -version-info 1:0:0 $(SYS_LDFLAGS_LIBPNG) $(SYS_LDFLAGS_LIBJPEG) $(SYS_LDFLAGS_LIBTIFF) \
#      $(SYS_LDFLAGS_LIBUNGIF) $(SYS_LDFLAGS_LIBCURL) 
libpaintlib_la_LDFLAGS = -no-undefined -version-info 1:0:0 $(PAINTLIB_LIBS)
libpaintlib_la_LIBADD  =  ../common/libcommon.la ../common/Filter/libfilter.la 
