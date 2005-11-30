/*
/--------------------------------------------------------------------
|
|      $Id: pltiffenc.h,v 1.5 2004/06/19 18:16:33 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTIFFENC
#define INCL_PLTIFFENC

#ifndef INCL_PLPICENC
#include "plpicenc.h"
#endif

typedef struct tiff TIFF;

//! TIFF file encoder. Uses LIBTIFF to do the actual conversion.
class PLTIFFEncoder : public PLPicEncoder
{

public:
  //! Creates an encoder
  PLTIFFEncoder
    ();

  //! Destroys a encoder
  virtual ~PLTIFFEncoder
    ();

  void SetCompression (PLWORD Compression);

protected:
  //! Sets up LIBTIFF environment and calls LIBTIFF to encode an image.
  virtual void DoEncode
    ( PLBmpBase* pBmp,
      PLDataSink* pDataSnk
    );

  //! this one mostly useful for the "extended" decoder
  virtual void DoTiffEncode
    ( PLBmpBase* pBmp,
      TIFF* tif
    );

  //! According to the characteristics of the given bitmap,
  //! set the baseline tags
  int SetBaseTags( TIFF*, PLBmpBase* );

  //! Set tag value in directory; also, "pseudo-tags" are interpreted by
  //! LIBTIFF as modifiers to certain algorithms (compression....).
  //! We do _not_ support multiple images by file, nor tiling nor stripping.
  int SetField( TIFF*, int tag_id, ... );

private:
  PLWORD m_Compression;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: pltiffenc.h,v $
|      Revision 1.5  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.4  2002/02/24 13:00:41  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.3  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.2  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.7  2001/09/13 20:43:02  uzadow
|      Added tiff compression support.
|
|      Revision 1.6  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|
\--------------------------------------------------------------------
*/
