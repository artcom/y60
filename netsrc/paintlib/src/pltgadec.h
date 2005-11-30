/*
/--------------------------------------------------------------------
|
|      $Id: pltgadec.h,v 1.6 2004/06/19 16:49:07 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTGADEC
#define INCL_PLTGADEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

#ifndef INCL_PLTGA
#include "pltga.h"
#endif

//! Targa file decoder. Decodes 8, 15, 16, 24 and 32 bpp
//! targa files (compressed and uncompressed) and returns an 8 or 32
//! bpp CBitmap. Preserves the alpha channel.
class PLTGADecoder : public PLPicDecoder
{


public:
  //! Creates a decoder
  PLTGADecoder
    ();

  //! Destroys a decoder
  virtual ~PLTGADecoder
    ();

   //!
  virtual void Open (PLDataSource * pDataSrc);

  //! Fills the bitmap with the image. 
  virtual void GetImage (PLBmpBase & Bmp);

private:

  // Fills TGAHEADER structure with the information in the file.
  void readTgaHeader
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      PLDataSource * pDataSrc
    );

  // Reads the TGA palette and creates a windows palette.
  void readPalette
    ( int StartIndex,           // Index of first palette entry.
      int Length,               // Number of palette entries stored.
      int EntrySize,            // Size of palette entries in bits.
      PLBmpBase * pBmp,
      PLDataSource * pDataSrc
    );


  // Determines compression type and calls readData.
  void readImage
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      PLBmpBase * pBmp,
      PLDataSource * pDataSrc
    );

  // Reads image data line-by-line.
  void readData
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      bool bCompressed,
      PLBmpBase * pBmp,
      PLDataSource * pDataSrc
    );

  // Decodes one line of uncompressed image data.
  void expandUncompressedLine
    ( PLBYTE * pDest,
      int Width,
      bool bReversed,
      int bpp,
      PLDataSource * pDataSrc
    );

  // Decodes one line of compressed image data.
  void expandCompressedLine
    ( PLBYTE * pDest,
      int Width,
      bool bReversed,
      int bpp,
      PLDataSource * pDataSrc
    );

  // Reads one image pixel and returns it in RGBA format.
  PLPixel32 readPixel32
    ( int bpp,
      PLDataSource * pDataSrc
    );

  // Reads one image pixel and returns it in 8-bit format.
  PLBYTE readPixel8
    ( int bpp,
      PLDataSource * pDataSrc
    );

  TGAHEADER m_TgaHead;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltgadec.h,v $
|      Revision 1.6  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
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
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
\--------------------------------------------------------------------
*/
