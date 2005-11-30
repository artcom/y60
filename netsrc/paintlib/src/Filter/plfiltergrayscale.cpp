/*
/--------------------------------------------------------------------
|
|      $Id: plfiltergrayscale.cpp,v 1.11 2004/09/15 14:51:52 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfiltergrayscale.h"
#include "plbitmap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PLFilterGrayscale::PLFilterGrayscale() : PLFilter()
{
}

PLFilterGrayscale::~PLFilterGrayscale()
{

}

void PLFilterGrayscale::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{
  if (pBmpSource->IsGreyscale()) {
    *pBmpDest  = *pBmpSource;
    return;
  }
  PLASSERT (pBmpSource->GetBitsPerPixel() == 32 || 
            pBmpSource->GetBitsPerPixel() == 24);

  pBmpDest->Create (pBmpSource->GetWidth(), pBmpSource->GetHeight(),
                    PLPixelFormat::L8, NULL, 0, 
                    pBmpSource->GetResolution());
  PLBYTE ** pSrcLines = pBmpSource->GetLineArray();
  PLBYTE ** pDstLines = pBmpDest->GetLineArray();

  for (int y = 0; y<pBmpDest->GetHeight(); ++y)
  { // For each line
    PLBYTE * pSrcPixel = pSrcLines[y];
    PLBYTE * pDstPixel = pDstLines[y];

    for (int x = 0; x < pBmpDest->GetWidth(); ++x)
    { // For each pixel
      // For the coefficients used, see http://www.inforamp.net/~poynton/
      // We could approximate this for more speed by using
      // Y = (54 * R + 183 * G + 19 * B)/256 like libpng does.
      *pDstPixel = PLBYTE (pSrcPixel[PL_RGBA_RED]*0.212671+
                         pSrcPixel[PL_RGBA_GREEN]*0.715160+
                         pSrcPixel[PL_RGBA_BLUE]*0.072169);
      if (pBmpSource->GetBitsPerPixel() == 32)
      {
        pSrcPixel += sizeof(PLPixel32);
      }
      else
      {
        pSrcPixel += sizeof(PLPixel24);
      }
      ++pDstPixel;
    }
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltergrayscale.cpp,v $
|      Revision 1.11  2004/09/15 14:51:52  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.10  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.9  2004/09/09 16:52:45  artcom
|      refactored PixelFormat
|
|      Revision 1.8  2004/08/04 14:53:36  uzadow
|      Added PLFilterColorize.
|
|      Revision 1.7  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.6  2003/11/05 15:17:26  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.5  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.7  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.6  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.5  2000/12/18 22:42:53  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.4  2000/09/26 10:47:41  Administrator
|      Added comment
|
|      Revision 1.3  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.2  1999/12/08 16:31:40  Ulrich von Zadow
|      Unix compatibility
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
