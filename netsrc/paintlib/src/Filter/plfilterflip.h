/*
/--------------------------------------------------------------------
|
|      $Id: plfilterflip.h,v 1.4 2004/06/15 10:26:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|      Original code by Richard Hollis
|
\--------------------------------------------------------------------
*/

#ifndef FILTERFLIP_H
#define FILTERFLIP_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

//! Flips a 32 bpp bitmap upside-down
class PLFilterFlip: public PLFilter
{
public:
  PLFilterFlip();
  virtual ~PLFilterFlip();
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const;

};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterflip.h,v $
|      Revision 1.4  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.3  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.2  2002/02/24 13:00:46  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2001/09/13 20:48:42  uzadow
|      Added fill filters.
|
|      Revision 1.2  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.1  2001/01/13 20:06:16  uzadow
|      Added Flip and Mirror filters.
|
|
|
\--------------------------------------------------------------------
*/
