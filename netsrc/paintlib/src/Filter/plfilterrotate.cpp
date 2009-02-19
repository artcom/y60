/*
/--------------------------------------------------------------------
|
|      $Id: plfilterrotate.cpp,v 1.10 2004/10/02 22:23:13 uzadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include <math.h>

#include "plbitmap.h"
#include "plfilterrotate.h"
#include "plpoint.h"
#include "plexcept.h"


//////////////////////////////////////////////////////////////////////
// PixelType based Template rotation beef
//////////////////////////////////////////////////////////////////////
template<class PIXELTYPE>
void getPixel(PLBmpBase * pBmpSource, int theX, int theY, PIXELTYPE& theResult);
template<>
void getPixel(PLBmpBase * pBmpSource, int theX, int theY, PLPixel32& theResult) { theResult = pBmpSource->GetPixel32(theX,theY);}
template<>
void getPixel(PLBmpBase * pBmpSource, int theX, int theY, PLPixel24& theResult) { theResult = pBmpSource->GetPixel24(theX,theY);}
template<>
void getPixel(PLBmpBase * pBmpSource, int theX, int theY, PLPixel16& theResult) { theResult = pBmpSource->GetPixel16(theX,theY);}

template <class PIXELTYPE>
void
rotate(PLBmpBase * pBmpSource, PLBmp * pBmpDest, const PLFilterRotate::AngleType & theAngleType) {
  int oldWidth = pBmpSource->GetWidth();
  int oldHeight= pBmpSource->GetHeight();
  int newWidth = theAngleType == PLFilterRotate::oneeighty ? oldWidth : oldHeight;
  int newHeight = theAngleType == PLFilterRotate::oneeighty ? oldHeight : oldWidth;

  // Calculate the size of the new bitmap
  pBmpDest->Create (newWidth, newHeight, pBmpSource->GetPixelFormat(), 
                    NULL, 0, pBmpSource->GetResolution());
  PIXELTYPE pix;
  for (int newY = 0; newY < newHeight; ++newY)
  {
    for (int newX = 0; newX < newWidth; ++newX)
    {
      int oldX = 0;
      int oldY = 0;
      switch(theAngleType)
      {
        case PLFilterRotate::ninety:
          oldX = newY;
          oldY = oldHeight - newX - 1;
          break;
        case PLFilterRotate::oneeighty:
          oldX = oldWidth - newX - 1;
          oldY = oldHeight - newY - 1;
          break;
        case PLFilterRotate::twoseventy:
          oldY = newX;
          oldX = oldWidth - newY - 1;
          break;
      }
      getPixel<PIXELTYPE>(pBmpSource, oldX,oldY, pix);
      pBmpDest->SetPixel(newX,newY,pix);
    }
  }
    
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
PLFilterRotate::PLFilterRotate(AngleType a)
    : PLFilter(),
      m_Angle(a)
{}


PLFilterRotate::~PLFilterRotate()
{}


void PLFilterRotate::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{
    switch (pBmpSource->GetBitsPerPixel()) {
        case 32:
            rotate<PLPixel32>(pBmpSource, pBmpDest, m_Angle);
            break;
        case 24:
            rotate<PLPixel24>(pBmpSource, pBmpDest, m_Angle);
            break;
        case 16:
            rotate<PLPixel16>(pBmpSource, pBmpDest, m_Angle);
            break;
        case 8:
            throw (PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "Unsupported."));
            //rotate<PLPixel8>(pBmpSource, pBmpDest, m_Angle);
            break;
        default:
            throw (PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "Unsupported."));
    };
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterrotate.cpp,v $
|      Revision 1.10  2004/10/02 22:23:13  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
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
|      Revision 1.5  2003/02/27 23:01:33  uzadow
|      Linux version of PLFilterRotate, nicer test output
|
|      Revision 1.4  2003/02/27 22:46:56  uzadow
|      Added plfilterrotate.
|
|      Revision 1.1  2002/02/05 09:40:54  david
|      pavel added rotate90 filter
|
\--------------------------------------------------------------------
*/
