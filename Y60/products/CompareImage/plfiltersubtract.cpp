/*
/--------------------------------------------------------------------
|
|      $Id: plfiltersubtract.cpp,v 1.3 2004/10/05 13:53:43 uzadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfiltersubtract.h"

#include <math.h>

#include <paintlib/plbitmap.h>
#include <paintlib/plpoint.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PLFilterSubtract::PLFilterSubtract(PLBmpBase * pBmpSubtrahend)
    : PLFilter(),
      m_pBmpSubtrahend(pBmpSubtrahend)
{}

PLFilterSubtract::~PLFilterSubtract()
{}


void PLFilterSubtract::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{
  // Calculate the size of the new bitmap
  pBmpDest->Create (pBmpSource->GetWidth(), pBmpSource->GetHeight(), pBmpSource->GetPixelFormat(),
                    NULL, 0, pBmpSource->GetResolution());
  for (int y = 0; y < pBmpSource->GetHeight(); ++y)
  {
    for (int x = 0; x < pBmpSource->GetWidth(); ++x)
    {
      PLPixel32 srcPixel = pBmpSource->GetPixel32(x,y);
      PLPixel32 differencePixel;

      if (x < m_pBmpSubtrahend->GetWidth() &&
          y < m_pBmpSubtrahend->GetHeight())
      {
        PLPixel32 substrahendPixel = m_pBmpSubtrahend->GetPixel32(x,y);
        differencePixel.SetR(abs(srcPixel.GetR() - substrahendPixel.GetR()));
        differencePixel.SetG(abs(srcPixel.GetG() - substrahendPixel.GetG()));
        differencePixel.SetB(abs(srcPixel.GetB() - substrahendPixel.GetB()));
        differencePixel.SetA(abs(srcPixel.GetA() - substrahendPixel.GetA()));
      } else {
        differencePixel = srcPixel;
      }
      pBmpDest->SetPixel(x,y,differencePixel);
    }
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltersubtract.cpp,v $
|      Revision 1.3  2004/10/05 13:53:43  uzadow
|      paintlib linux rgb oder change.
|
|      Revision 1.2  2004/09/16 16:48:03  janbo
|      -  adjustments for new PaintLib interface (PLPixelFormat)
|      - Mouse Wheel Event
|      - ImageViwer now pans and zooms
|      - Texture leak fix in TextureManager
|
|      Revision 1.1  2004/08/03 14:30:09  martin
|      better image compare algorithem
|
|
\--------------------------------------------------------------------
*/
