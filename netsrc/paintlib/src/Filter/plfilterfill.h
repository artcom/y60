/*
/--------------------------------------------------------------------
|
|      $Id: plfilterfill.h,v 1.8 2004/06/20 16:59:38 uzadow Exp $
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLFILTERFILL)
#define INCL_PLFILTERFILL

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "plinplacefilter.h"
#include "plfilterfillrect.h"

class PLBmp;

//! Filter that fills a bitmap with a color. Works for 8, 24 and 
//! 32 bpp 
template<class PixelC> class PLFilterFill : public PLInPlaceFilter
{
public:
  PLFilterFill (const PixelC& Color);
  virtual ~PLFilterFill();
  virtual void ApplyInPlace(PLBmpBase *pBmp) const;

private:
  PixelC m_Color;
};

template<class PixelC>
PLFilterFill<PixelC>::PLFilterFill(const PixelC& Color)
    : m_Color (Color)
{
}

template<class PixelC>
PLFilterFill<PixelC>::~PLFilterFill()
{
}

template<class PixelC>
void PLFilterFill<PixelC>::ApplyInPlace(PLBmpBase * pBmp) const
{
  PLFilterFillRect<PixelC> RectFilter(PLPoint(0,0),
        PLPoint (pBmp->GetWidth(), pBmp->GetHeight()), m_Color);
  RectFilter.ApplyInPlace(pBmp);
}

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterfill.h,v $
|      Revision 1.8  2004/06/20 16:59:38  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.7  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.6  2004/04/16 20:14:42  uzadow
|      Changes to make cdoc work.
|
|      Revision 1.5  2004/02/15 22:43:32  uzadow
|      Added 8-bit-support to DFBBitmap and PLFilterFill
|
|      Revision 1.4  2002/02/24 13:00:46  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.3  2002/02/11 16:45:37  uzadow
|      Fixed bug decoding 16 bit per channel tiffs.
|
|      Revision 1.2  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.1  2001/09/13 20:48:42  uzadow
|      Added fill filters.
|
|      Revision 1.2  2001/09/13 10:39:31  uzadow
|      Added FilterFillRect
|
|      Revision 1.1  2001/09/06 14:20:13  uzadow
|      Moved FilterFill to paintlib
|
|      Revision 1.1.2.1  2001/09/04 11:20:40  uzadow
|      Initial version: test works, no main program yet.
|
|
\--------------------------------------------------------------------
*/
