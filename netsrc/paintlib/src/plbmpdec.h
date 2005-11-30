/*
/--------------------------------------------------------------------
|
|      $Id: plbmpdec.h,v 1.9 2004/06/19 16:49:07 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLBMPDEC
#define INCL_PLBMPDEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

#ifndef INCL_PLWINDEFS
#include "plwindefs.h"
#endif

//! Windows bitmap file decoder. Decodes bitmap files (compressed and
//! uncompressed) of all bit depths. Returns an 8
//! or 32 bpp bitmap.
class PLBmpDecoder : public PLPicDecoder
{

public:
  //! Creates a decoder
  PLBmpDecoder
    ();

  //! Destroys a decoder
  virtual ~PLBmpDecoder
    ();

  //! Opens the data source. 
  virtual void Open (PLDataSource * pDataSrc);

  //! Fills the bitmap with the main image. 
  virtual void GetImage (PLBmpBase & Bmp);

  //!
  virtual void Close ();

private:

  // Decodes the bitmap file & info headers
  WINBITMAPINFOHEADER * getInfoHeader
    ( PLDataSource * pDataSrc,
      PLPixel32* Pal
    );

  // Decodes a 2-color bitmap. Ignores the palette & just uses
  // black & white as 'colors'
  void decode1bpp
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp
    );

  // Decodes an uncompressed 16-color-bitmap.
  void decode4bpp
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp
    );

  // Decodes an uncompressed 256-color-bitmap.
  void decode8bpp
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp
    );

  // Decodes a compressed 16-color-bitmap.
  void decodeRLE4
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp
    );

  // Decodes a compressed 256-color-bitmap.
  void decodeRLE8
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp
    );

  void decodeHiColor
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp,
      WINBITMAPINFOHEADER * pBMI
    );

  // Decodes true-color bitmap
  void decodeTrueColor
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp,
      int SrcBPP
    );

  void decode15bppLine
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp,
      PLBYTE * pDest
    );

  void decode16bppLine
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp,
      PLBYTE * pDest
    );

  void decode24bppLine
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp,
      PLBYTE * pDest
    );

  void decode32bppLine
    ( PLDataSource * pDataSrc,
      PLBmpBase * pBmp,
      PLBYTE * pDest
    );


  // Decodes two 4-bit pixels using a palette & puts them in pDest
  // and pDest+4.
  void decode2Nibbles
    ( PLBYTE * pDest,
      PLBYTE SrcByte
    );

  // Makes a private copy of the palette in the file and sets the
  // alpha channel. Returns the number of colors read.
  int readPalette
    ( WINBITMAPINFOHEADER * pBMI,     // Pointer to bitmapinfoheader in file.
      PLDataSource * pDataSrc,
      PLPixel32 * pPal,
      int RGBSize
    );

  PLPixel32 m_Pal[256];
  WINBITMAPINFOHEADER * m_pBMI;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plbmpdec.h,v $
|      Revision 1.9  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.8  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.7  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.6  2002/03/31 13:36:41  uzadow
|      Updated copyright.
|
|      Revision 1.5  2001/10/21 17:12:39  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.4  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.3  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.2  2001/10/05 21:15:09  uzadow
|      Improved support for OS/2 bitmap decoding.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
\--------------------------------------------------------------------
*/
