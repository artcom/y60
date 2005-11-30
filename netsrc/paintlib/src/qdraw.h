/*
/--------------------------------------------------------------------
|
|      $Id: qdraw.h,v 1.7 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! \file
//! This file defines the data structures used in pict files. They
//! correspond to the appropriate MAC QuickDraw structs. See
//! QuickDraw docs for an explanation of these structs.

#ifndef INCL_PLQDRAW
#define INCL_PLQDRAW

struct MacRect
{
  PLWORD top;
  PLWORD left;
  PLWORD bottom;
  PLWORD right;
};

struct MacpixMap
{
  // Ptr baseAddr              // Not used in file.
  // short rowBytes            // read in seperatly.
  struct MacRect Bounds;
  PLWORD version;
  PLWORD packType;
  PLLONG packSize;
  PLLONG hRes;
  PLLONG vRes;
  PLWORD pixelType;
  PLWORD pixelSize;
  PLWORD cmpCount;
  PLWORD cmpSize;
  PLLONG planeBytes;
  PLLONG pmTable;
  PLLONG pmReserved;
};

struct MacRGBColour
{
  PLWORD red;
  PLWORD green;
  PLWORD blue;
};

struct MacPoint
{
  PLWORD x;
  PLWORD y;
};

struct MacPattern // Klaube
{
  PLBYTE pix[64];
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: qdraw.h,v $
|      Revision 1.7  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.6  2002/02/24 13:00:42  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.5  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
