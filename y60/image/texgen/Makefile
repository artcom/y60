#!smake
# -*- mode: makefile -*- 
###
### -- Y50 common make environment - Release 1.3
### --
### -- Copyright (C) 1992-1999 Steffen Meschkat <mesch@mesch.org>
### -- Copyright (C) 1997-1999 ART+COM AG
### --
### -- $Author: uzadow $, $Date: 2004/10/06 10:17:14 $, $Revision: 1.1 $ 
### 
###
### == DON'T use ``###'' for your own comments -- they mark the copyright and
### == release message and will be changed automatically.
###
###
### == See http://pro.artcom.de/make/ for documentation.
###

#ifndef ___GNUMAKE
#include "ac-make-library"
#else
include ac-make-library
#endif

PREFIX       =  tex
NAME         =  TexGen# the name of the library without any prefix of variant suffix

LIBHEADERS   = TerrainTexGen.h TextureGenerator.h# source file names with .h suffix
TEMPLATES    = # source file names with .c++ suffix
CLASSES      = XmlHelper LitTerrainTexGen FilterTile TestFilterTile TestTerrainTexGen \
               TerrainTexGen TestTextureDefinition TestXMLTexGen LayerDefinition \
               TextureDefinition TextureGenerator TestTextureGenerator TestTiming \
               FilterIntDownscale 
LIBOBJECTS   = # object file names with .o suffix

TESTS        = testTexProc.tst # names of the executables without any variant suffix
TESTLIBS     = # -l options of the linker command line
VTESTLIBS    = # -l options of the linker command line, LIBVARIANT appended

LCDEFS       = # C compiler -D command line options
LCINCS       = # C compiler -I command line options
LCOPTS       = # other C compiler command line options

LC++DEFS     = # C++ compiler -D command line options
LC++INCS     = -I$(PRO)/include/pl# C++ compiler -I command line options
LC++OPTS     = #-DNDEBUG# other C++ compiler command line options

LLDOPTS      = # C++ compiler -L and other linker stage command line options
LLDLIBS      = -lpng -ltiff -lz -ljpeg # C++ compiler -l command line options
VLLDLIBS     = -lplfilter -lplpaintlib -lxml # C++ compiler -l command line options

STATIC_VLLDLIBS = # C++ compiler -l command line options
STATIC_LLDLIBS = # C++ compiler -l command line options

LLDDSOOPTS   = # C++ compiler linker stage command line options relevant to DSO

#ifndef __GNUMAKE
#include "$(RULES)"
#else
include $(RULES)
#endif
