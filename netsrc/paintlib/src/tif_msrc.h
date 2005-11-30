/*
/--------------------------------------------------------------------
|
|      $Id: tif_msrc.h,v 1.8 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! \file
//! Custom data source for libtiff. Assumes all data is in memory at
//! start.


#include <tiffio.h>

extern TIFF* TIFFOpenMem (unsigned char* pData,
                            int FileSize,
                            int* pCurPos);


typedef struct
{
  unsigned char* pData;
  int    CurPos;
  int    MaxFileSize;
  // used when open for reading
  int    rFileSize;
  // not NULL when open for writing
  int*   pFileSize;
  char   mode[2];
}
MemSrcTIFFInfo;
/*
/--------------------------------------------------------------------
|
|      $Log: tif_msrc.h,v $
|      Revision 1.8  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.7  2002/08/07 19:03:58  uzadow
|      no message
|
|      Revision 1.6  2002/08/07 18:33:51  uzadow
|      Removed dependency on internal libtiff header tiffiop.h
|
|      Revision 1.5  2002/02/24 13:00:43  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
