/*
/--------------------------------------------------------------------
|
|      $Id: pltester.h,v 1.31 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTER
#define INCL_PLTESTER

#include "pltest.h"
#include "config.h"

#ifdef _WIN32
#include <windows.h>
#else
#include "plpaintlibdefs.h"
#endif

#include "plbitmap.h"
#include "planybmp.h"

#ifdef _WIN32
#include "pldibsect.h"
// #include "plddrawbmp.h"
#endif

#include "plpicenc.h"
#include "plpicdec.h"

#include <string>

class PLFilter;

class PLTester: public PLTest
{

public:
  PLTester ();

  virtual ~PLTester();

  virtual void RunTests ();

  static const std::string m_sTestFileDir;
  static const std::string m_sTestFileList;

private:
  void testBmpInfo (PLBmpInfo& BmpInfo);
  void testBmpBase(PLBmpBase& Bmp);
  void testPLBmp (PLBmp& Bmp);
#ifdef _WIN32
  void testPLWinBmp (PLWinBmp& Bmp);
  void testLoadRes ();
  void testDIBSect (PLDIBSection& Bmp);
#endif
  void testPixels ();
  void testPoint ();
  void testRect ();

  void testCodec (const PLPicEncoder& Encoder, const PLPicDecoder& Decoder);
  void testCodecBmp (PLBmp& Bmp, const PLPicEncoder& Encoder,
                     const PLPicDecoder& Decoder, char * pszFName);
#ifdef PL_SUPPORT_TIFF
  void testCTIFFEx (PLBmp& Bmp);
#endif

  void setUpBmp (PLBmp& Bmp, const PLPixelFormat & pf);
  void setUp ();
  void tearDown ();
  void dumpBmp (PLBmp& bmp);
  bool almostEqual (PLBmp& bmp1, PLBmp& bmp2, int epsilon);

  PLAnyBmp m_AnyBmp8;
  PLAnyBmp m_AnyBmp16;
  PLAnyBmp m_AnyBmp24;
  PLAnyBmp m_AnyBmp32;

#ifdef _WIN32
  PLDIBSection m_DIBSect8;
  PLDIBSection m_DIBSect32;
#endif

};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltester.h,v $
|      Revision 1.31  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.30  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.29  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.28  2004/06/15 14:17:25  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.27  2004/06/09 20:27:52  uzadow
|      Added 16 bpp pixel class.
|
|      Revision 1.26  2004/04/15 19:09:38  uzadow
|      - Moved TestBmpList to test source, where it is versioned.
|      - All test output goes to cerr now.
|
|      Revision 1.25  2003/07/27 13:50:49  uzadow
|      Added support for DirectFB surfaces.
|
|      Revision 1.24  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|      Revision 1.23  2003/03/08 15:16:02  uzadow
|      Added PLSDLBmp::Attach().
|
|      Revision 1.22  2003/03/08 14:32:20  uzadow
|      Added support for bitmaps in SDL surface format.
|
|      Revision 1.21  2002/11/27 22:32:24  uzadow
|      Now works if TIFF, PNG and/or JPEG support are not available
|
|      Revision 1.20  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.19  2002/02/24 13:00:49  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.18  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.17  2001/10/03 14:00:29  uzadow
|      Much improved quality in FilterResizeBilinear.
|
|      Revision 1.16  2001/09/30 19:55:37  uzadow
|      Fixed bug for 8 bpp in PLFilterCrop.
|
|      Revision 1.15  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.14  2001/09/24 14:19:19  uzadow
|      Added PLPoint & PLRect tests.
|
|      Revision 1.13  2001/09/16 20:57:17  uzadow
|      Linux version name prefix changes
|
|      Revision 1.12  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.11  2001/09/15 14:30:20  uzadow
|      Fixed PLPixel32 initialization bug.
|
|      Revision 1.10  2001/09/13 20:49:31  uzadow
|      Added fill filters.
|
|      Revision 1.9  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.8  2001/01/13 20:06:16  uzadow
|      Added Flip and Mirror filters.
|
|      Revision 1.7  2000/12/18 22:42:53  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.6  2000/12/09 12:16:26  uzadow
|      Fixed several memory leaks.
|
|      Revision 1.5  2000/12/04 23:59:02  uzadow
|      Added filter tests.
|
|      Revision 1.4  2000/12/03 21:15:43  uzadow
|      Fixed png handling in configure; test/ adapted to linux
|
|      Revision 1.3  2000/12/02 19:42:30  uzadow
|      Added PLTIFFEncoderEx test.
|
|      Revision 1.2  2000/11/21 23:28:53  uzadow
|      Moved PLBmp tests from testdec to test.
|
|      Revision 1.1  2000/11/21 20:29:39  uzadow
|      Added test project.
|
|
\--------------------------------------------------------------------
*/
