/*
/--------------------------------------------------------------------
|
|      $Id: plpcx.h,v 1.4 2003/08/03 10:58:32 uzadow Exp $
|
|      Defines structures and constants present in PCX files.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPCX
#define INCL_PLPCX

// PCX Image File
typedef struct tagPCXHEADER
{
  char Manufacturer;  // always 0X0A
  char Version;  // version number
  char Encoding;  // always 1
  char BitsPerPixel;  // color bits
  PLWORD Xmin, Ymin;  // image origin
  PLWORD Xmax, Ymax;  // image dimensions
  PLWORD Hres, Vres;  // resolution values
  PLBYTE ColorMap[16][3];  // color palette
  char Reserved;
  char ColorPlanes;  // color planes
  PLWORD BytesPerLine;  // line buffer size
  PLWORD PaletteType;  // grey or color palette
  char Filter[58];
} PCXHEADER;

#define PCX_MAGIC 0X0A  // PCX magic number
#define PCX_256_COLORS 0X0C  // magic number for 256 colors
#define PCX_HDR_SIZE 128  // size of PCX header
#define PCX_MAXCOLORS 256
#define PCX_MAXPLANES 4
#define PCX_MAXVAL 255

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plpcx.h,v $
|      Revision 1.4  2003/08/03 10:58:32  uzadow
|      Windows port of PLFilterFlipRGB. Fixed PLPCXDecoder.
|
|      Revision 1.3  2002/02/24 13:00:22  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/12/15 21:16:30  Ulrich von Zadow
|      Removed references to gif in pcx decoder.
|
|
\--------------------------------------------------------------------
*/
