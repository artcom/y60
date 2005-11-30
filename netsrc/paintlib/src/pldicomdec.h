/*
/--------------------------------------------------------------------
|
|      $Id: pldicomdec.h,v 1.1 2004/09/15 14:52:09 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLDICOMDEC
#define INCL_PLDICOMDEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

class PLProducer;
class PLInputStream;
class DicomImage;

class PLDICOMDecoder 
  : public PLPicDecoder
{
public:
  //! Creates a pDecoder
  PLDICOMDecoder
    ();

  //! Destroys a pDecoder
  virtual ~PLDICOMDecoder
    ();

   //!
  virtual void Open (PLDataSource * pDataSrc);
  virtual void Close ();

  //! Fills the bitmap with the image. 
  virtual void GetImage (PLBmpBase & Bmp);

private:
  DicomImage * m_di;
  bool m_bInvert;
  PLProducer * m_pProducer;
  PLInputStream * m_pInputStream;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pldicomdec.h,v $
|      Revision 1.1  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
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
|      Added PSD pDecoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
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
