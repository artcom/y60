/*
/--------------------------------------------------------------------
|
|      $Id: plfilterthreshold.cpp,v 1.9 2004/09/11 12:41:36 uzadow Exp $
|
|      Copyright (c) 2000-2001 Iyad Hatem
|
\--------------------------------------------------------------------
*/

#include "plfilterthreshold.h"
#include "plbitmap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PLFilterThreshold::PLFilterThreshold(int threshold_min,int threshold_max, int channel)
: PLFilter(),
  m_threshold_min(threshold_min),
  m_threshold_max(threshold_max),
  m_channel(channel)
{
}

PLFilterThreshold::~PLFilterThreshold()
{

}

void PLFilterThreshold::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{

  int threshold_min = m_threshold_min;
  int threshold_max = m_threshold_max;
  int channel = m_channel;

  // Only works for 32 bpp bitmaps at the moment.
  PLASSERT (pBmpSource->GetBitsPerPixel() == 32);

  pBmpDest->Create (pBmpSource->GetWidth(), pBmpSource->GetHeight(), 
            PLPixelFormat::L8, NULL, 0, pBmpSource->GetResolution());
  PLBYTE ** pSrcLines = pBmpSource->GetLineArray();
  PLBYTE ** pDstLines = pBmpDest->GetLineArray();

  for (int y = 0; y<pBmpDest->GetHeight(); ++y)
  { // For each line
    PLBYTE * pSrcPixel = pSrcLines[y];
    PLBYTE * pDstPixel = pDstLines[y];

    for (int x = 0; x < pBmpDest->GetWidth(); ++x)
    { // For each pixel
      if ((PLBYTE (pSrcPixel[channel]) <= PLBYTE (threshold_min)) ||
          (PLBYTE (pSrcPixel[channel]) >= PLBYTE(threshold_max)))
        *pDstPixel = 0;
      else
        *pDstPixel = pSrcPixel[channel];
      pSrcPixel += sizeof(PLPixel32);
      ++pDstPixel;
    }
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterthreshold.cpp,v $
|      Revision 1.9  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.8  2004/09/09 16:52:45  artcom
|      refactored PixelFormat
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
|      Revision 1.4  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.3  2001/10/16 17:12:27  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.4  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.3  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.2  2000/12/18 22:42:53  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.1  2000/09/26 14:28:47  Administrator
|      Added Threshold filter
|
|
|
\--------------------------------------------------------------------
*/
