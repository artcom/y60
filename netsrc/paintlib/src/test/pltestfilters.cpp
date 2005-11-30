/*
/--------------------------------------------------------------------
|
|      $Id: pltestfilters.cpp,v 1.14 2004/10/02 22:23:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pltestfilters.h"
#include "plpaintlibdefs.h"
#include "planybmp.h"
#include "plpngenc.h"

#include "plpixel8.h"

#include "Filter/plfilter.h"
#include "Filter/plfiltercontrast.h"
#include "Filter/plfiltercrop.h"
#include "Filter/plfilterintensity.h"
#include "Filter/plfilterlightness.h"
#include "Filter/plfilterquantize.h"
#include "Filter/plfilterresizebilinear.h"
#include "Filter/plfilterresizegaussian.h"
#include "Filter/plfilterflip.h"
#include "Filter/plfiltermirror.h"
#include "Filter/plfilterfillrect.h"
#include "Filter/plfilterfill.h"
#include "Filter/plfilterrotate.h"
#include "Filter/plfiltercolorize.h"
#include "Filter/plfilterfliprgb.h"

#include "testfilterresizebilinear.h"

#include <iostream>

using namespace std;

PLTestFilters::PLTestFilters(PLBmp* pBmp32, PLBmp* pBmp24, PLBmp* pBmp16, PLBmp* pBmp8)
  : m_pBmp32 (pBmp32),
    m_pBmp24 (pBmp24),
    m_pBmp16 (pBmp16),
    m_pBmp8 (pBmp8)
{
}

PLTestFilters::~PLTestFilters(void)
{
}

void PLTestFilters::RunTests ()
{
  cerr << "  Testing contrast...\n";
  testFilterWithBmp (PLFilterContrast (1, 128), *m_pBmp32);

  cerr << "  Testing crop...\n";
  cerr << "    32 bpp...\n";
  testFilterWithBmp (PLFilterCrop (0,0,5,5), *m_pBmp32);
  cerr << "    24 bpp...\n";
  testFilterWithBmp (PLFilterCrop (0,0,5,5), *m_pBmp24);
  cerr << "    16 bpp...\n";
  testFilterWithBmp (PLFilterCrop (0,0,5,5), *m_pBmp16);
  cerr << "    8 bpp...\n";
  testFilterWithBmp (PLFilterCrop (0,0,5,5), *m_pBmp8);

  cerr << "  Testing intensity...\n";
  testFilterWithBmp (PLFilterIntensity (20, 128, 1), *m_pBmp32);

  cerr << "  Testing lightness...\n";
  testFilterWithBmp (PLFilterLightness (0), *m_pBmp32);

  cerr << "  Testing quantization...\n";
  cerr << "    Testing median cut...\n";
  testQuant (PLDTHPAL_MEDIAN, PLDTH_NONE);
  cerr << "    Testing popularity sort...\n";
  testQuant (PLDTHPAL_POPULARITY, PLDTH_NONE);
  cerr << "    Testing default palette...\n";
  testQuant (PLDTHPAL_DEFAULT, PLDTH_NONE);
  cerr << "    Testing user-defined palette...\n";
  testQuant (PLDTHPAL_USERDEFINED, PLDTH_NONE);
  cerr << "    Testing ordered dithering...\n";
  testQuant (PLDTHPAL_MEDIAN, PLDTH_ORDERED);
  cerr << "    Testing floyd-steinberg dithering...\n";
  testQuant (PLDTHPAL_MEDIAN, PLDTH_FS);

  {
    PLTestFilterResizeBilinear ResizeTester;
    ResizeTester.RunTests();
  }

  cerr << "  Testing gauss filter...\n";
  testFilterWithBmp (PLFilterResizeGaussian (5,5,0.2), *m_pBmp32);

  cerr << "  Testing flip...\n";
  testDoubleFilter (PLFilterFlip());

  cerr << "  Testing mirror...\n";
  testDoubleFilter (PLFilterMirror());

  cerr << "  Testing fillrect...\n";
  testFillRect ();

  cerr << "  Testing fill...\n";
  testFill ();

  cerr << "  Testing rotate...\n";
  testRotate ();

  cerr << "  Testing colorize..\n";
  testColorize ();

  cerr << "  Testing rgb flip...\n";
  testFlipRGB ();
}

void PLTestFilters::testFilterWithBmp (const PLFilter& Filter, PLBmp& SrcBmp)
{
  PLAnyBmp DestBmp;
  DestBmp.CreateFilteredCopy (SrcBmp, Filter);
  Test (SrcBmp.AlmostEqual (DestBmp, 4));

  DestBmp = SrcBmp;
  DestBmp.ApplyFilter (Filter);
  Test (SrcBmp.AlmostEqual (DestBmp, 8));
}

void PLTestFilters::testDoubleFilter (const PLFilter& Filter)
{
  PLAnyBmp DestBmp1;
  PLAnyBmp DestBmp2;
  cerr << "    32 bpp...\n";
  const_cast<PLFilter&>(Filter).Apply (m_pBmp32, &DestBmp1);
  const_cast<PLFilter&>(Filter).Apply (&DestBmp1, &DestBmp2);
  Test (m_pBmp32->AlmostEqual (DestBmp2, 4));
  cerr << "    24 bpp...\n";
  const_cast<PLFilter&>(Filter).Apply (m_pBmp24, &DestBmp1);
  const_cast<PLFilter&>(Filter).Apply (&DestBmp1, &DestBmp2);
  Test (m_pBmp24->AlmostEqual (DestBmp2, 4));
  cerr << "    16 bpp...\n";
  const_cast<PLFilter&>(Filter).Apply (m_pBmp16, &DestBmp1);
  const_cast<PLFilter&>(Filter).Apply (&DestBmp1, &DestBmp2);
  Test (m_pBmp16->AlmostEqual (DestBmp2, 4));
  cerr << "    8 bpp...\n";
  const_cast<PLFilter&>(Filter).Apply (m_pBmp8, &DestBmp1);
  const_cast<PLFilter&>(Filter).Apply (&DestBmp1, &DestBmp2);
  Test (m_pBmp8->AlmostEqual (DestBmp2, 4));
}


void PLTestFilters::testQuant (int DthPal, int Dth)
{
  PLAnyBmp DestBmp8;
  PLAnyBmp DestBmp32;
  PLFilterQuantize Q (DthPal, Dth);
  if (DthPal == PLDTHPAL_USERDEFINED)
    Q.SetUserPalette(PLFilterQuantize::GetDefaultPalette ());
  Q.Apply(m_pBmp32, &DestBmp8);
  DestBmp32.CreateCopy (DestBmp8, PLPixelFormat::X8R8G8B8);
  Test (m_pBmp32->AlmostEqual (DestBmp32, 4));
}

void PLTestFilters::testFillRect ()
{
  PLAnyBmp Bmp8;
  Bmp8.Create (4, 4, PLPixelFormat::L8);
  Bmp8.ApplyFilter (PLFilterFillRect<PLPixel8> (PLPoint (0, 0), PLPoint (4, 4), PLPixel8(0)));
  Bmp8.ApplyFilter (PLFilterFillRect<PLPixel8> (PLPoint (1, 1), PLPoint (3, 3), PLPixel8(255)));
  PLPixel8 ** ppLines = (PLPixel8**)Bmp8.GetLineArray();
  for (int i=0; i<4; ++i)
  {
    PLASSERT (ppLines[0][i] == 0);
    PLASSERT (ppLines[3][i] == 0);
    PLASSERT (ppLines[i][0] == 0);
    PLASSERT (ppLines[i][3] == 0);
  }
  PLASSERT (ppLines[1][1] == 255);
  PLASSERT (ppLines[1][2] == 255);
  PLASSERT (ppLines[2][1] == 255);
  PLASSERT (ppLines[2][2] == 255);

  PLFilterFillRect<PLPixel32> Filter32(PLPoint (0, 0), PLPoint (15, 15), PLPixel32(0,0,0,0));
}

void PLTestFilters::testFill ()
{
  PLAnyBmp Bmp;
  Bmp.Create (5, 5, PLPixelFormat::A8R8G8B8);
  PLPixel32 color(1,2,3,4);
  Bmp.ApplyFilter (PLFilterFill<PLPixel32>(color));
  PLBYTE ** ppLines = Bmp.GetLineArray();
  for (int y=0; y<5; y++)
    for (int x=0; x<5; x++)
    {
      PLPixel32 * pPix = (PLPixel32*)&(ppLines[y][x*4]);
      PLASSERT (*pPix == color);
    }
}

void PLTestFilters::testRotate ()
{
  PLAnyBmp Bmp = *m_pBmp32;
  m_pBmp32->ApplyFilter(PLFilterRotate(PLFilterRotate::ninety));
  m_pBmp32->ApplyFilter(PLFilterRotate(PLFilterRotate::twoseventy));
  PLASSERT(Bmp == *m_pBmp32);
  
  m_pBmp32->ApplyFilter(PLFilterRotate(PLFilterRotate::oneeighty));
  m_pBmp32->ApplyFilter(PLFilterRotate(PLFilterRotate::oneeighty));
  PLASSERT(Bmp == *m_pBmp32);
}

void PLTestFilters::testColorize ()
{
  PLAnyBmp Bmp;
  Bmp.Create(5, 5, PLPixelFormat::R8G8B8);
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel24> (PLPoint (0, 0), PLPoint (5, 1), PLPixel24(0,0,0)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel24> (PLPoint (0, 1), PLPoint (5, 2), PLPixel24(64,64,64)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel24> (PLPoint (0, 2), PLPoint (5, 3), PLPixel24(128,128,128)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel24> (PLPoint (0, 3), PLPoint (5, 4), PLPixel24(192,192,192)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel24> (PLPoint (0, 4), PLPoint (5, 5), PLPixel24(255,255,255)));
  PLAnyBmp Bmp24;
  Bmp24.CreateCopy(Bmp, PLPixelFormat::R8G8B8);
  
  Bmp.ApplyFilter(PLFilterColorize(195, 1));
  Bmp.ApplyFilter(PLFilterColorize(0,0));
  PLASSERT(Bmp.AlmostEqual(Bmp24,8));

  Bmp.Create(5, 5, PLPixelFormat::X8R8G8B8);
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel32> (PLPoint (0, 0), PLPoint (5, 1), PLPixel32(0,0,0,0)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel32> (PLPoint (0, 1), PLPoint (5, 2), PLPixel32(64,64,64,0)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel32> (PLPoint (0, 2), PLPoint (5, 3), PLPixel32(128,128,128,0)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel32> (PLPoint (0, 3), PLPoint (5, 4), PLPixel32(192,192,192,0)));
  Bmp.ApplyFilter (PLFilterFillRect<PLPixel32> (PLPoint (0, 4), PLPoint (5, 5), PLPixel32(255,255,255,0)));
  PLAnyBmp Bmp32;
  Bmp32.CreateCopy(Bmp, PLPixelFormat::X8R8G8B8);
  
  Bmp.ApplyFilter(PLFilterColorize(195, 1));
  Bmp.ApplyFilter(PLFilterColorize(0,0));
  PLASSERT(Bmp.AlmostEqual(Bmp32,8));
}

void PLTestFilters::testFlipRGB ()
{
  PLAnyBmp Bmp = *m_pBmp32;
  m_pBmp32->ApplyFilter(PLFilterFlipRGB());
  m_pBmp32->ApplyFilter(PLFilterFlipRGB());
  PLASSERT(Bmp == *m_pBmp32);

  Bmp = *m_pBmp24;
  m_pBmp24->ApplyFilter(PLFilterFlipRGB());
  m_pBmp24->ApplyFilter(PLFilterFlipRGB());
  PLASSERT(Bmp == *m_pBmp24);
}

/*
/--------------------------------------------------------------------
|
|      $Log: pltestfilters.cpp,v $
|      Revision 1.14  2004/10/02 22:23:13  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.13  2004/09/21 13:03:05  uzadow
|      Reenabled testFlipRGB, added 32-bit version of PLFilterColorize.
|
|      Revision 1.12  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.11  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.10  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.9  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.8  2004/08/04 16:48:02  uzadow
|      Fixed bug in hls2rgb.
|
|      Revision 1.7  2004/08/04 14:53:36  uzadow
|      Added PLFilterColorize.
|
|      Revision 1.6  2004/04/15 19:09:38  uzadow
|      - Moved TestBmpList to test source, where it is versioned.
|      - All test output goes to cerr now.
|
|      Revision 1.5  2004/02/15 22:43:32  uzadow
|      Added 8-bit-support to DFBBitmap and PLFilterFill
|
|      Revision 1.4  2003/11/05 15:17:26  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.3  2003/07/27 18:08:38  uzadow
|      Added plfilterfliprgb
|
|      Revision 1.2  2003/02/27 22:46:59  uzadow
|      Added plfilterrotate.
|
|      Revision 1.1  2002/08/04 20:12:34  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|
|
|
\--------------------------------------------------------------------
*/
