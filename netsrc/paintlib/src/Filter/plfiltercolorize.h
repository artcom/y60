/*
/--------------------------------------------------------------------
|
|      $Id: plfiltercolorize.h,v 1.3 2004/09/12 19:23:31 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLFILTERCOLORIZE)
#define INCL_PLFILTERCOLORIZE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plinplacefilter.h"

class PLBmp;

//! Filter that colorizes a bitmap given a hue and saturation. Corresponds
//! loosely to the photoshop hue/saturation control when set to 'colorize'.
//! The range of hue is 0..359, the range of saturation is 0..100.
class PLFilterColorize : public PLInPlaceFilter
{
public:
  PLFilterColorize(double Hue, double Saturation);
  virtual ~PLFilterColorize();
  virtual void ApplyInPlace(PLBmpBase * pBmp) const;

private:
  double m_Hue;
  double m_Saturation;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltercolorize.h,v $
|      Revision 1.3  2004/09/12 19:23:31  uzadow
|      Smaller testpics.
|
|      Revision 1.2  2004/08/04 16:09:19  uzadow
|      *** empty log message ***
|
|      Revision 1.1  2004/08/04 14:53:36  uzadow
|      Added PLFilterColorize.
|
|      Revision 1.3  2004/06/20 16:59:38  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.2  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.1  2003/07/27 18:08:38  uzadow
|      Added plfilterfliprgb
|
|
|
\--------------------------------------------------------------------
*/
