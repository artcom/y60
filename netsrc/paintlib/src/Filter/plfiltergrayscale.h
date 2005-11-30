/*
/--------------------------------------------------------------------
|
|      $Id: plfiltergrayscale.h,v 1.4 2004/06/15 10:26:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLFILTERGRAYSCALE)
#define INCL_PLFILTERGRAYSCALE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

class PLBmp;

//! Creates a grayscale version of a 32 bpp bitmap.
class PLFilterGrayscale : public PLFilter
{
public:
  PLFilterGrayscale();
  virtual ~PLFilterGrayscale();
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const;

private:
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltergrayscale.h,v $
|      Revision 1.4  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.3  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.2  2002/02/24 13:00:47  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.4  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.3  1999/12/08 15:39:46  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.2  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/21 16:05:17  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:44  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
