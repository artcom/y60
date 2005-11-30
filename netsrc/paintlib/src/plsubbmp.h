/*
/--------------------------------------------------------------------
|
|      $Id: plsubbmp.h,v 1.3 2004/09/09 16:52:49 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLSUBBMP
#define INCL_PLSUBBMP

#ifndef INCL_PLBITMAP
#include "plbitmap.h"
#endif

class PLRect;


//! A PLSubBmp is a way to manipulate bitmap data in an application-defined 
//! memory region or a rectangular subsection of another PLBmp. All operations
//! on PLBmps that don't change the size or bpp of the image can be performed
//! on a PLSubBmp. A PLSubBmp can be used as source for all filter operations. 
//! It doesn't own the pixel data it manipulates, so care must be taken to ensure
//! that the data remains valid as long as the PLSubBmp exists.
class PLSubBmp : public PLBmpBase
{

public:
  //! Creates an empty bitmap.
  PLSubBmp
    ();

  //! Destroys the bitmap.
  virtual ~PLSubBmp
    ();

  //! Creates a PLSubBmp as a rectangle in another bitmap.
  virtual void Create
    ( PLBmpBase & SrcBmp,
      const PLRect & SrcRect
    );
    
  //! Creates a PLSubBmp from an arbitrary memory region. Stride
  //! is the distance in bytes from one line of the bitmap to the
  //! next.
  virtual void Create
    ( int Width,
      int Height,
      const PLPixelFormat& pf,
      PLBYTE * pBits,
      int Stride
    );

private:

  // Deletes memory allocated by member variables.
  virtual void freeMembers
    ();
};



#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plsubbmp.h,v $
|      Revision 1.3  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.2  2004/06/19 17:34:27  uzadow
|      Documentation update
|
|      Revision 1.1  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|
\--------------------------------------------------------------------
*/
