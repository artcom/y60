/*
/--------------------------------------------------------------------
|
|      $Id: plfiltercrop.cpp,v 1.13 2004/10/05 13:54:28 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfiltercrop.h"
#include "plbitmap.h"
#include "plrect.h"


PLFilterCrop::PLFilterCrop(int XMin, int YMin, int XMax, int YMax)
  : PLFilter(),
    m_XMin(XMin),
    m_XMax(XMax),
    m_YMin(YMin),
    m_YMax(YMax)
{
}

PLFilterCrop::PLFilterCrop(const PLRect & rc)
{
  m_XMin = rc.tl.x;
  m_XMax = rc.br.x;
  m_YMin = rc.tl.y;
  m_YMax = rc.br.y;
}

PLFilterCrop::~PLFilterCrop()
{

}

void PLFilterCrop::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{
  PLASSERT (m_XMin >= 0);
  PLASSERT (m_XMax <= pBmpSource->GetWidth());
  PLASSERT (m_YMin >= 0);
  PLASSERT (m_YMax <= pBmpSource->GetHeight());
  PLASSERT (m_XMin < m_XMax);
  PLASSERT (m_YMin < m_YMax);

  PLASSERT (pBmpSource->GetBitsPerPixel() >= 8);

  pBmpDest->Create (m_XMax-m_XMin, m_YMax-m_YMin,
                    pBmpSource->GetPixelFormat(),
                    NULL, 0, pBmpSource->GetResolution());
  PLBYTE ** pSrcLineArray = pBmpSource->GetLineArray();
  PLBYTE ** pDstLineArray = pBmpDest->GetLineArray();

  int y;
  for (y = m_YMin; y < m_YMax; y++)
  {
    PLBYTE * pSrcLine = pSrcLineArray[y];
    PLBYTE * pDstLine = pDstLineArray[y-m_YMin];
    int BytesPerPixel = pBmpSource->GetBitsPerPixel()/8;
    memcpy (pDstLine, pSrcLine + m_XMin*BytesPerPixel, (m_XMax-m_XMin)*BytesPerPixel);
  }
  if (pBmpSource->GetBitsPerPixel() == 8)
  {
    pBmpDest->SetPalette(pBmpSource->GetPalette());
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltercrop.cpp,v $
|      Revision 1.13  2004/10/05 13:54:28  artcom
|      *** empty log message ***
|
|      Revision 1.12  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.11  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.10  2004/09/09 16:52:45  artcom
|      refactored PixelFormat
|
|      Revision 1.9  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.8  2003/11/05 15:17:26  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.7  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.6  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.5  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.4  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.3  2001/09/30 19:55:37  uzadow
|      Fixed bug for 8 bpp in PLFilterCrop.
|
|      Revision 1.2  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
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
|      Revision 1.5  2000/09/26 10:47:26  Administrator
|      no message
|
|      Revision 1.4  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  1999/12/08 16:31:40  Ulrich von Zadow
|      Unix compatibility
|
|      Revision 1.2  1999/10/21 18:48:03  Ulrich von Zadow
|      no message
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
