/*
/--------------------------------------------------------------------
|
|      $Id: pltester.cpp,v 1.74 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "config.h"

#include "pltester.h"

#include "plpaintlibdefs.h"
#include "planybmp.h"
#include "plsubbmp.h"
#include "planydec.h"
#include "plbmpenc.h"
#include "plbmpdec.h"
#ifdef PL_SUPPORT_JPEG
#include "pljpegenc.h"
#include "pljpegdec.h"
#endif
#ifdef PL_SUPPORT_TIFF
#include "pltiffenc.h"
#include "pltiffencex.h"
#include "pltiffdec.h"
#endif
#ifdef PL_SUPPORT_PNG
#include "plpngenc.h"
#include "plpngdec.h"
#endif
#include "plfilesink.h"
#include "plpixel32.h"
#include "plpixel24.h"
#include "plpixel16.h"
#include "plpixel8.h"
#include "plpoint.h"
#include "plrect.h"
#include "plmemsink.h"

#ifdef PL_SUPPORT_INFO
#include "plinfodec.h"
#endif

#include "pltestdecoders.h"
#include "pltestcountedpointer.h"
#include "pltestfilters.h"
#include "pltestpixelformat.h"

#include <typeinfo>
#include <iostream>
#include <string>
#include <fstream>

#ifdef _WIN32
#include "../../win/test/resource.h"
#endif

#ifdef WIN32
const std::string PLTester::m_sTestFileDir ("..\\..\\..\\testpic\\");
const std::string PLTester::m_sTestFileList ("..\\..\\test\\TestBmpList.txt");
#else
const std::string PLTester::m_sTestFileDir ("../../../testpic/");
const std::string PLTester::m_sTestFileList ("../../test/TestBmpList.txt");
#endif

#define PL_TESTIMAGES_AVAILABLE

using namespace std;
PLTester::PLTester ()
{
}

PLTester::~PLTester ()
{
}

void PLTester::RunTests ()
{
  setUp();
  try
  {

    cerr << "Testing PixelFormats..\n";
    PLTestPixelFormat pltf;
    pltf.RunTests();
    AggregateStatistics(pltf);

    cerr << "Testing Bmp classes...\n";
    cerr << "  Testing PLBmpInfo...\n";
    testBmpInfo (m_AnyBmp32);
    cerr << "  Testing 32 bpp PLAnyBmp...\n";
    testPLBmp (m_AnyBmp32);

    cerr << "  Testing 24 bpp PLAnyBmp...\n";
    testPLBmp (m_AnyBmp24);

    cerr << "  Testing 16 bpp PLAnyBmp...\n";
    testPLBmp (m_AnyBmp16);

    cerr << "  Testing 8 bpp PLAnyBmp...\n";
    testPLBmp (m_AnyBmp8);

#ifdef _WIN32
    cerr << "  Testing 32 bpp PLDIBSection...\n";
    testPLBmp (m_DIBSect32);
    testPLWinBmp (m_DIBSect32);
    testDIBSect (m_DIBSect32);
    setUp();
    cerr << "  Testing 8 bpp PLDIBSection...\n";
    testPLBmp (m_DIBSect8);
    testPLWinBmp (m_DIBSect8);
    testDIBSect (m_DIBSect32);
    setUp();
    //cerr << "  Testing load from resource...\n";
    //testLoadRes ();
#endif
    PLBYTE TempBytes[10240];

    setUp();
    cerr << "  Testing 32 bpp PLSubBmp...\n";
    PLSubBmp SubBmp32;
    SubBmp32.Create(m_AnyBmp32, PLRect(0, 0, 5, 5));
    testBmpBase(SubBmp32);
    PLSubBmp SubBmp32_Raw;
    SubBmp32_Raw.Create(16, 16, PLPixelFormat::A8R8G8B8, TempBytes, 128);
    testBmpBase(SubBmp32_Raw);

    cerr << "  Testing 24 bpp PLSubBmp...\n";
    PLSubBmp SubBmp24;
    SubBmp24.Create(m_AnyBmp24, PLRect(1, 1, 4, 4));
    testBmpBase(SubBmp24);
    PLSubBmp SubBmp24_Raw;
    SubBmp24_Raw.Create(16, 16, PLPixelFormat::R8G8B8, TempBytes, 64);
    testBmpBase(SubBmp24_Raw);

    cerr << "  Testing 16 bpp PLSubBmp...\n";
    PLSubBmp SubBmp16;
    SubBmp16.Create(m_AnyBmp16, PLRect(1, 1, 4, 4));
    testBmpBase(SubBmp16);
    PLSubBmp SubBmp16_Raw;
    SubBmp16_Raw.Create(16, 16, PLPixelFormat::X1R5G5B5, TempBytes, 32);
    testBmpBase(SubBmp16_Raw);

    cerr << "  Testing 8 bpp PLSubBmp...\n";
    PLSubBmp SubBmp8;
    SubBmp8.Create(m_AnyBmp8, PLRect(0, 0, 5, 5));
    testBmpBase(SubBmp8);
    PLSubBmp SubBmp8_Raw;
    SubBmp8_Raw.Create(16, 16, PLPixelFormat::L8, TempBytes, 128);
    testBmpBase(SubBmp8_Raw);

    cerr << "  Testing pixel classes...\n";
    testPixels ();

    cerr << "  Testing PLPoint...\n";
    testPoint ();
    cerr << "  Testing PLRect...\n";
    testRect ();

    cerr << "Testing counted pointer class...\n";
    PLTestCountedPointer TestPtr;
    TestPtr.RunTests();
    AggregateStatistics(TestPtr);

    PLTestDecoders TestDec;
    TestDec.RunTests();
    AggregateStatistics(TestDec);

#ifdef PL_SUPPORT_BMP
    cerr << "Testing Bmp Codec...\n";
#ifdef WORDS_BIGENDIAN
    cerr << "  Not implemented for bigendian machines.\n";
    cerr << "    --> skipping.\n";
#else
    testCodec (PLBmpEncoder(), PLBmpDecoder());
#endif
#else
    cerr << "Skipping Bmp Codec check - support not compiled into library.\n";
#endif
#ifdef PL_SUPPORT_TIFF
    cerr << "Testing TIFF Codec...\n";
    testCodec (PLTIFFEncoder(), PLTIFFDecoder());
    cerr << "Testing TIFF Codec with compression...\n";
    cerr << "  Testing 32 bpp PLAnyBmp....\n";
    testCTIFFEx (m_AnyBmp32);
    cerr << "  Testing 8 bpp PLAnyBmp....\n";
    testCTIFFEx (m_AnyBmp8);
#else
    cerr << "Skipping TIFF Codec check - support not compiled into library.\n";
#endif
#ifdef PL_SUPPORT_PNG
    cerr << "Testing PNG Codec without compression...\n";
    testCodec (PLPNGEncoder(), PLPNGDecoder());
    cerr << "Testing PNG Codec with compression...\n";
    PLPNGEncoder encoder;
    encoder.SetCompressionLevel(9);
    testCodec (encoder, PLPNGDecoder());

#else
    cerr << "Skipping PNG Codec check - support not compiled into library.\n";
#endif

// Test Amiga icon support for all types and images.
// Decoder is also tested in pltestdecoders.cpp with the others.
#ifdef PL_SUPPORT_INFO
    cerr << "Testing INFO Decoder type and image selection..." << endl;
    PLINFODecoder infoDecoder;
    PLAnyBmp infoBmp;

    cerr << "  Reading standard icon, unselected image..." << endl;
    infoDecoder.MakeBmpFromFile ("..\\..\\..\\testpic\\amigastd.info", &infoBmp);

    cerr << "  Reading standard icon, selected image..." << endl;
    infoDecoder.SetImage(PL_INFO_IMAGE_SELECTED);
    infoDecoder.MakeBmpFromFile ("..\\..\\..\\testpic\\amigastd.info", &infoBmp);

    cerr << "  Reading NewIcon, unselected image..." << endl;
    infoDecoder.SetType(PL_INFO_ICON_NEWICON);
    infoDecoder.SetImage(PL_INFO_IMAGE_UNSELECTED);
    infoDecoder.MakeBmpFromFile ("..\\..\\..\\testpic\\amiganew.info", &infoBmp);

    cerr << "  Reading NewIcon, selected image..." << endl;
    infoDecoder.SetImage(PL_INFO_IMAGE_SELECTED);
    infoDecoder.MakeBmpFromFile ("..\\..\\..\\testpic\\amiganew.info", &infoBmp);

#else
    cerr << "Skipping INFO Decoder check - support not compiled into library.\n" << endl;
#endif

    cerr << "Testing Filters...\n";
    setUp();
    PLTestFilters FilterTest(&m_AnyBmp32, &m_AnyBmp24, &m_AnyBmp16, &m_AnyBmp8);
    FilterTest.RunTests();
    AggregateStatistics (FilterTest);
    if (!FilterTest.IsOk())
      SetFailed();

    cerr << "------ Test results ------\n";
    cerr << "Number of successful tests: " << GetNumSucceeded() << endl;
    cerr << "Number of failed tests: " << GetNumFailed() << endl;
    if (GetNumFailed())
    {
      cerr << "------ WARNING: some tests failed!\n";
    }

    // TODO:
    // - JPEG Codec test.
    // - Codec tests using CMemSource and PLMemSink.
    // - More error handling tests (libcurl errors,...).

  }
  catch (PLTextException& e)
  {
    cerr << "\npaintlibtest: " << (const char*)e;
    Test(false);
  }
  tearDown();
}

void PLTester::testBmpInfo (PLBmpInfo& BmpInfo)
{
  char sz[256];
  BmpInfo.AsString(sz, 256);
  PLBmpInfo InfoCopy (sz);
  Test (InfoCopy == BmpInfo);
}

void PLTester::testBmpBase(PLBmpBase& Bmp)
{
  {
    cerr << "    Testing copy constructors...\n";
    PLAnyBmp AnyBmp (Bmp);
    Test (Bmp == AnyBmp);
#ifdef _WIN32
    PLWinBmp WinBmp (Bmp);
    Test (Bmp == WinBmp);

    PLDIBSection DIBSect (Bmp);
    Test (Bmp == DIBSect);

#endif

  }
  {
    cerr << "    Testing assignment operators...\n";
    PLAnyBmp AnyBmp;
    AnyBmp = Bmp;
    Test (Bmp == AnyBmp);
#ifdef _WIN32
    PLWinBmp WinBmp;
    PLDIBSection DIBSect;
    WinBmp = Bmp;
    Test (Bmp == WinBmp);
    DIBSect = Bmp;
    Test (Bmp == DIBSect);
#endif
  }
  // Misc. tests
  {
    if (Bmp.GetBitsPerPixel() == 8)
    {
      cerr << "    Testing FindNearestColor()...\n";
      PLPixel32 Pix = Bmp.GetPalette()[0];
      PLBYTE index = Bmp.FindNearestColor (Pix);
      Test (index == 0);
    }
  }
}

void PLTester::testPLBmp (PLBmp& Bmp)
{
  testBmpBase(Bmp);

  {
    cerr << "      Testing for duplicated members...\n";
    PLAnyBmp * pBmp2 = new PLAnyBmp(Bmp);
    PLAnyBmp * pBmp3 = new PLAnyBmp();
    *pBmp3 = *pBmp2;
    delete pBmp3;
    delete pBmp2;
  }

  // CreateCopy tests
  {
    if (Bmp.GetBitsPerPixel() == 32)
    {
      cerr << "    Testing CreateCopy()...\n";
      PLAnyBmp AnyBmp8;
      PLAnyBmp AnyBmp16;
      PLAnyBmp AnyBmp24;
      PLAnyBmp AnyBmp32;
      AnyBmp24.CreateCopy (Bmp, PLPixelFormat::R8G8B8);
      AnyBmp32.CreateCopy (AnyBmp24, PLPixelFormat::X8R8G8B8);
//      AnyBmp32.CreateCopy (Bmp);
//      Bmp.CreateCopy (AnyBmp32, 32);
      Test (AnyBmp32 == Bmp);

      AnyBmp16.CreateCopy (Bmp, PLPixelFormat::X1R5G5B5);
      AnyBmp32.CreateCopy (AnyBmp16, PLPixelFormat::X8R8G8B8);
      Test (AnyBmp32.AlmostEqual(Bmp, 8));

      AnyBmp8.CreateCopy (Bmp, PLPixelFormat::I8);
      AnyBmp32.CreateCopy (AnyBmp8, PLPixelFormat::X8R8G8B8);
      Test (AnyBmp32.AlmostEqual(Bmp, 8));
    }
  }
}

#ifdef _WIN32
void PLTester::testPLWinBmp (PLWinBmp& Bmp)
{
  // Test clipboard
  cerr << "    Testing clipboard support....\n";
  Bmp.ToClipboard();

  PLWinBmp DestBmp;
  DestBmp.FromClipboard();
  cerr << "        expected to fail...\n";
//  Test (Bmp == DestBmp);
}

void PLTester::testLoadRes ()
{
  // Load from resource
  PLWinBmp ResBmp;
  ResBmp.CreateRes (NULL, IDB_TEST);
  // We can't make sure the data is loaded correctly because CreateRes
  // outputs a 24 bpp bitmap :-(.
}

void PLTester::testDIBSect (PLDIBSection& Bmp)
{
  // Get the handle and create a PLWinBmp from it again.
  cerr << "    Testing GetHandle()...\n";
  HBITMAP hBmp = Bmp.GetHandle ();
  PLWinBmp TestBmp;
  TestBmp.CreateFromHBitmap (hBmp);
  TestBmp.SetResolution(PLPoint(72,72));  // Resolution isn't preserved.
  Test (TestBmp == Bmp);

  // Test if Detach() works.
  cerr << "    Testing Detach()...\n";
  hBmp = Bmp.GetHandle();
  BITMAPINFOHEADER * pBMI = Bmp.GetBMI();
  Bmp.Detach();
  DeleteObject (hBmp);
  free (pBMI);
  PLASSERT_VALID (&Bmp);
}
#endif

void PLTester::testPixels ()
{
  // 32 bpp
  cerr << "    Testing PLPixel32" << endl;
  PLPixel32 px1(0,1,2,3);
  PLPixel32 px2;
  px2.Set (0,1,2,3);

  Test (px1 == px2);

  Test (px1.GetR() == 0);
  Test (px1.GetG() == 1);
  Test (px1.GetB() == 2);
  Test (px1.GetA() == 3);

  px2.SetR (4);
  px2.SetG (5);
  px2.SetB (6);
  px2.SetA (7);
  Test (px1 != px2);

  px1 = px2;
  Test (px1 == px2);

  Test (px1.BoxDist (px2) == 0);

  PLPixel32 px3;
  px3 = PLPixel32::Blend(256, PLPixel32(255,255,255,0), PLPixel32(0,0,0,0));
  Test (px3 == PLPixel32 (255,255,255,0));
  px3 = PLPixel32::Blend(0, PLPixel32(255,255,255,0), PLPixel32(0,0,0,0));
  Test (px3 == PLPixel32 (0,0,0,0));
  px3 = PLPixel32::Blend(128, PLPixel32(255,0,0,0), PLPixel32(0,255,0,0));
  Test (px3 == PLPixel32 (127,127,0,0));

  // 24 bpp
  cerr << "    Testing PLPixel24" << endl;
  Test (sizeof (PLPixel24) == 3);
  PLPixel24 pxs[4];
  Test (sizeof (pxs) == 12);

  PLPixel24 px241 (0,1,2);
  PLPixel24 px242;
  px242.Set (0,1,2);
  Test (px241 == px242);
  Test (px241.GetR() == 0);
  Test (px241.GetG() == 1);
  Test (px241.GetB() == 2);

  px242.SetR (4);
  px242.SetG (5);
  px242.SetB (6);
  Test (px241 != px242);

  px241 = px242;
  Test (px241 == px242);

  Test (px241.BoxDist (px242) == 0);

  px1.Set (8,9,10,11);
  px241 = px1;
  Test (px241 == PLPixel24 (8,9,10));

  // 16 bpp
  cerr << "    Testing PLPixel16" << endl;
  PLPixel16 px161 (0,16,32);
  PLPixel16 px162;
  px162.Set (0,16,32);
  Test (px161 == px162);
  Test (px161.GetR() == 0);
  Test (px161.GetG() == 16);
  Test (px161.GetB() == 32);

  PLPixel16 px163 (255,255,255);
//cerr << (int)px163.GetR() << ", " << (int)px163.GetG() << ", " << (int)px163.GetB() << endl;
  Test(px163.GetR() == 0xF8);
  Test(px163.GetG() == 0xFC);
  Test(px163.GetB() == 0xF8);


  px162.SetR (48);
  px162.SetG (64);
  px162.SetB (80);
  Test (px161 != px162);
  Test (px162.GetR() == 48);
  Test (px162.GetG() == 64);
  Test (px162.GetB() == 80);

  px161 = px162;
  Test (px161 == px162);

  Test (px161.BoxDist (px162) == 0);

  px1.Set (8,9,10,11);
  px161 = px1;
  Test (px161 == PLPixel16(8,9,10));
  px1 = px161;
  Test (px1 == PLPixel32(8,8,8,0xFF));

  px241.Set(12,13,14);
  px161 = px241;
  Test (px161 == PLPixel16(12,13,14));
  px241 = PLPixel24(px161);
  Test (px241 == PLPixel24(8,12,8));

  // 8 bpp
  cerr << "    Testing PLPixel8" << endl;
  PLPixel8 px81 (128);
  PLASSERT (px81 == PLPixel8(128));
  PLASSERT (px81.Get() == 128);

  PLPixel8 px82 (129);
  PLASSERT (px81 != px82);

  PLASSERT (px81.BoxDist(px82) == 1);
  px82.Set (128);
  PLASSERT (px81 == px82);
}

void PLTester::testPoint ()
{
  PLPoint pt1 (1,1);
  PLPoint pt2;
  pt2.x = 1;
  pt2.y = 1;
  PLASSERT (pt1 == pt2);
  pt2.x = 2;
  PLASSERT (pt1 != pt2);

  pt1 += PLPoint (1,0);
  PLASSERT (pt1 == pt2);

  pt1 -= PLPoint (1,0);
  PLASSERT (pt1 == PLPoint (1,1));

  pt2 = -pt1;
  PLASSERT (pt2 == PLPoint (-1, -1));

  PLASSERT (pt1+pt2 == PLPoint (0,0));
  PLASSERT (pt1-pt2 == PLPoint (2,2));

  PLASSERT (PLPoint (2,2)/2 == PLPoint (1,1));
  PLASSERT (PLPoint (1,1)*2 == PLPoint (2,2));
}

void PLTester::testRect ()
{
  PLRect Rect (1,1,5,6);
  PLASSERT (Rect == PLRect (1,1,5,6));

  PLASSERT (Rect.Width() == 4);
  PLASSERT (Rect.Height() == 5);

  PLASSERT (Rect == PLRect (PLPoint (1,1), PLPoint (5,6)));

  PLASSERT (Rect.Contains(PLPoint(1,1)));
  PLASSERT (Rect.Contains(PLPoint(1,5)));
  PLASSERT (Rect.Contains(PLPoint(4,1)));
  PLASSERT (Rect.Contains(PLPoint(4,5)));
  PLASSERT (!Rect.Contains(PLPoint(0,1)));
  PLASSERT (!Rect.Contains(PLPoint(1,0)));
  PLASSERT (!Rect.Contains(PLPoint(4,6)));
  PLASSERT (!Rect.Contains(PLPoint(5,5)));

  PLASSERT (!Rect.Intersects(PLRect(5,1,10,10)));
  PLASSERT (Rect.Intersects(PLRect(4,1,10,10)));
  PLASSERT (!Rect.Intersects(PLRect(1,6,10,10)));
  PLASSERT (Rect.Intersects(PLRect(1,5,10,10)));
  PLASSERT (!Rect.Intersects(PLRect(0,0,1,1)));
  PLASSERT (Rect.Intersects(PLRect(0,0,2,2)));
  PLASSERT (!Rect.Intersects(PLRect(0,0,1,10)));
  PLASSERT (Rect.Intersects(PLRect(0,4,2,5)));

  Rect.Expand (PLRect(0,4,1,5));
  PLASSERT (Rect == PLRect(0,1,5,6));

  Rect.Expand (PLRect(3,0,2,4));
  PLASSERT (Rect == PLRect(0,0,5,6));

  Rect.Expand (PLRect(3,3,7,4));
  PLASSERT (Rect == PLRect(0,0,7,6));

  Rect.Expand (PLRect(3,3,4,7));
  PLASSERT (Rect == PLRect(0,0,7,7));

  Rect.Intersect (PLRect(4,4,10,10));
  PLASSERT (Rect == PLRect(4,4,7,7));

  Rect.Intersect (PLRect(3,3,5,5));
  PLASSERT (Rect == PLRect(4,4,5,5));
}

void PLTester::testCodec (const PLPicEncoder& Encoder, const PLPicDecoder& Decoder)
{
  cerr << "  Testing 32 bpp PLAnyBmp....\n";
  testCodecBmp (m_AnyBmp32, Encoder, Decoder, "mytestbmp");
#ifdef PL_SUPPORT_PNG
  if (typeid(Encoder) == typeid (PLPNGEncoder))
  {
    cerr << "  Testing 24 bpp PLAnyBmp....\n";
    testCodecBmp (m_AnyBmp24, Encoder, Decoder, "mytestbmp");
  }
#endif
  cerr << "  Testing 8 bpp PLAnyBmp....\n";
  testCodecBmp (m_AnyBmp8, Encoder, Decoder, "mytestbmp");
#ifdef _WIN32
  cerr << "  Testing 32 bpp PLDIBSection....\n";
  testCodecBmp (m_DIBSect32, Encoder, Decoder, "mytestbmp");
  cerr << "  Testing 8 bpp PLDIBSection....\n";
  testCodecBmp (m_DIBSect8, Encoder, Decoder, "mytestbmp");
#endif

  // Test bit-depth conversion.
  cerr << "  Testing bit-depth conversion....\n";
  PLAnyBmp NewBmp;
  const_cast<PLPicEncoder&>(Encoder).MakeFileFromBmp ("mytestbmp", &m_AnyBmp8);
  const_cast<PLPicDecoder&>(Decoder).MakeBmpFromFile ("mytestbmp", &NewBmp, PLPixelFormat::X8R8G8B8);
  PLAnyBmp Bmp32;
  Bmp32.CreateCopy (m_AnyBmp8, PLPixelFormat::X8R8G8B8);
  Test (NewBmp == Bmp32);
}

void PLTester::testCodecBmp (PLBmp& Bmp, const PLPicEncoder& Encoder,
                              const PLPicDecoder& Decoder, char * pszFName)
{
  // Test writing to file.
  PLAnyBmp NewBmp;
  // paintlib isn't const-correct yet :-(.
  const_cast<PLPicEncoder&>(Encoder).MakeFileFromBmp (pszFName, &Bmp);
  const_cast<PLPicDecoder&>(Decoder).MakeBmpFromFile (pszFName, &NewBmp);
  ::remove (pszFName);  // Delete file again.

  PLAnyBmp TempBmp;
  TempBmp.CreateCopy (NewBmp, Bmp.GetPixelFormat());
  NewBmp = TempBmp;

  Test (Bmp == NewBmp);

  // Test writing to memory.
  int Size = Bmp.GetWidth()*Bmp.GetHeight()*4+2000;
  PLMemSink MemSink;
  MemSink.Open (pszFName, Size);
  const_cast<PLPicEncoder&>(Encoder).SaveBmp (&Bmp, &MemSink);
  const_cast<PLPicDecoder&>(Decoder).MakeBmpFromMemory (
      MemSink.GetBytes(), Size, &Bmp);
  MemSink.Close();

  TempBmp.CreateCopy (NewBmp, Bmp.GetPixelFormat());
  NewBmp = TempBmp;

  Test (Bmp == NewBmp);
}

#ifdef PL_SUPPORT_TIFF
void PLTester::testCTIFFEx (PLBmp& Bmp)
{
  const char szFName[] = "mytestbmp";
  PLAnyBmp NewBmp;
  PLTIFFEncoderEx Encoder;
  PLFileSink Sink;
  Sink.Open (szFName, Bmp.GetMemUsed() + 4096);
  Encoder.Associate (&Sink );
  // mandatory 'base_line' tags
  Encoder.SetBaseTags( &Bmp );
  // one strip = the whole image (no strip, no tile)
  Encoder.SetField( TIFFTAG_ROWSPERSTRIP, Bmp.GetHeight());
  Encoder.SetField( TIFFTAG_DOCUMENTNAME, szFName);
  Encoder.SetField (TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
  Encoder.SaveBmp (&Bmp, &Sink);
  // only really needed if you encode several bitmaps
  Encoder.Dissociate ();
  Sink.Close();

  PLTIFFDecoder Decoder;
  Decoder.MakeBmpFromFile (szFName, &NewBmp);
  ::remove (szFName);  // Delete file again.
  Test (Bmp == NewBmp);
}
#endif


void PLTester::setUpBmp (PLBmp& Bmp, const PLPixelFormat & pf)
{
  Bmp.Create(5, 5, pf);
  Bmp.SetResolution (PLPoint (72, 72));
  PLBYTE ** ppLines = Bmp.GetLineArray();
  int y,x;
  for (y=0; y<5; y++)
    for (x=0; x<5; x++)
    {
      switch (pf.GetBitsPerPixel())
      {
        case 8:
          if (y<2)
            ppLines[y][x] = 255;
          else if (y<3)
            ppLines[y][x] = 128;
          else
            ppLines[y][x] = 0;
          break;
        case 16:
          {
            PLPixel16 * pPix;
            pPix = (PLPixel16*)&(ppLines[y][x*2]);
            if (y == 0)
              pPix->Set(255,255,255);
            else if (y == 1)
              pPix->Set(255,255,0);
            else if (y == 2 || y == 3)
              pPix->Set(255,0,0);
            else if (y == 4)
              pPix->Set(0,0,0);
          }
          break;
        case 24:
          {
            PLPixel24 * pPix;
            pPix = (PLPixel24*)&(ppLines[y][x*3]);
            if (y == 0)
              pPix->Set(255,255,255);
            else if (y == 1)
              pPix->Set(255,255,0);
            else if (y == 2 || y == 3)
              pPix->Set(255,0,0);
            else if (y == 4)
              pPix->Set(0,0,0);
          }
          break;
        case 32:
          {
            PLPixel32 * pPix;
            pPix = (PLPixel32*)&(ppLines[y][x*4]);
            if (y == 0)
              pPix->Set(255,255,255,0);
            else if (y == 1)
              pPix->Set(255,255,0,0);
            else if (y == 2 || y == 3)
              pPix->Set(255,0,0,0);
            else if (y == 4)
              pPix->Set(0,0,0,0);
          }
          break;
        default:
          PLASSERT (false);
      }
    }

  if (pf == PLPixelFormat::I8)
  {
    for (int i=0; i< 256; ++i)
    {
      Bmp.SetPaletteEntry (i, i,i,0,255);
    }
  }

}

void PLTester::setUp ()
{
  setUpBmp (m_AnyBmp32, PLPixelFormat::X8R8G8B8);
  setUpBmp (m_AnyBmp24, PLPixelFormat::R8G8B8);
  setUpBmp (m_AnyBmp16, PLPixelFormat::X1R5G5B5);
  setUpBmp (m_AnyBmp8, PLPixelFormat::I8);
#ifdef _WIN32
  setUpBmp (m_DIBSect32, PLPixelFormat::X8R8G8B8);
  setUpBmp (m_DIBSect8, PLPixelFormat::I8);
//  setUpBmp (m_DDBmp32, 32);
//  setUpBmp (m_DDBmp8, 8);
#endif
}

void PLTester::tearDown ()
{
}

bool PLTester::almostEqual (PLBmp& bmp1, PLBmp& bmp2, int epsilon)
{
  return bmp1.AlmostEqual (bmp2, epsilon);
}


/*
/--------------------------------------------------------------------
|
|      $Log: pltester.cpp,v $
|      Revision 1.74  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.73  2004/10/02 22:23:13  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.72  2004/09/21 13:03:05  uzadow
|      Reenabled testFlipRGB, added 32-bit version of PLFilterColorize.
|
|      Revision 1.71  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.70  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.69  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.68  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.67  2004/07/28 11:41:08  uzadow
|      PNG decoder now decodes directly to 24 bpp
|
|      Revision 1.66  2004/06/20 16:59:38  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.65  2004/06/19 16:49:11  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.64  2004/06/15 14:17:25  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.63  2004/06/15 11:18:17  uzadow
|      First working version of PLBmpBase.
|
|      Revision 1.62  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.61  2004/06/12 14:52:50  uzadow
|      Added CreateFromSurface function.
|
|      Revision 1.60  2004/06/09 21:34:57  uzadow
|      Added 16 bpp support to plbitmap, planybmp and pldirectfbbmp
|
|      Revision 1.59  2004/06/09 20:27:52  uzadow
|      Added 16 bpp pixel class.
|
|      Revision 1.58  2004/04/15 20:22:22  uzadow
|      - All test output goes to cerr now.
|      - Fixed behaviour when test images were not available.
|
|      Revision 1.57  2004/04/15 19:09:38  uzadow
|      - Moved TestBmpList to test source, where it is versioned.
|      - All test output goes to cerr now.
|
|      Revision 1.56  2004/04/15 16:39:30  uzadow
|      Initial Mac OS X port
|
|      Revision 1.55  2004/03/10 21:36:43  uzadow
|      pltester now has sensible output in non-debug mode.
|
|      Revision 1.54  2004/03/10 20:39:25  uzadow
|      Removed root access requirement
|
|      Revision 1.53  2004/02/21 16:48:24  uzadow
|      Fixed rect.Intersects()
|
|      Revision 1.52  2004/02/15 22:43:32  uzadow
|      Added 8-bit-support to DFBBitmap and PLFilterFill
|
|      Revision 1.51  2003/08/17 18:04:39  uzadow
|      Added PLRect::Intersect()
|
|      Revision 1.50  2003/08/17 15:04:31  uzadow
|      Added PLRect::Intersects() and Expand()
|
|      Revision 1.49  2003/08/03 16:41:45  uzadow
|      More bigendian combatibility changes.
|
|      Revision 1.48  2003/07/29 21:27:42  uzadow
|      Fixed PLDirectFBBmp::GetBytesPerLine(), im2* Makefiles
|
|      Revision 1.47  2003/07/27 13:50:49  uzadow
|      Added support for DirectFB surfaces.
|
|      Revision 1.46  2003/06/03 14:12:12  artcom
|      Only one test failure if test images aren't available.
|
|      Revision 1.45  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|      Revision 1.44  2003/04/13 20:13:21  uzadow
|      Added counted pointer classes (windows ver.)
|
|      Revision 1.43  2003/03/19 14:33:18  uzadow
|      Added Rect.Contains
|
|      Revision 1.42  2003/03/08 15:16:02  uzadow
|      Added PLSDLBmp::Attach().
|
|      Revision 1.41  2003/03/08 14:32:20  uzadow
|      Added support for bitmaps in SDL surface format.
|
|      Revision 1.40  2003/02/24 20:59:40  uzadow
|      Added windows url support, url support tests.
|
|      Revision 1.39  2002/11/27 22:32:22  uzadow
|      Now works if TIFF, PNG and/or JPEG support are not available
|
|      Revision 1.38  2002/11/18 14:45:40  uzadow
|      Added PNG compression support, fixed docs.
|
|      Revision 1.37  2002/10/20 22:11:53  uzadow
|      Partial doc update.
|      Fixed tests when images were not available.
|
|      Revision 1.36  2002/08/05 19:06:32  uzadow
|      no message
|
|      Revision 1.35  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.34  2002/03/29 19:19:11  uzadow
|      Added test for MakeBmpFromFile() with BPPWanted.
|
|      Revision 1.33  2002/03/06 23:11:15  uzadow
|      Fixed major PLAnyDec bug
|
|      Revision 1.32  2002/03/06 22:46:55  uzadow
|      Fixed major PLAnyDec bug
|
|      Revision 1.31  2002/03/03 16:29:56  uzadow
|      Re-added BPPWanted.
|
|      Revision 1.30  2002/03/03 11:59:43  uzadow
|      Updated to work with cygwin.
|
|      Revision 1.29  2002/02/24 13:24:17  uzadow
|      no message
|
|      Revision 1.28  2002/02/24 13:00:49  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.27  2002/01/27 18:20:18  uzadow
|      Updated copyright message; corrected pcx decoder bug.
|
|      Revision 1.26  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.25  2001/10/16 17:12:27  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.24  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.23  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.22  2001/10/06 15:32:22  uzadow
|      Removed types LPBYTE, DWORD, UCHAR, VOID and INT from the code.
|
|      Revision 1.21  2001/10/03 14:00:29  uzadow
|      Much improved quality in FilterResizeBilinear.
|
|      Revision 1.20  2001/09/30 19:55:37  uzadow
|      Fixed bug for 8 bpp in PLFilterCrop.
|
|      Revision 1.19  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.18  2001/09/24 14:19:19  uzadow
|      Added PLPoint & PLRect tests.
|
|      Revision 1.17  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.16  2001/09/15 20:39:57  uzadow
|      Added Quantization to user-defined palette (Andreas Köpf)
|
|      Revision 1.15  2001/09/15 14:30:20  uzadow
|      Fixed PLPixel32 initialization bug.
|
|      Revision 1.14  2001/09/13 20:49:31  uzadow
|      Added fill filters.
|
|      Revision 1.13  2001/02/04 14:07:26  uzadow
|      Changed max. filename length.
|
|      Revision 1.12  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.11  2001/01/13 20:06:16  uzadow
|      Added Flip and Mirror filters.
|
|      Revision 1.10  2001/01/13 00:28:03  uzadow
|      no message
|
|      Revision 1.9  2001/01/12 23:06:11  uzadow
|      Additional bmp assignment test.
|
|      Revision 1.8  2000/12/18 22:42:53  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.7  2000/12/09 12:16:26  uzadow
|      Fixed several memory leaks.
|
|      Revision 1.6  2000/12/05 00:12:18  uzadow
|      *** empty log message ***
|
|      Revision 1.5  2000/12/04 23:59:02  uzadow
|      Added filter tests.
|
|      Revision 1.4  2000/12/03 21:15:43  uzadow
|      Fixed png handling in configure; test/ adapted to linux
|
|      Revision 1.3  2000/12/02 19:42:29  uzadow
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
