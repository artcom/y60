/*
/--------------------------------------------------------------------
|
|      $Id: plfilterflip.cpp,v 1.10 2004/10/03 17:33:10 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|      Original code by Richard Hollis
|
\--------------------------------------------------------------------
*/

#include "plbitmap.h"
#include "plfilterflip.h"
#include "plpoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PLFilterFlip::PLFilterFlip() : PLFilter()
{}


PLFilterFlip::~PLFilterFlip()
{}


void PLFilterFlip::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{
  int nWidth = pBmpSource->GetWidth();
  int nHeight= pBmpSource->GetHeight();

  pBmpDest->Create (nWidth, nHeight, pBmpSource->GetPixelFormat(),
                    NULL, 0, pBmpSource->GetResolution());

  PLBYTE** ppSrcLines = pBmpSource->GetLineArray();
  PLBYTE** ppDestLines = pBmpDest->GetLineArray();

  for (int y = 0; y<nHeight; y++)
  {
    memcpy(ppDestLines[nHeight-y-1], ppSrcLines[y], pBmpDest->GetBytesPerLine());
  }

  PLPixel32 * pPalette = pBmpSource->GetPalette();
  if ((pBmpSource->GetBitsPerPixel()<=8) && (pPalette))
    pBmpDest->SetPalette(pPalette);
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterflip.cpp,v $
|      Revision 1.10  2004/10/03 17:33:10  uzadow
|      Minor build issues
|
|      Revision 1.9  2004/09/30 12:26:22  artcom
|       - made flip and mirror 24 bit aware
|
|      Revision 1.8  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.7  2004/09/09 16:52:45  artcom
|      refactored PixelFormat
|
|      Revision 1.6  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.5  2003/11/05 15:17:26  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/02/24 13:00:46  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
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
