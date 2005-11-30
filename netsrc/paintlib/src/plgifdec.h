/*
/--------------------------------------------------------------------
|
|      $Id: plgifdec.h,v 1.7 2004/06/19 16:49:07 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLGIFDEC
#define INCL_PLGIFDEC


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "plpicdec.h"

//! Compuserve gif file decoder using libungif to do the actual work. The
//! bitmap returned always has 8 bpp. If the gif has a transparent color,
//! the palette of the bitmap has alpha=0 for this color and HasAlpha=true.
//! Does not support loading more than one image from a multi-image gif.
//! (One issue with gifs is that you don't know whether the image has a 
//! transparent color until it's been decoded completely. For this reason,
//! calling HasAlpha for the decoder before GetImage() will always return
//! false.)
class PLGIFDecoder : public PLPicDecoder  
{
public:
  PLGIFDecoder();
  virtual ~PLGIFDecoder();

  //!
  virtual void Open (PLDataSource * pDataSrc);

  //!
  virtual void GetImage (PLBmpBase & Bmp);

  //!
  virtual void Close ();

private:
  void * m_pGifFile;

};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plgifdec.h,v $
|      Revision 1.7  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.6  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.5  2002/11/02 20:33:27  uzadow
|      Added gif transparency handling (Mark Richarme)
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/03/31 13:36:41  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.2  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.1  2000/12/08 13:41:23  uzadow
|      Added gif support by Michael Salzlechner.
|
|
\--------------------------------------------------------------------
*/
