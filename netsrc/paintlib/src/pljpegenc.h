/*
/--------------------------------------------------------------------
|
|      $Id: pljpegenc.h,v 1.5 2004/06/19 18:16:33 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLJPEGENC
#define INCL_PLJPEGENC

#ifndef INCL_PLPICENC
#include "plpicenc.h"
#endif

#include "plexif.h"

struct jpeg_compress_struct;
struct jpeg_error_mgr;

//! JPEG file encoder. Uses the independent JPEG group's library
//! to do the actual conversion.
class PLJPEGEncoder : public PLPicEncoder
{

public:
  //! Creates an encoder
  PLJPEGEncoder
    ();

  //! Destroys a encoder
  ~PLJPEGEncoder
    ();

  // todo: Notification not yet implemented for encoding.
  /*
  static void JNotification
    (j_common_ptr cinfo
    );
  */

  //! Set the compression quality on a scale from 0 to 100.
  void SetQuality(int iQuality);
  //! Enable or disable the generation of optimal Huffmann coding tables.
  void SetOptimizeCoding(bool bOptimizeCoding);
  //! Set the smoothing factor (<=100). 0 turns it off.
  void SetSmoothingFactor(int iSmoothingFactor);
  //! Set the resolution information (DPI) for the image.
  void SetDensity(unsigned int uiX, unsigned int uiY);

  //! Set Exif (digital camera) Data for the next encode. The encoder only 
  //! holds on to the Exif data until the next encode.
  void SetExifData(PLExif& ExifData);

protected:
  // Sets up the jpeg library data source and error handler and
  // calls the jpeg encoder.
  void DoEncode
    ( PLBmpBase * pBmp,
      PLDataSink * pDataSink
    );

private:
  // Assumes IJPEG decoder is already set up.
  void encodeRGB
    ( PLBmpBase * pBmp,
      int iScanLines
    );

  jpeg_compress_struct * m_pcinfo;  // IJPEG encoder state.
  jpeg_error_mgr       * m_pjerr;   // Custom error manager.

  int iQuality_;
  bool bOptimizeCoding_;
  int iSmoothingFactor_;
  unsigned int uiDensityX_;
  unsigned int uiDensityY_;
  PLExif* m_pExifData;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: pljpegenc.h,v $
|      Revision 1.5  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.4  2003/04/19 19:03:52  uzadow
|      Exif save (windows)
|
|      Revision 1.3  2002/02/24 13:00:21  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2000/05/22 17:43:25  Ulrich von Zadow
|      Added SetQuality(), SetDensity(), SetOptimizeCoding() and
|      SetSmoothingFactor().
|
|      Revision 1.4  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.2  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/19 21:28:05  Ulrich von Zadow
|      Added jpeg encoder
|
|
\--------------------------------------------------------------------
*/
