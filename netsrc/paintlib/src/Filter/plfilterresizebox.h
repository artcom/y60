/*
/--------------------------------------------------------------------
|
|      $Id: plfilterresizebox.h,v 1.6 2004/06/15 10:26:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERResizeBOX_H
#define FILTERResizeBOX_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilterresize.h"

//! Resizes a 32 bpp bitmap using a box filter to calculate results.
class PLFilterResizeBox : public PLFilterResize
{
public:
  //! 
  PLFilterResizeBox (int NewXSize, int NewYSize);
  //! 
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const;

private: 
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterresizebox.h,v $
|      Revision 1.6  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.5  2002/02/24 13:00:48  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.4  2001/09/30 16:57:26  uzadow
|      Improved speed of 2passfilter.h, code readability changes.
|
|      Revision 1.3  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.2  2001/09/16 20:57:17  uzadow
|      Linux version name prefix changes
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.2  1999/11/27 18:45:49  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/21 16:05:18  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:45  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
