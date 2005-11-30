/*
/--------------------------------------------------------------------
|
|      $Id: plbitmap.h,v 1.22 2004/09/11 10:30:40 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLBITMAP
#define INCL_PLBITMAP

#include "plbmpbase.h"
#include "plpoint.h"
#include "pldebug.h"
#include "plpixel32.h"
#include "plpixel24.h"
#include "plpixel16.h"

class PLFilter;

//! Device- and OS-independent bitmap class. Manipulates uncompressed
//! bitmaps of all color depths.
//!
//! This class is an abstract base class. It exists to define a
//! format-independent interface for bitmap manipulation and to
//! provide common routines. Derived classes must support at least
//! the color depths 1, 8 and 32 bpp. PLBmp defines a public interface
//! for general use and a protected interface for use by derived
//! classes.
//!
//! For 32 bpp, alpha channel information is stored in one byte
//! (PL_RGBA_ALPHA) of each 4-byte pixel. To allow for optimizations
//! when no alpha channel is present, a flag is set whenever the
//! alpha information is valid. The complete alpha channel of a
//! bitmap can be replaced by a different one by calling
//! SetAlphaChannel(). A 0 in an alpha channel entry is completely
//! transparent; a 255 is completely opaque.
class PLBmp : public PLBmpBase
{

public:

  //! Empty constructor. Constructors in derived classes create a
  //! small empty bitmap to ensure that the object is always in a
  //! sane state.
  PLBmp
    ();

  //! Empty destructor.
  virtual ~PLBmp
    ();

  //! Assignment operator. Note that assignment between different derived
  //! classes is possible and results in a format conversion.
  PLBmp &operator=
    ( PLBmpBase const &Orig
    );

  PLBmp &operator=
    ( PLBmp const &Orig
    );

  // PLBmp creation

  //! Creates a new empty bitmap. Memory for the bits is allocated
  //! but not initialized. Previous contents of the bitmap object are
  //! discarded. If bAlphaChannel is true, the bitmap is assumed to
  //! contain a valid alpha channel. If pBits is non-null, it contains
  //! pixel data in the format expected by the bitmap and Stride is the
  //! distance in bytes from one source line to the next. In this case, 
  //! the data in pBits is copied into the internal buffer of the PLBmp.
  virtual void Create
    ( PLLONG Width,
      PLLONG Height,
	  const PLPixelFormat& pf,
      PLBYTE * pBits = 0,
      int Stride = 0, 
      const PLPoint& Resolution = PLPoint (0,0)
    );

  //! Creates a new empty bitmap. Info contains the metadata (width, height, 
  //! etc.) to be used in creation.
  virtual void Create
    ( const PLBmpInfo& Info 
    );

  //! Creates a copy of rSrcBmp, converting color depth if nessesary.
  //! Supports 1, 8, 16, 24 and 32 BPP. Alpha channel information is preserved if
  //! supported by the target bit depth. Conversion to 8 bpp is possible only
  //! from 1 and 32 bpp.
  void CreateCopy
    ( const PLBmpBase & rSrcBmp,
      const PLPixelFormat& pfWanted = PLPixelFormat::DONTCARE
    );

  //! Creates a copy of rSrcBmp, applying rFilter on the way. Depending
  //! on the filter called, this is often much faster than CreateCopy()
  //! followed by ApplyFilter().
  void CreateFilteredCopy (PLBmpBase & rSrcBmp, const PLFilter & rFilter);

  // PLBmp information
  
  //! Returns memory used by a bitmap
  virtual long GetMemUsed
    () = 0;

  //! Returns number of bytes used per line. Note that this is not the 
  //! stride of the bitmap.
  virtual long GetBytesPerLine
    () = 0;

  // PLBmp manipulation

  //! Applies a filter to the bitmap.
  void ApplyFilter
    ( const PLFilter& Filter
    );


protected:
  //! Can be called from internalCreate() to initialize object state.
  virtual void initLocals
    ( PLLONG Width,
      PLLONG Height,
      const PLPixelFormat& pf
    );

  //! Create a new bitmap with uninitialized bits. (Assume no memory
  //! is allocated yet.)
  virtual void internalCreate
    ( PLLONG Width,
      PLLONG Height,
      const PLPixelFormat& pf
    ) = 0;

  //! Initialize internal table of line addresses.
  virtual void initLineArray
    () = 0;

  //! Delete memory allocated by member variables.
  virtual void freeMembers
    () = 0;

  //! Creates a new PLBmp as copy of rSrcBmp. Assumes there is no memory
  //! allocated yet.
  void internalCopy
    ( const PLBmpBase & rSrcBmp
    );

};

inline PLBmp & PLBmp::operator=
    ( PLBmpBase const &Orig
    )
{
  if (this != &Orig)
    CreateCopy(Orig);
  return *this;
}

inline PLBmp & PLBmp::operator=
    ( PLBmp const &Orig
    )
{
  if (this != &Orig)
    CreateCopy(Orig);
  return *this;
}

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plbitmap.h,v $
|      Revision 1.22  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.21  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.20  2004/06/20 16:59:34  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.19  2004/06/19 17:04:22  uzadow
|      Removed Lock(), Unlock(), PLDDrawBmp
|
|      Revision 1.18  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.17  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.16  2004/06/15 11:18:17  uzadow
|      First working version of PLBmpBase.
|
|      Revision 1.15  2004/06/15 10:26:05  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.14  2004/06/10 16:05:28  artcom
|      Doc update
|
|      Revision 1.13  2004/06/09 21:34:53  uzadow
|      Added 16 bpp support to plbitmap, planybmp and pldirectfbbmp
|
|      Revision 1.12  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.11  2003/11/05 15:17:23  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.10  2003/07/29 21:27:41  uzadow
|      Fixed PLDirectFBBmp::GetBytesPerLine(), im2* Makefiles
|
|      Revision 1.9  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.8  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.7  2002/03/31 13:36:41  uzadow
|      Updated copyright.
|
|      Revision 1.6  2001/10/21 17:12:39  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.5  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.4  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.3  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.2  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.26  2001/09/15 14:30:20  uzadow
|      Fixed PLPixel32 initialization bug.
|
|      Revision 1.25  2001/09/13 20:47:36  uzadow
|      Removed commented-out lines.
|
|      Revision 1.24  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.23  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.22  2000/11/21 20:18:03  uzadow
|      Added operator ==
|
|      Revision 1.21  2000/11/07 15:40:46  jmbuena
|      Changes related to paintlibdefs.h and pixeldefs.h
|
|      Revision 1.20  2000/11/02 21:28:47  uzadow
|      Fixed copy constructors.
|
|      Revision 1.19  2000/10/24 16:46:34  uzadow
|      Fixed build problems
|
|      Revision 1.18  2000/10/23 17:45:03  jmbuena
|      Linux compatibility changes
|
|      Revision 1.17  2000/09/26 14:28:47  Administrator
|      Added Threshold filter
|
|      Revision 1.16  2000/09/26 12:14:51  Administrator
|      Refactored quantization.
|
|      Revision 1.15  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.14  2000/07/11 17:11:00  Ulrich von Zadow
|      Added support for RGBA pixel ordering (Jose Miguel Buenaposada Biencinto).
|
|      Revision 1.13  2000/03/31 12:20:05  Ulrich von Zadow
|      Video invert filter (beta)
|
|      Revision 1.12  2000/03/31 11:53:30  Ulrich von Zadow
|      Added quantization support.
|
|      Revision 1.11  2000/01/16 20:43:12  anonymous
|      Removed MFC dependencies
|
|      Revision 1.10  1999/12/10 01:27:26  Ulrich von Zadow
|      Added assignment operator and copy constructor to
|      bitmap classes.
|
|      Revision 1.9  1999/12/09 16:35:22  Ulrich von Zadow
|      no message
|
|      Revision 1.8  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.7  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.6  1999/10/22 21:25:51  Ulrich von Zadow
|      Removed buggy octree quantization
|
\--------------------------------------------------------------------
*/
