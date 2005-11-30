/*
/--------------------------------------------------------------------
|
|      $Id: pliff85dec.h,v 1.2 2004/06/19 16:49:07 uzadow Exp $
|      Electronic Arts IFF 85 Decoder Class
|
|       REFERENCES :
|           Jerry Morrison, Electronic Arts; "EA IFF 85" Standard for
|           Interchange Format Files; January 14, 1985
|           Available from http://www.wotsit.org as iff.zip
|
|           Jerry Morrison, Electronic Arts; "ILBM" IFF Interleaved Bitmap;
|           January 17, 1986
|           Available from http://www.wotsit.org as ilbm.zip
| 
|      Rupert Welch (rafw@mindless.com) - December 2003
|
|      Copyright (c) 2003-2004 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLIFF85DEC)
#define      INCL_PLIFF85DEC

#if !defined(INCL_PLPICDEC)
#include "plpicdec.h"
#endif

#include "pliff85.h"

//! EA IFF-85 ILBM/PBM decoder.  Will decode a single-image file.
class PLIFF85Decoder : public PLPicDecoder
{

public:
  //! Creates a decoder
  PLIFF85Decoder
    ();

  //! Destroys a decoder
  virtual ~PLIFF85Decoder
    ();

  //! Opens the data source and extracts image properties.
  virtual void Open(PLDataSource * pDataSrc);

  //! Fills the bitmap with the image. 
  virtual void GetImage(PLBmpBase & Bmp);

private:
  void readChunkHeader(PLIFF85::Chunk& chunk, PLDataSource * pDataSrc);
  void readCompressedRow(PLBYTE * pRow, PLDataSource * pDataSrc, int numBytes);
  void readUncompressedRow(PLBYTE * pRow, PLDataSource * pDataSrc, int numBytes);
  int getBytesPerRow() const;

  PLIFF85::ID           m_formType;
  PLIFF85::BitMapHeader m_bitmapHeader;
  PLPixel32             m_pal[256];
  PLIFF85::Viewmode     m_viewMode;
};

#endif // !defined(INCL_PLIFF85DEC)

/*
/--------------------------------------------------------------------
|
|      $Log: pliff85dec.h,v $
|      Revision 1.2  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.1  2004/03/13 19:40:23  uzadow
|      Added Rupert Welchs iff decoder.
|
|
\--------------------------------------------------------------------
*/
