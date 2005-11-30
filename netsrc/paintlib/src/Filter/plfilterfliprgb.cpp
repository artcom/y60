/*
/--------------------------------------------------------------------
|
|      $Id: plfilterfliprgb.cpp,v 1.6 2004/10/02 22:23:12 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfilterfliprgb.h"
#include "plbitmap.h"


PLFilterFlipRGB::PLFilterFlipRGB()
  : PLInPlaceFilter()
{
}

PLFilterFlipRGB::~PLFilterFlipRGB()
{

}

void PLFilterFlipRGB::ApplyInPlace(PLBmpBase * pBmp) const
{
  // Only 24 and 32 bpp supported.
  PLASSERT (pBmp->GetBitsPerPixel() == 24 ||
            pBmp->GetBitsPerPixel() == 32);
  pBmp->SwapPixelFormatRGB();

  PLBYTE ** pLineArray = pBmp->GetLineArray();
  for (int y = 0; y < pBmp->GetHeight(); y++)
  {
    PLBYTE * pLine = pLineArray[y];
    if (pBmp->GetBitsPerPixel() == 24) 
    {
      for (int x = 0; x < pBmp->GetWidth(); x++) 
      { 
        PLBYTE tmp = pLine[x*3+PL_RGBA_RED];
        pLine[x*3+PL_RGBA_RED] = pLine[x*3+PL_RGBA_BLUE];
        pLine[x*3+PL_RGBA_BLUE] = tmp;
      }
    } 
    else
    {
      for (int x = 0; x < pBmp->GetWidth(); x++) 
      { 
        PLBYTE tmp = pLine[x*4+PL_RGBA_RED];
        pLine[x*4+PL_RGBA_RED] = pLine[x*4+PL_RGBA_BLUE];
        pLine[x*4+PL_RGBA_BLUE] = tmp;
      } 
    }
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterfliprgb.cpp,v $
|      Revision 1.6  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.5  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.4  2004/06/20 16:59:38  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.3  2004/06/14 12:51:35  artcom
|      Performance improvement
|
|      Revision 1.2  2003/07/29 21:27:41  uzadow
|      Fixed PLDirectFBBmp::GetBytesPerLine(), im2* Makefiles
|
|      Revision 1.1  2003/07/27 18:08:38  uzadow
|      Added plfilterfliprgb
|
|
\--------------------------------------------------------------------
*/
