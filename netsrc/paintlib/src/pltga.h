/*
/--------------------------------------------------------------------
|
|      $Id: pltga.h,v 1.4 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! \file
//! Defines structures and constants present in TGA files.

#ifndef INCL_PLTGA
#define INCL_PLTGA

typedef struct _TgaHeader
{
    PLBYTE   IdLength;            // Image ID Field Length
    PLBYTE   CmapType;            // Color Map Type
    PLBYTE   ImageType;           // Image Type

    PLWORD   CmapIndex;           // First Entry Index
    PLWORD   CmapLength;          // Color Map Length
    PLBYTE   CmapEntrySize;       // Color Map Entry Size

    PLWORD   X_Origin;            // X-origin of Image
    PLWORD   Y_Origin;            // Y-origin of Image
    PLWORD   ImageWidth;          // Image Width
    PLWORD   ImageHeight;         // Image Height
    PLBYTE   PixelDepth;          // Pixel Depth
    PLBYTE   ImagDesc;            // Image Descriptor
} TGAHEADER;

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltga.h,v $
|      Revision 1.4  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.3  2002/02/24 13:00:40  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
