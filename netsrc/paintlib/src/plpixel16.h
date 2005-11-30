/*
/--------------------------------------------------------------------
|
|      $Id: plpixel16.h,v 1.5 2004/10/06 12:54:00 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPIXEL16
#define INCL_PLPIXEL16

#include "plpixeldefs.h"
#include "plpaintlibdefs.h"

#include <stdlib.h>
#include <math.h>

//! 16 bit pixel class. A pixel in this class contains 5 bits of
//! red, 6 of green and 5 of blue (in that order). The order of the 
//! color components is
//! OS-dependent and defined in plpixeldefs.h. This class is meant to be
//! fast, so all methods are inlined.
class PLPixel16
{
  public:
    //!
    PLPixel16 ();
    //!
    PLPixel16 (PLBYTE r, PLBYTE g, PLBYTE b);
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
    PLPixel16 operator = (const PLPixel32& Pix);

    //!
    operator PLPixel32 () const;

    //!
    PLPixel16 operator = (const PLPixel24& Pix);

    //!
    operator PLPixel24 () const;

    //!
    bool operator ==(const PLPixel16 Pix) const;

    //!
    bool operator !=(const PLPixel16 Pix) const;

    //! Simple and fast 'distance' between two pixels. Just adds the
    //! distances between the color components and treats colors
    //! equally.
    int BoxDist (const PLPixel16 Pix) const;

  private:
    PLWORD m_Data;
};

inline PLPixel16::PLPixel16()
{
}


inline PLPixel16::PLPixel16(PLBYTE r, PLBYTE g, PLBYTE b)
{
  Set (r, g, b);
}


inline void PLPixel16::Set (PLBYTE r, PLBYTE g, PLBYTE b)
{
#ifdef PL_PIXEL_BGRA_ORDER
  m_Data = ((r&0xF8) << 8) | ((g&0xFC) << 3) | (b>>3);
#else
  m_Data = (b&0xF8) << 8 | ((g&0xFC) << 3) | (r>>3);
#endif  
}

inline void PLPixel16::SetR(PLBYTE r)
{
#ifdef PL_PIXEL_BGRA_ORDER
  m_Data = (m_Data&0x07FF)|((r&0xF8)<<8);
#else
  m_Data = (m_Data&0xFFE0)|(r>>3);
#endif
}

inline void PLPixel16::SetG(PLBYTE g)
{
  m_Data = (m_Data&0xF81F)|((g&0xFC)<<3);
}


inline void PLPixel16::SetB(PLBYTE b)
{
#ifdef PL_PIXEL_BGRA_ORDER
  m_Data = (m_Data&0xFFE0)|(b>>3);
#else
  m_Data = (m_Data&0x07FF)|((b&0xF8)<<8);
#endif
}

inline PLBYTE PLPixel16::GetR() const
{
#ifdef PL_PIXEL_BGRA_ORDER
  return (m_Data&0xF800)>>8;
#else
  return (m_Data&0x001F)<<3;
#endif  
}


inline PLBYTE PLPixel16::GetG() const
{
  return (m_Data&0x07E0)>>3;
}

inline PLBYTE PLPixel16::GetB() const
{
#ifdef PL_PIXEL_BGRA_ORDER
  return (m_Data&0x001F)<<3;
#else
  return (m_Data&0xF800)>>8;
#endif  
}

inline PLPixel16 PLPixel16::operator = (const PLPixel32& Pix)
{
  Set (Pix.GetR(), Pix.GetG(), Pix.GetB());
  return *this;
}

inline PLPixel16::operator PLPixel32 () const
{
  // TODO: Make faster.
  return PLPixel32 (GetR(), GetG(), GetB(), 255);
}

inline PLPixel16 PLPixel16::operator = (const PLPixel24& Pix)
{
  Set (Pix.GetR(), Pix.GetG(), Pix.GetB());

  return *this;
}

inline PLPixel16::operator PLPixel24 () const
{
  // TODO: Make faster.  
  return PLPixel24 (GetR(), GetG(), GetB());
}

inline int PLPixel16::BoxDist (const PLPixel16 Pix) const
{
  return (abs ((int)GetR()-Pix.GetR()) +
          abs ((int)GetG()-Pix.GetG()) +
          abs ((int)GetB()-Pix.GetB()));
}

inline bool PLPixel16::operator ==(const PLPixel16 Pix) const
{
  return (*(const PLWORD *)this == *(const PLWORD*)&Pix);
}

inline bool PLPixel16::operator !=(const PLPixel16 Pix) const
{
  return (!(*this == Pix));
}


#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plpixel16.h,v $
|      Revision 1.5  2004/10/06 12:54:00  artcom
|      Added missing includes.
|
|      Revision 1.4  2004/09/29 21:59:33  uzadow
|      Mac OS X fixes.
|
|      Revision 1.3  2004/06/13 20:20:33  uzadow
|      no message
|
|      Revision 1.2  2004/06/09 21:34:53  uzadow
|      Added 16 bpp support to plbitmap, planybmp and pldirectfbbmp
|
|      Revision 1.1  2004/06/09 20:27:48  uzadow
|      Added 16 bpp pixel class.
|
|
\--------------------------------------------------------------------
*/
