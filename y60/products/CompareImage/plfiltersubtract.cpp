/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "plfiltersubtract.h"

#include <math.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/plpoint.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)


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
        differencePixel.SetR( static_cast<PLBYTE>(abs(srcPixel.GetR() - substrahendPixel.GetR())) );
        differencePixel.SetG( static_cast<PLBYTE>(abs(srcPixel.GetG() - substrahendPixel.GetG())) );
        differencePixel.SetB( static_cast<PLBYTE>(abs(srcPixel.GetB() - substrahendPixel.GetB())) );
        differencePixel.SetA( static_cast<PLBYTE>(abs(srcPixel.GetA() - substrahendPixel.GetA())) );
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
