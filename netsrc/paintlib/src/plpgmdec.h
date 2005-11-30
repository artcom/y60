/*
/--------------------------------------------------------------------
|
|      $Id: plpgmdec.h,v 1.6 2004/06/19 16:49:07 uzadow Exp $
|
|      portable graymap Decoder Class
|
|      Original author: Jose Miguel Buenaposada Biencinto.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPGMDEC
#define INCL_PLPGMDEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

#include "plpgm.h"

//! Portable Graymap file decoder. Delivers an 8 bpp grayscale image.
class PLPGMDecoder : public PLPicDecoder
{

public:
  //! Creates a decoder
  PLPGMDecoder
    ();

  //! Destroys a decoder
  virtual ~PLPGMDecoder
    ();

  //!
  virtual void Open (PLDataSource * pDataSrc);

  //!
  virtual void GetImage (PLBmpBase & Bmp);

private:
  void readPgmHeader (PGMHEADER *pPgmHead,
	                    PLDataSource * pDataSrc);

  int  readASCIIDecimal(PLDataSource * pDataSrc);

  void skipComment(PLDataSource * pDataSrc);

  void skipPgmASCIISeparators(PLDataSource * pDataSrc);

  PLBYTE *readASCIILine(PLDataSource *pDataSrc);

  void readImage (PLBmpBase * pBmp,
                  PLDataSource * pDataSrc);

  void readData(PLBmpBase * pBmp,
                PLDataSource * pDataSrc);

  void expandASCIILine
    ( PLBYTE * pDest,
      int MaxGrayValue,
      int Width,
      PLDataSource * pDataSrc
    );

  void expandByteLine
    ( PLBYTE * pDest,
	    int MaxGrayValue,
	    int Width,
      PLDataSource * pDataSrc
    );

  PLBYTE PLPGMDecoder::readASCIIPixel8
    ( int MaxGrayValue,
      PLDataSource * pDataSrc
    );

  PLBYTE m_LastByte;
  bool m_UseLastByte;
  PGMHEADER m_PGMHeader;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plpgmdec.h,v $
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
|      Revision 1.4  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.3  2000/05/23 10:19:11  Ulrich von Zadow
|      Minor unix compatibility changes.
|
|      Revision 1.2  2000/03/21 10:51:16  Ulrich von Zadow
|      Documentation update.
|
|      Revision 1.1  2000/03/16 13:56:37  Ulrich von Zadow
|      Added pgm decoder by Jose Miguel Buenaposada Biencinto
|
|
\--------------------------------------------------------------------
*/
