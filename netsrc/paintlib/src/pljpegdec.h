/*
/--------------------------------------------------------------------
|
|      $Id: pljpegdec.h,v 1.7 2004/06/19 16:49:07 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLJPEGDEC
#define INCL_PLJPEGDEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

#include <stdio.h>

extern "C"
{
#include "jpeglib.h"
}

class PLExif;

//! JPEG file decoder. Uses the independent JPEG group's library
//! to do the actual conversion.
class PLJPEGDecoder : public PLPicDecoder
{

public:
  //! Creates a decoder
  PLJPEGDecoder
    ();

  //! Destroys a decoder
  ~PLJPEGDecoder
    ();

  //!
  virtual void Open (PLDataSource * pDataSrc);

  //! Fills the bitmap with the image. 
  virtual void GetImage (PLBmpBase & Bmp);

  void GetExifData(PLExif& ExifData);

  virtual void Close();

  //! true (the default) selects fast but sloppy decoding.
  void SetFast
    ( bool bFast
    );

  // Jo Hagelberg 15.4.99
  // this must be static as it's passed to extern C
  //! callback for jpeglib's progress notification
  static void JNotification
    (j_common_ptr cinfo
    );

private:
  // Assumes IJPEG decoder is already set up.
  void decodeGray
    ( PLBmpBase * pBmp);

  // Assumes IJPEG decoder is already set up.
  void decodeRGB
    ( PLBmpBase * pBmp);

  jpeg_decompress_struct cinfo;  // IJPEG decoder state.
  jpeg_error_mgr         jerr;   // Custom error manager.

  bool  m_bFast;  // true selects fast but sloppy decoding.
  PLExif * m_pExifData;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pljpegdec.h,v $
|      Revision 1.7  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.6  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.5  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2001/09/15 21:02:44  uzadow
|      Cleaned up stdpch.h and config.h to make them internal headers.
|
|      Revision 1.7  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.6  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.4  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|
\--------------------------------------------------------------------
*/
