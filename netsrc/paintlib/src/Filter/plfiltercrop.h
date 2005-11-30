/*
/--------------------------------------------------------------------
|
|      $Id: plfiltercrop.h,v 1.5 2004/10/05 13:54:28 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLFILTERCROP)
#define INCL_PLFILTERCROP

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

class PLBmp;
class PLRect;

//! Filter that cuts part of the image off. XMin and YMin are included
//! in the rectangle, XMax and YMax aren't.
class PLFilterCrop : public PLFilter
{
public:

  //!
  PLFilterCrop(int XMin, int YMin, int XMax, int YMax);

  //!
  PLFilterCrop(const PLRect & rc);

  //!
  virtual ~PLFilterCrop();

  //!
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const;

private:
  int m_XMin;
  int m_XMax;
  int m_YMin;
  int m_YMax;

};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltercrop.h,v $
|      Revision 1.5  2004/10/05 13:54:28  artcom
|      *** empty log message ***
|
|      Revision 1.4  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.3  2002/02/11 16:45:37  uzadow
|      Fixed bug decoding 16 bit per channel tiffs.
|
|      Revision 1.2  2001/09/30 19:55:37  uzadow
|      Fixed bug for 8 bpp in PLFilterCrop.
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
