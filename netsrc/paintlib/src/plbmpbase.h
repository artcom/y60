/*
/--------------------------------------------------------------------
|
|      $Id: plbmpbase.h,v 1.8 2004/10/05 13:54:30 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLBMPBASE
#define INCL_PLBMPBASE

#include "plbmpinfo.h"
#include "plpoint.h"
#include "pldebug.h"
#include "plpixel32.h"
#include "plpixel24.h"
#include "plpixel16.h"
#include "plpixel8.h"

class PLFilter;

//! Base class for PLBmp and PLSubBmp. PLBmpBase supports all operations
//! on a bitmap that PLBmp supports except those that depend on being able
//! to change the size or bpp.
class PLBmpBase : public PLBmpInfo
{

public:

  //! Empty constructor. 
  PLBmpBase
    ();

  //! Empty destructor.
  virtual ~PLBmpBase
    ();

  //! Test for equality. This function actually tests every pixel, so
  //! it's not fast. It's meant mainly for use in asserts and such.
  bool const operator ==
    ( PLBmpBase const &Other
    );

#ifdef _DEBUG
  virtual void AssertValid
    () const;    // Tests internal object state
#endif

  // PLBmpBase manipulation

  //! BitBlts SrcBmp on the current bitmap. Color depth conversion is 
  //! performed as nessesary.
  void CopyPixels
    ( const PLBmpBase& SrcBmp
    );

  //! Fills the color table with a grayscale palette. This function
  //! is only usable for bitmaps containing a color table. Index 0
  //! contains black (0) and the last index contains white (255). The
  //! alpha channel is set to opaque (255) for every palette entry.
  void SetGrayPalette
    ();

  //! Sets the color table to pPal. The contents of pPal are copied.
  void SetPalette
    ( PLPixel32 * pPal
    );

  //! Sets one entry in the color table. The function may only be
  //! called if there is a color table stored with the bitmap. The
  //! color table entry is set to the red, green, blue, and alpha
  //! values specified.
  void SetPaletteEntry
    ( PLBYTE Entry,
      PLBYTE r,
      PLBYTE g,
      PLBYTE b,
      PLBYTE a
    );

  //! Sets one entry in the color table. The function may only be
  //! called if there is a color table stored with the bitmap. The
  //! color table entry is set to the red, green, blue, and alpha
  //! values specified.
  void SetPaletteEntry
    ( PLBYTE Entry,
      PLPixel32 Value
    );

  //! Sets whether a bitmap stores an alpha channel. Works for 8 and 32 bpp 
  //! bitmaps. In either case, if b is true and the bitmap did not have an 
  //! alpha channel before the call, the complete alpha channel is set to
  //! opaque by the call.
  void SetHasAlpha
    (bool b
    );

  //! Replaces the alpha channel of the bitmap with a new one. This
  //! only works for bitmaps with 32 bpp. pAlphaBmp must point to an
  //! 8 bpp bitmap with the same dimensions as the object. The alpha
  //! channel information is physically copied into the bitmap.
  void SetAlphaChannel
    ( PLBmpBase * pAlphaBmp
    );

  //! Sets a pixel. PixelC needs to correspond to the bitmap's pixel
  //! format.
  template<class PixelC>
  void SetPixel (int x, int y, PixelC pixel)
  {
      PLASSERT (GetBitsPerPixel() == sizeof(PixelC)*8);
      *((PixelC*)(GetLineArray()[y])+x) = pixel;
  }

  //! Function to get a 32 bpp pixel.
  PLPixel32 GetPixel32
    ( int x,
      int y
    ) const;

  //! Function to get a 24 bpp pixel.
  PLPixel24 GetPixel24
      ( int x,
        int y
      ) const;
  
  //! Function to get a 16 bpp pixel.
  PLPixel16 GetPixel16
      ( int x,
        int y
      ) const;
  
  //! Find the nearest color to cr in the palette used by this bitmap
  //! Only works for 8 bpp bitmaps.
  PLBYTE FindNearestColor
    ( PLPixel32 cr
    );

  // PLBmpBase information.

  //! Returns number of colors that can be stored by a bitmap with this
  //! color depth.
  int GetNumColors
    ();

  // PLBmpBase direct manipulation

  //! Returns the address of the color table of the bitmap or NULL if
  //! no color table exists. The color table is stored as an array of
  //! consecutive PLPixel32 objects.
  PLPixel32 * GetPalette
    () const;

  //! Returns pointer to an array containing the starting addresses of
  //! the bitmap lines. This array should be used whenever the bitmap
  //! bits need to be manipulated directly.
  PLBYTE ** GetLineArray
    () const;

  //! Returns pointer to an array containing the starting addresses of
  //! the bitmap lines. This array should be used whenever the bitmap
  //! bits need to be manipulated directly.
  PLPixel32 ** GetLineArray32
    () const;

  //! Returns pointer to an array containing the starting addresses of
  //! the bitmap lines. This array should be used whenever the bitmap
  //! bits need to be manipulated directly.
  PLPixel24 ** GetLineArray24
    () const;

  //! Returns pointer to an array containing the starting addresses of
  //! the bitmap lines. This array should be used whenever the bitmap
  //! bits need to be manipulated directly.
  PLPixel16 ** GetLineArray16
    () const;

  //! Returns true if Bmp and this are almost equal. The comparison is done by 
  //! comparing the pixels in the bitmaps component-wise. If all components are 
  //! closer than epsilon, the bitmaps are considered almost equal.
  bool AlmostEqual
    ( const PLBmpBase& Bmp,
      int epsilon
    ) const;

  //! Sets the bitmap resolution in pixels per inch.
  void SetResolution (const PLPoint& Resolution);

  //! Sets quality of conversion to 8 bpp by CreateCopy(). Valid parameters are
  //! defined in FilterQuantize.h.
  void SetQuantizationMode
    ( int DitherType,
      int DitherPaletteType
    );

  void Dump();
  
protected:
  //! Can be called from internalCreate() to initialize object state.
  virtual void initLocals
    ( PLLONG Width,
      PLLONG Height,
      const PLPixelFormat & pf
    );

  void create8BPPCopy
    ( const PLBmpBase & rSrcBmp
    );

  void create1BPPCopy
    ( const PLBmpBase & rSrcBmp
    );

  // Member variables
  PLPixel32 * m_pClrTab;      // Pointer to the color table.
  PLBYTE    ** m_pLineArray;  // Table of the starting addresses of
                              // the lines.

  int m_DitherType;
  int m_DitherPaletteType;
};

inline void PLBmpBase::SetPaletteEntry
    ( PLBYTE Entry,
      PLBYTE r,
      PLBYTE g,
      PLBYTE b,
      PLBYTE a
    )
{
  m_pClrTab[Entry].Set (r, g, b, a);
}

inline void PLBmpBase::SetPaletteEntry
    ( PLBYTE Entry,
      PLPixel32 Value
    )
{
  m_pClrTab[Entry] = Value;
}

inline PLPixel32 PLBmpBase::GetPixel32 (int x, int y) const
{
    PLASSERT (GetBitsPerPixel() == 32);
    return *(GetLineArray32()[y]+x);
}

inline PLPixel24 PLBmpBase::GetPixel24 (int x, int y) const
{
    PLASSERT (GetBitsPerPixel() == 24);
    return *(GetLineArray24()[y]+x);
}

inline PLPixel16 PLBmpBase::GetPixel16 (int x, int y) const
{
    PLASSERT (GetBitsPerPixel() == 16);
    return *(GetLineArray16()[y]+x);
}


inline void PLBmpBase::SetResolution(const PLPoint& Resolution)
{
  PLASSERT_VALID(this);
  m_Resolution = Resolution;
}

inline int PLBmpBase::GetNumColors
    ()
{
  PLASSERT_VALID (this);
  return m_pf.GetNumColors();
}


// PLBmpBase direct manipulation

inline PLBYTE ** PLBmpBase::GetLineArray
    () const
{
  PLASSERT (m_pLineArray);
  return m_pLineArray;
}

inline PLPixel32 ** PLBmpBase::GetLineArray32
    () const
{
  PLASSERT (m_pLineArray);
  PLASSERT (GetBitsPerPixel() == 32);
  return (PLPixel32**)m_pLineArray;
}

inline PLPixel24 ** PLBmpBase::GetLineArray24
    () const
{
  PLASSERT (m_pLineArray);
  PLASSERT (GetBitsPerPixel() == 24);
  return (PLPixel24**)m_pLineArray;
}

inline PLPixel16 ** PLBmpBase::GetLineArray16
    () const
{
  PLASSERT (m_pLineArray);
  PLASSERT (GetBitsPerPixel() == 16);
  return (PLPixel16**)m_pLineArray;
}

inline PLPixel32 * PLBmpBase::GetPalette
    () const
    // Returns adress of the color table of the bitmap or NULL if no
    // color table exists.
{
  PLASSERT_VALID (this);

  return m_pClrTab;
}

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plbmpbase.h,v $
|      Revision 1.8  2004/10/05 13:54:30  artcom
|      *** empty log message ***
|
|      Revision 1.7  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.6  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.5  2004/06/20 16:59:34  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.4  2004/06/19 17:34:27  uzadow
|      Documentation update
|
|      Revision 1.3  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.2  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.1  2004/06/15 10:46:41  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|
\--------------------------------------------------------------------
*/
