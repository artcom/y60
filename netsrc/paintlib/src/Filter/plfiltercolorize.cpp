/*
/--------------------------------------------------------------------
|
|      $Id: plfiltercolorize.cpp,v 1.7 2004/09/21 13:02:50 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfiltercolorize.h"
#include "plfiltergrayscale.h"
#include "plbitmap.h"
#include "planybmp.h"

#include <math.h>

// From Foley, van Dam p. 593
/*
PLPixel24 hsv2rgb(double h, double s, double v) 
{
  if (s<0.001) 
  {
    return PLPixel24(PLBYTE(v),PLBYTE(v),PLBYTE(v));
  }
  else
  {
    s /= 100;
      
    double f;
    int i;
    if (h > 359.9) 
      h = 0.0;
    h /= 60;
    i = int(floor(h));
    f = h-i;
    int p = int(v*(1.0-s));
    int q = int(v*(1.0-(s*f)));
    int t = int(v*(1.0-(s*(1.0-f))));
    switch(i) {
      case 0: 
        return PLPixel24(PLBYTE(v), t, p);
      case 1:
        return PLPixel24(q, PLBYTE(v), p);
      case 2: 
        return PLPixel24(p, PLBYTE(v), t);
      case 3:
        return PLPixel24(p, q, PLBYTE(v));
      case 4: 
        return PLPixel24(t, p, PLBYTE(v));
      case 5:
        return PLPixel24(PLBYTE(v), p, q);
      default:
        PLASSERT(false);
        return PLPixel24(0,0,0);
    }
  }
}
*/

// From Foley, van Dam p. 596
PLBYTE hls_value(double n1, double n2, double hue)
{
  if (hue>360)
    hue-=360;
  if (hue<0)   
    hue+=360;
  
  double rv;
  if (hue<60)  
    rv = n1+(n2-n1)*hue/60.0;
  else if (hue<180) 
    rv = n2;
  else if (hue<240) 
    rv = n1+(n2-n1)*(240.0-hue)/60.0;
  else 
    rv = n1;

  return PLBYTE(rv*255);
}

PLPixel24 hls2rgb (double h, double l, double s)
{
  double m1, m2;
  l /= 255;
  s /= 100;
  // Warning: Foley, van Dam has a typo on the next line!
  m2 = (l<=0.5)?(l*(1.0+s)):(l+s-l*s);
  m1 = 2.0*l-m2;
  if (s<0.001)
    return PLPixel24(PLBYTE(l*255), PLBYTE(l*255), PLBYTE(l*255));
  else 
  {
    return PLPixel24(hls_value(m1,m2,h+120.0),
                     hls_value(m1,m2,h),
                     hls_value(m1,m2,h-120.0));
  }
}

PLFilterColorize::PLFilterColorize(double Hue, double Saturation)
  : PLInPlaceFilter(),
    m_Hue(Hue),
    m_Saturation(Saturation)
{
}

PLFilterColorize::~PLFilterColorize()
{

}

void PLFilterColorize::ApplyInPlace(PLBmpBase * pBmp) const
{
  PLAnyBmp TempBmp;
  TempBmp.CreateFilteredCopy(*pBmp, PLFilterGrayscale());
  
  PLPixel24 ColorTable[256];
  for (int i=0; i<256; i++) 
  {
    ColorTable[i] = hls2rgb(m_Hue, i, m_Saturation);
  }

  PLBYTE ** pSrcLines = TempBmp.GetLineArray();
  for (int y = 0; y < TempBmp.GetHeight(); y++)
  {
    PLBYTE * pSrcLine = pSrcLines[y];
    switch (pBmp->GetBitsPerPixel()) {
      case 32:
        {
          PLPixel32 * pDestLine = pBmp->GetLineArray32()[y];
          for (int x = 0; x < TempBmp.GetWidth(); x++) 
          { 
            pDestLine[x] = ColorTable[pSrcLine[x]];
          }
        }
        break;
      case 24:
        {
          PLPixel24 * pDestLine = pBmp->GetLineArray24()[y];
          for (int x = 0; x < TempBmp.GetWidth(); x++) 
          { 
            pDestLine[x] = ColorTable[pSrcLine[x]];
          }
        }
        break;
      default:
        // bpp other than 8 and 24 are not supported.
        PLASSERT(false);
    }
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltercolorize.cpp,v $
|      Revision 1.7  2004/09/21 13:02:50  uzadow
|      Reenabled testFlipRGB, added 32-bit version of PLFilterColorize.
|
|      Revision 1.6  2004/09/12 19:23:31  uzadow
|      Smaller testpics.
|
|      Revision 1.5  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.4  2004/09/09 16:52:45  artcom
|      refactored PixelFormat
|
|      Revision 1.3  2004/08/04 16:47:59  uzadow
|      Fixed bug in hls2rgb.
|
|      Revision 1.2  2004/08/04 16:09:19  uzadow
|      *** empty log message ***
|
|      Revision 1.1  2004/08/04 14:53:36  uzadow
|      Added PLFilterColorize.
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
