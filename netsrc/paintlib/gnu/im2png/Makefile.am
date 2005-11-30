
MAINTAINERCLEANFILES=Makefile.in

#
# There is PNG Support from the library ?
#
if SYS_LIBPNG
SYS_LDFLAGS_LIBPNG=-lz -lpng
else
SYS_LDFLAGS_LIBPNG=
endif

#
# There is Tiff Support from the library ?
#
if SYS_LIBTIFF
SYS_LDFLAGS_LIBTIFF=-ltiff
else
SYS_LDFLAGS_LIBTIFF=
endif

if SYS_LIBCURL
SYS_LDFLAGS_LIBCURL=-lcurl
else
SYS_LDFLAGS_LIBCURL=
endif

#
# There is JPEG Support from the library ?
#
if SYS_LIBJPEG
SYS_LDFLAGS_LIBJPEG=-ljpeg
else
SYS_LDFLAGS_LIBJPEG=
endif

#
# There is GIF Support from the library ?
#
if SYS_LIBUNGIF
SYS_LDFLAGS_LIBUNGIF=-lungif -lX11
else
SYS_LDFLAGS_LIBUNGIF=
endif

if SYS_LIBSDL
SYS_LDFLAGS_LIBSDL=-lSDL -lpthread
else
SYS_LDFLAGS_LIBSDL=
endif

if SYS_LIBDFB
SYS_LDFLAGS_LIBDFB = `directfb-config --libs`
DFB_CFLAGS = `directfb-config --cflags`
else
SYS_LDFLAGS_LIBDFB = 
DFB_CFLAGS =
endif

INCLUDES=-I$(top_srcdir)/common/ -I$(top_srcdir)/common/Filter $(DFB_CFLAGS)

noinst_PROGRAMS=im2png
im2png_SOURCES=im2png.cpp im2png.h
im2png_LDFLAGS=$(SYS_LDFLAGS_LIBPNG) $(SYS_LDFLAGS_LIBJPEG) $(SYS_LDFLAGS_LIBTIFF) $(SYS_LDFLAGS_LIBUNGIF) \
               $(SYS_LDFLAGS_LIBCURL) $(SYS_LDFLAGS_LIBSDL) $(SYS_LDFLAGS_LIBDFB)
im2png_LDADD=../../lib/libpaintlib.la
