#
#   Process this file with autoconf to produce a configure script.

AC_INIT(common/plbitmap.h)
AC_CONFIG_AUX_DIR(config)

MAJOR_VERSION=2
MINOR_VERSION=6
MICRO_VERSION=1
VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION dnl

AC_SUBST(VERSION)

PACKAGE=paintlib

AM_CONFIG_HEADER(common/config.h)
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AC_C_BIGENDIAN

# Checks for libraries.
AC_CHECK_LIB(c, main)
AC_CHECK_LIB(m, main)
AC_CHECK_LIB(stdc, main)
AC_CHECK_LIB(stdcx, main)

# Checks for header files.
AC_HEADER_STDC

# Checks which header(s) contain dirent stuff
AC_HEADER_DIRENT

# Checks if memory-mapped files are available
AC_FUNC_MMAP

# Checks for library functions.
AC_FUNC_MEMCMP


# Mac OS X Framework support
# TODO: I'm not sure if this ever gets enabled. 
AC_ARG_ENABLE(extra-framework-path, AC_HELP_STRING([--enable-extra-framework-path], [additionally search for mac os x frameworks here.]),
[ac_xtra_fmk_path="$ac_xtra_fmk_path -F$enableval"
AC_MSG_NOTICE([enabled extra mac os x framework path: $enableval])
AC_SUBST([FRAMEWORK_CFLAGS], [$ac_xtra_fmk_path])], 
[ac_xtra_fmk_path=""
AC_MSG_NOTICE([disabled extra mac os x framework path.])
AC_SUBST([FRAMEWORK_CFLAGS], [$ac_xtra_fmk_path])])

# Defines for the graphic file formats supported

AC_CHECK_LIB(tiff, main, sys_libtiff=yes, sys_libtiff=no)
if test "$sys_libtiff" = yes; then
AC_DEFINE(PL_SUPPORT_TIFF)
libstiff="-ltiff"
fi

AC_CHECK_LIB(jpeg, main, sys_libjpeg=yes, sys_libjpeg=no)
if test "$sys_libjpeg" = yes; then
AC_DEFINE(PL_SUPPORT_JPEG)
libsjpeg="-ljpeg"
fi

AC_CHECK_LIB(png, main, sys_png=yes, sys_png=no, -lz)
if test "$sys_png" = yes; then
AC_DEFINE(PL_SUPPORT_PNG)
libspng="-lz -lpng"
fi

AC_CHECK_LIB(ungif, main, sys_libungif=yes, sys_libungif=no)
if test "$sys_libungif" = yes; then
AC_DEFINE(PL_SUPPORT_GIF)
libsungif="-lungif"
fi

AC_CHECK_LIB(curl, main, sys_libcurl=yes, sys_libcurl=no)
if test "$sys_libcurl" = yes; then
AC_DEFINE(PL_SUPPORT_CURL)
libscurl="-lcurl"
fi

case "$host" in
*-cygwin* | *-mingw*)
  mswindows=yes
  ;;
*)
  mswindows=no
  ;;
esac
# Check for Windows platform
AM_CONDITIONAL(WIN32, test "$mswindows" = yes)


AC_DEFINE(PL_SUPPORT_TGA)
AC_DEFINE(PL_SUPPORT_BMP)
AC_DEFINE(PL_SUPPORT_PICT)
AC_DEFINE(PL_SUPPORT_PCX)
AC_DEFINE(PL_SUPPORT_PGM)
AC_DEFINE(PL_SUPPORT_PPM)
AC_DEFINE(PL_SUPPORT_PSD)
AC_DEFINE(PL_SUPPORT_SGI)
AC_DEFINE(PL_SUPPORT_IFF85)

AM_CONDITIONAL(SYS_LIBJPEG, test "$sys_libjpeg" = yes)
AM_CONDITIONAL(SYS_LIBTIFF, test "$sys_libtiff" = yes)
AM_CONDITIONAL(SYS_LIBPNG, test "$sys_png" = yes)
AM_CONDITIONAL(SYS_LIBUNGIF, test "$sys_libungif" = yes)
AM_CONDITIONAL(SYS_LIBCURL, test "$sys_libcurl" = yes)

# Now establish PAINTLIB_CFLAGS and PAINTLIB_LIBS
PAINTLIB_CFLAGS="$CFLAGS -I$includedir -D_REENTRANT"
PAINTLIB_LIBS="$libspng $libsjpeg $libstiff $libsungif $libscurl -L$libdir"

AC_SUBST(PAINTLIB_CFLAGS)
AC_SUBST(PAINTLIB_LIBS)

# TODO: Why were the next two lines put in here?
# This hurts so much
# AM_CONDITIONAL(LIBTIFF_MACOSX, test "$OSTYPE" = "darwin7.0")
if test "$OSTYPE" = "darwin7.0"; then
CXXFLAGS="$CXXFLAGS -g2 -ggdb -O0"
fi

AC_SUBST(CXX)
AC_SUBST(CXXFLAGS)


AC_OUTPUT(paintlib-config 
          lib/paintlib_version.h
          Makefile
          common/Makefile
          common/Filter/Makefile
          common/test/Makefile
          lib/Makefile)
