/*
/--------------------------------------------------------------------
|
|      $Id: pltestpixelformat.cpp,v 1.3 2004/10/02 22:23:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pltestpixelformat.h"
#include "plpixelformat.h"
#include "pltester.h"

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

PLTestPixelFormat ::PLTestPixelFormat  ()
{
}

PLTestPixelFormat ::~PLTestPixelFormat  ()
{
}

void PLTestPixelFormat ::RunTests ()
{
  cerr << "  Testing PixelFormat L16" << endl;
  cerr << "    GetBitsPerPixel..." << endl;
  Test(PLPixelFormat::L16.GetBitsPerPixel()==16);
  cerr << "    GetMask..." << endl;
  Test(PLPixelFormat::L16.GetMask(PLPixelFormat::L)==0x0000ffff);
  cerr << "    HasAlpha..." << endl;
  Test(PLPixelFormat::L16.HasAlpha()==false);
  cerr << "    IsGrayscale..." << endl;
  Test(PLPixelFormat::L16.IsGrayscale()==true);
  cerr << "    GetName..." << endl;
  Test(PLPixelFormat::L16.GetName()=="L16");

  cerr << "  Testing PixelFormat R8G8B8..." << endl;
  cerr << "    GetBitsPerPixel..." << endl;
  Test(PLPixelFormat::R8G8B8.GetBitsPerPixel()==24);
  cerr << "    GetMask..." << endl;
  Test(PLPixelFormat::R8G8B8.GetMask(PLPixelFormat::R)==0x00ff0000);
  Test(PLPixelFormat::R8G8B8.GetMask(PLPixelFormat::G)==0x0000ff00);
  Test(PLPixelFormat::R8G8B8.GetMask(PLPixelFormat::B)==0x000000ff);
  cerr << "    HasAlpha..." << endl;
  Test(PLPixelFormat::R8G8B8.HasAlpha()==false);
  cerr << "    IsGrayscale..." << endl;
  Test(PLPixelFormat::R8G8B8.IsGrayscale()==false);
  cerr << "    GetName..." << endl;
  Test(PLPixelFormat::R8G8B8.GetName()=="R8G8B8");
  Test(PLPixelFormat::R8G8B8.GetRGBSwapped()==PLPixelFormat::B8G8R8);

  cerr << "  Testing PixelFormat A8R8G8B8..." << endl;
  cerr << "    GetBitsPerPixel..." << endl;
  Test(PLPixelFormat::A8R8G8B8.GetBitsPerPixel()==32);
  cerr << "    GetMask..." << endl;
  Test(PLPixelFormat::A8R8G8B8.GetMask(PLPixelFormat::A)==0xff000000);
  Test(PLPixelFormat::A8R8G8B8.GetMask(PLPixelFormat::R)==0x00ff0000);
  Test(PLPixelFormat::A8R8G8B8.GetMask(PLPixelFormat::G)==0x0000ff00);
  Test(PLPixelFormat::A8R8G8B8.GetMask(PLPixelFormat::B)==0x000000ff);
  cerr << "    HasAlpha..." << endl;
  Test(PLPixelFormat::A8R8G8B8.HasAlpha()==true);
  cerr << "    IsGrayscale..." << endl;
  Test(PLPixelFormat::A8R8G8B8.IsGrayscale()==false);
  cerr << "    GetName..." << endl;
  Test(PLPixelFormat::A8R8G8B8.GetName()=="A8R8G8B8");
  Test(PLPixelFormat::A8R8G8B8.GetRGBSwapped()==PLPixelFormat::A8B8G8R8);

  cerr << "  Testing PixelFormat B16G16R16..." << endl;
  cerr << "    GetBitsPerPixel..." << endl;
  Test(PLPixelFormat::B16G16R16.GetBitsPerPixel()==48);
  cerr << "    GetMask..." << endl;
  Test(PLPixelFormat::B16G16R16.GetMask(PLPixelFormat::R)==0x000000000000ffff);
  Test(PLPixelFormat::B16G16R16.GetMask(PLPixelFormat::G)==0x00000000ffff0000);
  Test(PLPixelFormat::B16G16R16.GetMask(PLPixelFormat::B)==
       ((PLPixelFormat::Mask)0xffff) << 32);  
  cerr << "    HasAlpha..." << endl;
  Test(PLPixelFormat::B16G16R16.HasAlpha()==false);
  cerr << "    IsGrayscale..." << endl;
  Test(PLPixelFormat::B16G16R16.IsGrayscale()==false);
  cerr << "    GetName..." << endl;
  Test(PLPixelFormat::B16G16R16.GetName()=="B16G16R16");

  cerr << "  Testing PixelFormat X1R5G5B5..." << endl;
  cerr << "    GetBitsPerPixel..." << endl;
  Test(PLPixelFormat::X1R5G5B5.GetBitsPerPixel()==16);
  cerr << "    GetMask..." << endl;
  Test(PLPixelFormat::X1R5G5B5.GetMask(PLPixelFormat::R)==0x1f << 10);
  Test(PLPixelFormat::X1R5G5B5.GetMask(PLPixelFormat::G)==0x1f << 5);
  Test(PLPixelFormat::X1R5G5B5.GetMask(PLPixelFormat::B)==0x1f);  
  cerr << "    HasAlpha..." << endl;
  Test(PLPixelFormat::X1R5G5B5.HasAlpha()==false);
  cerr << "    IsGrayscale..." << endl;
  Test(PLPixelFormat::X1R5G5B5.IsGrayscale()==false);
  cerr << "    GetName..." << endl;
  Test(PLPixelFormat::X1R5G5B5.GetName()=="X1R5G5B5");

  cerr << "  Testing fromChannels RGB, 24..." << endl;
  Test(PLPixelFormat::FromChannels("RGB", 24) == PLPixelFormat::R8G8B8);

  cerr << "  Testing fromChannels YCrCb, 8..." << endl;
  bool bOkay = false;
  try {
    PLPixelFormat::FromChannels("YCrCb", 8);
  } catch(PLPixelFormat::UnsupportedPixelFormat&) {
    bOkay = true;
  }
  Test(bOkay);

  cerr << "  Testing fromChannels YCrCb, 8..." << endl;
}

/*
/--------------------------------------------------------------------
|
|      $Log: pltestpixelformat.cpp,v $
|      Revision 1.3  2004/10/02 22:23:13  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.2  2004/09/15 15:26:23  uzadow
|      Linux compatibility changes, doc update.
|
|      Revision 1.1  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.1  2003/04/13 20:13:21  uzadow
|      Added counted pointer classes (windows ver.)
|
|
|
\--------------------------------------------------------------------
*/
