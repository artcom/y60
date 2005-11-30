/*
/--------------------------------------------------------------------
|
|      $Id: pldirectfbbmp.h,v 1.4 2004/06/12 14:52:46 uzadow Exp $
|
|      Copyright (c) 1996-2003 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLDIRECTFBBMP
#define INCL_PLDIRECTFBBMP

#ifndef INCL_PLBITMAP
#include "plbitmap.h"
#endif

#include <directfb.h>

//! Manipulates bitmaps stored as DirectFB surfaces.
//! 8, 24 and 32 bpp (pixel formats DSPF_ARGB, DSPF_RGB32, DSPF_RGB24, DSPF_A8).
//! The surfaces used are system memory surfaces.  
class PLDirectFBBmp : public PLBmp
{

public:
  //! Sets a pointer to the main DirectFB interface. This needs to be done 
  //! before any PLDirectFBBmps get allocated.
  static void SetDirectFB
    ( IDirectFB * pDirectFB
    );

  //! Creates an empty bitmap object.
  //! The Create() function allocates a DirectFB system memory surface for this bitmap.
  //! Note that only system memory surfaces are supported at the moment.
  PLDirectFBBmp
    ();

  //! Destroys the bitmap.
  virtual ~PLDirectFBBmp
    ();

  //! Copy constructor
  PLDirectFBBmp
    ( const PLBmp &Orig
    );

  //! Copy constructor
  PLDirectFBBmp
    ( const PLDirectFBBmp &Orig
    );

  virtual void CreateFromSurface
    ( IDirectFBSurface * pSurface,
      bool bOwnsSurface
    );

#ifdef _DEBUG
  virtual void AssertValid
    () const;    // Tests internal object state
#endif

  //! Assignment operator.
  PLDirectFBBmp &operator=
    ( const PLBmp &Orig
    );

  //! Assignment operator.
  PLDirectFBBmp &operator=
    ( const PLDirectFBBmp &Orig
    );

  // PLDirectFBBmp information

  //! Returns a pointer to the internal DirectFB surface. The surface is still
  //! owned by the bitmap object.
  //! Using this pointer, you can bring DirectFB and paintlib data out of sync
  //! easily. Handle with care.
  IDirectFBSurface * GetSurface
    ();

  //! Returns the amount of memory used by the object.
  long GetMemUsed
    ();

  //! Returns number of bytes used per line.
  long GetBytesPerLine
    ();

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
      PLWORD BitsPerPixel,
      bool bAlphaChannel,
      bool bIsGreyscale
    );

  // Deletes memory allocated by member variables.
  virtual void freeMembers
    ();

  // Initializes internal table of line addresses.
  virtual void initLineArray
    ();

private: 
  static IDirectFB * s_pDirectFB; 
  IDirectFBSurface * m_pSurface;        // Pointer to the bits.
  bool m_bOwnsSurface;
};

// Note that _both_ these copy constructors are needed. If only the
// second one is there, MSVC 6.0 generates a default copy
// constructor anyway :-(.
inline PLDirectFBBmp::PLDirectFBBmp
    ( const PLDirectFBBmp &Orig
    )
    : PLBmp ()
{
  internalCopy (Orig);
}

inline PLDirectFBBmp::PLDirectFBBmp
    ( const PLBmp &Orig
    )
    : PLBmp ()
{
  internalCopy (Orig);
}

inline PLDirectFBBmp & PLDirectFBBmp::operator=
    ( const PLBmp &Orig
    )
{
  PLBmp::operator=(Orig);
  return *this;
}

inline PLDirectFBBmp & PLDirectFBBmp::operator=
    ( const PLDirectFBBmp &Orig
    )
{
  PLBmp::operator=(Orig);
  return *this;
}


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pldirectfbbmp.h,v $
|      Revision 1.4  2004/06/12 14:52:46  uzadow
|      Added CreateFromSurface function.
|
|      Revision 1.3  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.2  2004/04/16 20:14:41  uzadow
|      Changes to make cdoc work.
|
|      Revision 1.1  2003/07/27 13:50:48  uzadow
|      Added support for DirectFB surfaces.
|
|
|
\--------------------------------------------------------------------
*/
