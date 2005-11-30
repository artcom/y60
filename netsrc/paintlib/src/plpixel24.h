/*
/--------------------------------------------------------------------
|
|      $Id: plpixel24.h,v 1.6 2004/10/06 12:54:00 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPIXEL24
#define INCL_PLPIXEL24

#include "plpixeldefs.h"
#include "plpaintlibdefs.h"
#include "plpixel32.h"

#include <stdlib.h>

//! 24 bit pixel class. A pixel in this class contains 8 bits each of
//! red, green and blue. The order of the color components is
//! OS-dependent and defined in config.h. This class is meant to be
//! fast, so all methods are inlined.
class PLPixel24
{
  public:
    //!
    PLPixel24 ();
    //!
    PLPixel24 (PLBYTE r, PLBYTE g, PLBYTE b);
    //!
    void Set (PLBYTE r, PLBYTE g, PLBYTE b);
    //!
    void SetR (PLBYTE r);
    //!
    void SetG (PLBYTE g);
    //!
    void SetB (PLBYTE b);
    //!
    PLBYTE GetR () const;
    //!
    PLBYTE GetG () const;
    //!
    PLBYTE GetB () const;

    //!
    PLPixel24 operator = (const PLPixel32& Pix);

    //!
    operator PLPixel32 () const;

    //!
    bool operator ==(const PLPixel24&) const;

    //!
    bool operator !=(const PLPixel24&) const;

    //! Simple and fast 'distance' between two pixels. Just adds the
    //! distances between the color components and treats colors
    //! equally.
    int BoxDist (const PLPixel24 Pix) const;

    //! Returns a weighed average between two pixels. Factor must be 
    //! between 0 and 256. Factor=256 means Pix1 is the result, Factor=0 
    //! means Pix2 is the result.
    static PLPixel24 Blend (int Factor, const PLPixel24 Pix1, 
                            const PLPixel24 Pix2);

  private:
    PLBYTE m_Data[3];
};

inline PLPixel24::PLPixel24()
{
}


inline PLPixel24::PLPixel24(PLBYTE r, PLBYTE g, PLBYTE b)
{
  Set (r, g, b);
}


inline void PLPixel24::Set(PLBYTE r, PLBYTE g, PLBYTE b)
{
  m_Data[PL_RGBA_RED] = r;
  m_Data[PL_RGBA_GREEN] = g;
  m_Data[PL_RGBA_BLUE] = b;
}


inline void PLPixel24::SetR(PLBYTE r)
{
  m_Data[PL_RGBA_RED] = r;
}


inline void PLPixel24::SetG(PLBYTE g)
{
  m_Data[PL_RGBA_GREEN] = g;
}


inline void PLPixel24::SetB(PLBYTE b)
{
  m_Data[PL_RGBA_BLUE] = b;
}


inline PLBYTE PLPixel24::GetR() const
{
  return m_Data[PL_RGBA_RED];
}


inline PLBYTE PLPixel24::GetG() const
{
  return m_Data[PL_RGBA_GREEN];
}


inline PLBYTE PLPixel24::GetB() const
{
  return m_Data[PL_RGBA_BLUE];
}


inline int PLPixel24::BoxDist (const PLPixel24 Pix) const
{
  return (abs ((int)GetR()-Pix.GetR()) +
          abs ((int)GetG()-Pix.GetG()) +
          abs ((int)GetB()-Pix.GetB()));
}


inline PLPixel24 PLPixel24::Blend (int Factor, const PLPixel24 Pix1, const PLPixel24 Pix2)
{
  PLASSERT (Factor >= 0 && Factor <= 256);

  return PLPixel24 ((Pix1.GetR()*Factor+Pix2.GetR()*(256-Factor))>>8,
                    (Pix1.GetG()*Factor+Pix2.GetG()*(256-Factor))>>8,
                    (Pix1.GetB()*Factor+Pix2.GetB()*(256-Factor))>>8);
}

inline PLPixel24 PLPixel24::operator = (const PLPixel32& Pix)
{
  SetR (Pix.GetR());
  SetG (Pix.GetG());
  SetB (Pix.GetB());

  return *this;
}

inline PLPixel24::operator PLPixel32 () const
{
  return PLPixel32 (GetR(), GetG(), GetB(), 255);
}

inline bool PLPixel24::operator ==(const PLPixel24& Pix) const
{
  return (GetR() == Pix.GetR() && GetG() == Pix.GetG() && GetB() == Pix.GetB());
}

inline bool PLPixel24::operator !=(const PLPixel24& Pix) const
{
  return (!(*this == Pix));
}


#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plpixel24.h,v $
|      Revision 1.6  2004/10/06 12:54:00  artcom
|      Added missing includes.
|
|      Revision 1.5  2004/06/09 21:34:53  uzadow
|      Added 16 bpp support to plbitmap, planybmp and pldirectfbbmp
|
|      Revision 1.4  2004/06/09 20:27:48  uzadow
|      Added 16 bpp pixel class.
|
|      Revision 1.3  2002/02/24 13:00:35  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/10/03 13:58:43  uzadow
|      Added.
|
|      Revision 1.1  2001/09/26 10:21:09  uzadow
|      Added PLPixel24, PLBmp::CreateCopy now supports 24 bpp in most cases.
|
|
|
\--------------------------------------------------------------------
*/
