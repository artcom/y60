/*
/--------------------------------------------------------------------
|
|      $Id: pltiffdec.h,v 1.7 2004/06/19 16:49:07 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTIFFDEC
#define INCL_PLTIFFDEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

#ifndef INCL_PLPOINT
#include "plpoint.h"
#endif

extern "C"
{
#include "tiff.h"
#include "tiffio.h"
}

//! TIFF file decoder. Uses LIBTIFF to do the actual conversion.
//! Supports 1, 8, 24 and 32 bits per pixel as output formats.
class PLTIFFDecoder : public PLPicDecoder
{

friend class _tiff_helper;
friend class PLTIFFEncoder;

public:
  //! Creates a decoder
  PLTIFFDecoder
    ();

  //! Destroys a decoder
  virtual ~PLTIFFDecoder
    ();

  //!
  virtual void Open (PLDataSource * pDataSrc);

  //! Fills the bitmap with the image. 
  virtual void GetImage (PLBmpBase & Bmp);

  //!
  virtual void Close ();

  int GetField( int tag_id, ... );
  //! this is this first argument to most libtiff's routines
  //! use at your own risk; you've been warned!
  TIFF* GetTIFFHandle()	{ return m_pTif; }

private:
	// these 2 helpers dedicated to hi/low bit-depths respectively
	void doHiColor
    ( TIFF * tif,
      PLBmpBase * pBmp,
      uint16 SamplePerPixel
    );

	void doLoColor
    ( TIFF * tif,
      PLBmpBase * pBmp
    );

  const PLPoint getResolution (TIFF* tif);

  // Callback from the TIFF library.
  // Should be accessed only by the TIFF decoder and libtiff.
  static void Win32WarningHandler
    ( const char* module,
      const char* fmt,
      va_list ap
    );

  // Callback from the TIFF library.
  // Should be accessed only by the TIFF decoder and libtiff.
  static void Win32ErrorHandler
    ( const char* module,
      const char* fmt,
      va_list ap
    );

private:
  // Temporary storage for an error message. 
  // Used in the callbacks, so it's static.
  static char m_szLastErr[256];

  TIFF* m_pTif;
};


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltiffdec.h,v $
|      Revision 1.7  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.6  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.5  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.2  2001/10/17 13:19:47  uzadow
|      Added support for photoshop tiff alpha channels.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.7  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.6  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  2000/01/08 15:56:12  Ulrich von Zadow
|      Made sure change logging works in every file.
|
|      Revision 1.4  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
