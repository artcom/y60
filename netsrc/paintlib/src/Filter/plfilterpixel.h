/*
/--------------------------------------------------------------------
|
|      $Id: plfilterpixel.h,v 1.4 2002/08/04 20:08:01 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_FILTERPIXEL
#define INCL_FILTERPIXEL

#if _MSC_VER > 1000
#pragma once
#endif

#include "plfilter.h"

#include "plpixel32.h"
#include "plpixel24.h"
#include "plpixel8.h"

#include <typeinfo.h>

template <class PixelC, class PixelOp>
class PLFilterPixel : public PLFilter
{
public:
  PLFilterPixel(const PixelOp & Op);

  virtual ~PLFilterPixel();

  void ApplyInPlace(PLBmp * pBmp) const;

private:
  PixelOp m_Op;
};

template <class PixelC, class PixelOp>
PLFilterPixel<PixelC, PixelOp>::PLFilterPixel
    (const PixelOp & Op)
  : m_Op (Op)
{
}

template <class PixelC, class PixelOp>
PLFilterPixel<PixelC, PixelOp>::~PLFilterPixel ()
{
}

template <class PixelC, class PixelOp>
void PLFilterPixel<PixelC, PixelOp>::ApplyInPlace (PLBmp *pBmp) const
{
  PixelC** ppLines = (PixelC**)(pBmp->GetLineArray());

  switch (pBmp->GetBitsPerPixel())
  {
    case 32:
      PLASSERT (typeid(PixelC) == typeid (PLPixel32));
      break;
    case 24:
      PLASSERT (typeid(PixelC) == typeid (PLPixel24));
      break;
    case 8:
      PLASSERT (typeid(PixelC) == typeid (PLPixel8));
      break;
    default:
      PLASSERT (false);
  }

  for (int y=0; y<pBmp->GetHeight(); ++y)
  {
    PixelC* pLine = ppLines[y];
    for (int x=0; x<pBmp->GetWidth(); ++x)
      pLine[x] = m_Op.Apply(pLine[x]);
  }
}

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterpixel.h,v $
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/02/24 13:00:47  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2002/02/10 22:53:26  uzadow
|      Fixed cdoc problems.
|
|      Revision 1.1  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|
\--------------------------------------------------------------------
*/
