/*
/--------------------------------------------------------------------
|
|      $Id: planybmp.h,v 1.10 2004/09/09 16:52:49 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLANYBMP
#define INCL_PLANYBMP

#ifndef INCL_PLBITMAP
#include "plbitmap.h"
#endif

//! Manipulates uncompressed device- and platform-independent bitmaps.
//! Supported data formats are 1, 8, 16, 24 and 32 bpp. The data is stored
//! sequentially without padding in the bitmap. The class implements exactly
//! the interface defined by PLBmp without additions.
class PLAnyBmp : public PLBmp
{

public:
  //! Creates an empty bitmap.
  PLAnyBmp
    ();

  //! Destroys the bitmap.
  virtual ~PLAnyBmp
    ();

  //! Copy constructor
  PLAnyBmp
    ( const PLBmpBase &Orig
    );

  //! Copy constructor
  PLAnyBmp
    ( const PLAnyBmp &Orig
    );

  //! Assignment operator.
  PLAnyBmp &operator=
    ( const PLBmpBase &Orig
    );

  //! Assignment operator.
  PLAnyBmp &operator=
    ( const PLAnyBmp &Orig
    );

  // PLAnyBmp information

  //! Returns the amount of memory used by the object.
  long GetMemUsed
    ();

  //! Returns number of bytes used per line.
  long GetBytesPerLine
    ();

  //! Returns a raw pointer to the bitmap bits.
  PLBYTE * GetPixels
    ()
  { return m_pBits; }

  //! Returns a raw pointer to the bitmap bits.
  const PLBYTE * GetPixels
    () const 
  { return m_pBits; }

  // Static functions

  //! Returns memory needed by a bitmap with the specified attributes.
  static long GetMemNeeded
    ( PLLONG width,
      PLLONG height,
      PLWORD BitsPerPixel
    );

  //! Returns memory needed by bitmap bits.
  static long GetBitsMemNeeded
    ( PLLONG width,
      PLLONG height,
      PLWORD BitsPerPixel
    );


protected:
  // Local functions

  // Create a new bitmap with uninitialized bits. Assumes no memory
  // is allocated yet.
  virtual void internalCreate
    ( PLLONG Width,
      PLLONG Height,
      const PLPixelFormat& pf
    );

  // Deletes memory allocated by member variables.
  virtual void freeMembers
    ();

  // Initializes internal table of line addresses.
  virtual void initLineArray
    ();

  PLBYTE      * m_pBits;        // Pointer to the bits.
};

// Note that _both_ these copy constructors are needed. If only the
// second one is there, MSVC 6.0 generates a default copy
// constructor anyway :-(.
inline PLAnyBmp::PLAnyBmp
    ( const PLAnyBmp &Orig
    )
    : PLBmp ()
{
  internalCopy (Orig);
}

inline PLAnyBmp::PLAnyBmp
    ( const PLBmpBase &Orig
    )
    : PLBmp ()
{
  internalCopy (Orig);
}

inline PLAnyBmp & PLAnyBmp::operator=
    ( const PLBmpBase &Orig
    )
{
  PLBmp::operator=(Orig);
  return *this;
}

inline PLAnyBmp & PLAnyBmp::operator=
    ( const PLAnyBmp &Orig
    )
{
  PLBmp::operator=(Orig);
  return *this;
}


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: planybmp.h,v $
|      Revision 1.10  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.9  2004/06/19 15:39:45  uzadow
|      Bugfix
|
|      Revision 1.8  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.7  2004/06/15 11:18:17  uzadow
|      First working version of PLBmpBase.
|
|      Revision 1.6  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.5  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from 
|      images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/02/24 13:00:17  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.10  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.9  2000/11/02 21:28:47  uzadow
|      Fixed copy constructors.
|
|      Revision 1.8  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.7  2000/01/16 20:43:12  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  1999/12/14 12:30:13  Ulrich von Zadow
|      Corrected copy constructor and assignment operator.
|
|      Revision 1.5  1999/12/10 01:27:26  Ulrich von Zadow
|      Added assignment operator and copy constructor to
|      bitmap classes.
|
|      Revision 1.4  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
\--------------------------------------------------------------------
*/
