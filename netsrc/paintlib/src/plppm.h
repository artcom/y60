/*
/--------------------------------------------------------------------
|
|      $Id: plppm.h,v 1.3 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 2001 Ulrich von Zadow
|
|      Originally by Todd Harris - Mar 2001
|      adadapted from pgm.h originally by
|			 Jose Miguel Buenaposada Biencinto. Mar 2000.
|
\--------------------------------------------------------------------
*/

//! \file
//! Defines structures and constants present in PGM files.

#ifndef INCL_PLPPM
#define INCL_PLPPM

typedef struct _PpmHeader
{
    int ImageType;           // Image Type
    int ImageWidth;          // Image Width
    int ImageHeight;         // Image Height
  	int MaxSampleValue;		   // Max sample value (RBG)

} PPMHEADER;

#define PPM_MAXLINESIZE 80  // Maximum number of characters per line

// Definitions for image types.
#define PPM_P3 0
#define PPM_P6 1

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plppm.h,v $
|      Revision 1.3  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.2  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.1  2001/10/16 17:51:32  uzadow
|      Added ppm support (Todd Harris)
|
|
\--------------------------------------------------------------------
*/
