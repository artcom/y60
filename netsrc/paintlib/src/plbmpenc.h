/*
/--------------------------------------------------------------------
|
|      Windows bitmap file encoder. Encodes 1, 4, 8, 24 and 32 bpp
|      bitmaps to a 1, 4, 8 or 24 bpp uncompressed BMP file
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLBMPENC
#define INCL_PLBMPENC

#ifndef INCL_PLPICENC
#include "plpicenc.h"
#endif

//! Encoder class for windows bmp files. Writes 1, 8 and 24 bpp. 32 bpp bitmaps 
//! are written as 24 bpp.
class PLBmpEncoder : public PLPicEncoder
{

public:
  //! Creates an encoder
  PLBmpEncoder
    ();

  //! Destroys a encoder
  ~PLBmpEncoder
    ();

  // todo: Notification not yet implemented for encoding - ms 3.6.99
  /*
  static void JNotification
    (j_common_ptr cinfo
    );
  */

protected:
  // encodes in bmp format
  void DoEncode
    ( PLBmpBase * pBmp,
      PLDataSink * pDataSink
    );

  int GetLineMemNeeded(PLLONG width, PLWORD BitsPerPixel);
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plbmpenc.h,v $
|      Revision 1.6  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.5  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.4  2002/02/24 13:00:18  uzadow
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
|      Revision 1.4  2000/12/08 12:32:00  uzadow
|      Added gif decoder by Michael Salzlechner.
|
|      Revision 1.3  2000/12/02 19:50:01  uzadow
|      Added Logging.
|
\--------------------------------------------------------------------
*/
