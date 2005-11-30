/*
/--------------------------------------------------------------------
|
|      $Id: testfilterresizebilinear.cpp,v 1.11 2005/03/08 14:22:46 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "testfilterresizebilinear.h"
#include "pltester.h"

#include "plpaintlibdefs.h"
#include "planybmp.h"
#include "plpixel32.h"

#include "Filter/plfilterresizebilinear.h"

#include <iostream>
#include <string>
#include <fstream>

#include "plpngenc.h"

using namespace std;

PLTestFilterResizeBilinear::PLTestFilterResizeBilinear ()
{
}

PLTestFilterResizeBilinear::~PLTestFilterResizeBilinear ()
{
}

void PLTestFilterResizeBilinear::RunTests ()
{
  PLAnyBmp AnyBmp32;
  createBmp (AnyBmp32);

  cerr << "  Testing bilinear resize...\n";
  cerr << "    32 bpp...\n";
  PLAnyBmp DestBmp1;
  PLAnyBmp DestBmp2;
  DestBmp1.CreateFilteredCopy (AnyBmp32, PLFilterResizeBilinear (4,4));
  DestBmp2.CreateFilteredCopy (DestBmp1, PLFilterResizeBilinear (8,8));
  Test (AnyBmp32.AlmostEqual (DestBmp2, 1));
/*
  DestBmp1.CreateFilteredCopy (AnyBmp32, PLFilterResizeBilinear (2,2));
  DestBmp2.CreateFilteredCopy (DestBmp1, PLFilterResizeBilinear (8,8));
  m_pTester->Test (AnyBmp32.AlmostEqual (DestBmp2, 5));
*/
  DestBmp1.CreateFilteredCopy (AnyBmp32, PLFilterResizeBilinear (16,16));
  DestBmp2.CreateFilteredCopy (DestBmp1, PLFilterResizeBilinear (8,8));
  Test (AnyBmp32.AlmostEqual (DestBmp2, 1));

  cerr << "    24 bpp...\n";
  PLAnyBmp AnyBmp24;
  AnyBmp24.CreateCopy(AnyBmp32, PLPixelFormat::R8G8B8);
  DestBmp1.CreateFilteredCopy (AnyBmp24, PLFilterResizeBilinear (16,16));
  DestBmp2.CreateFilteredCopy (DestBmp1, PLFilterResizeBilinear (8,8));
  Test (AnyBmp24.AlmostEqual (DestBmp2, 1));

  cerr << "    8 bpp...\n";
  PLAnyBmp AnyBmp8;
  createMonoBmp(AnyBmp8);
  //AnyBmp8.CreateCopy(AnyBmp32, PLPixelFormat::L8);
  DestBmp1.CreateFilteredCopy (AnyBmp8, PLFilterResizeBilinear (16,16));
  DestBmp2.CreateFilteredCopy (DestBmp1, PLFilterResizeBilinear (8,8));
  Test (AnyBmp8.AlmostEqual (DestBmp2, 1));
}

void PLTestFilterResizeBilinear::createMonoBmp(PLBmp& Bmp)
{
  Bmp.Create (8, 8, PLPixelFormat::L8);

  int x,y;
  for (y=0; y<4; y++)
      for (x=0; x<4; x++)
          Bmp.SetPixel(x,y, PLPixel8(255));
  for (y=0; y<4; y++)
    for (x=4; x<8; x++)
      Bmp.SetPixel (x,y, PLPixel8 (128));

  for (y=4; y<8; y++)
    for (x=0; x<4; x++)
      Bmp.SetPixel (x,y, PLPixel8 (34));

  for (y=4; y<8; y++)
    for (x=4; x<8; x++)
      Bmp.SetPixel (x,y, PLPixel8 (200));
}



void PLTestFilterResizeBilinear::createBmp(PLBmp& Bmp)
{
  Bmp.Create (8, 8, PLPixelFormat::X8R8G8B8);

  int x,y;
  for (y=0; y<4; y++)
    for (x=0; x<4; x++)
      Bmp.SetPixel (x,y, PLPixel32 (255,0,0,0));

  for (y=0; y<4; y++)
    for (x=4; x<8; x++)
      Bmp.SetPixel (x,y, PLPixel32 (0,255,0,0));

  for (y=4; y<8; y++)
    for (x=0; x<4; x++)
      Bmp.SetPixel (x,y, PLPixel32 (0,0,255,0));

  for (y=4; y<8; y++)
    for (x=4; x<8; x++)
      Bmp.SetPixel (x,y, PLPixel32 (255,255,255,0));
}

/*
/--------------------------------------------------------------------
|
|      $Log: testfilterresizebilinear.cpp,v $
|      Revision 1.11  2005/03/08 14:22:46  artcom
|      Added resizing of Luminace Bitmaps
|
|      Revision 1.10  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.9  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.8  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.7  2004/07/28 13:55:56  artcom
|      Added 24 bpp support to plfilterresizebilinear.
|
|      Revision 1.6  2004/04/15 20:22:22  uzadow
|      - All test output goes to cerr now.
|      - Fixed behaviour when test images were not available.
|
|      Revision 1.5  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.4  2002/01/27 18:20:18  uzadow
|      Updated copyright message; corrected pcx decoder bug.
|
|      Revision 1.3  2001/10/06 15:32:22  uzadow
|      Removed types LPBYTE, DWORD, UCHAR, VOID and INT from the code.
|
|      Revision 1.2  2001/10/05 21:15:09  uzadow
|      Improved support for OS/2 bitmap decoding.
|
|      Revision 1.1  2001/10/03 14:00:29  uzadow
|      Much improved quality in FilterResizeBilinear.
|
|
\--------------------------------------------------------------------
*/
