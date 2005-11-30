/*
/--------------------------------------------------------------------
|
|      $Id: pltestexif.cpp,v 1.12 2004/09/11 14:15:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pltestexif.h"
#include "pltester.h"

#include <iostream>
#include <fstream>

#include "plpaintlibdefs.h"
#include "pljpegdec.h"
#include "pljpegenc.h"

#include "plexif.h"

using namespace std;

PLTestExif::PLTestExif ()
{
}

PLTestExif::~PLTestExif ()
{
}

void PLTestExif::RunTests ()
{
  cerr << "Running exif decoding tests...\n";
/*  testImage("143_jpg.jpg", "", 0);
  testImage("canon-ixus.jpg", "Canon", 60);
  testImage("fujifilm-dx10.jpg", "FUJIFILM", 38);
*/
  testImage("fujifilm-finepix40i.jpg", "FUJIFILM", 54);
/*
  testImage("fujifilm-mx1700.jpg", "FUJIFILM", 38);
  testImage("kodak-dc210.jpg", "Eastman Kodak Company", 25);
  testImage("kodak-dc240.jpg", "EASTMAN KODAK COMPANY", 33);
  testImage("nikon-e950.jpg", "NIKON", 46);
  testImage("olympus-c960.jpg", "OLYMPUS OPTICAL CO.,LTD", 43);
  testImage("ricoh-rdc5300.jpg", "RICOH      ", 30);
  testImage("sanyo-vpcg250.jpg", "SANYO Electric Co.,Ltd.", 31);
  testImage("sanyo-vpcsx550.jpg", "SANYO Electric Co.,Ltd.", 33);
  testImage("sony-cybershot.jpg", "SONY", 32);
  testImage("sony-d700.jpg", "SONY", 26);
  testImage("../../more_testpics/exif/exif_error.jpg", "", 0);
*/
  testEncode();
}

void PLTestExif::testImage(const string& sFName, const string& sMakeExpected, int NumTags)
{
  string sCompleteFName = PLTester::m_sTestFileDir+sFName;
  cerr << "  Testing " << sFName.c_str() << "...\n";

  PLJPEGDecoder Decoder;
  try
  {
    PLAnyBmp Bmp;
    Decoder.OpenFile (sCompleteFName.c_str());
    PLExif ExifData;
    Decoder.GetExifData(ExifData);
    const PLExifTagList & ev = ExifData.GetAllTags();
    cerr << "    ev.size: " << ev.size() << ", NumTags: " << NumTags << endl;
    string sMake;
    ExifData.GetTagCommon("Main.Make", sMake);
    Test (sMake == sMakeExpected);

    Decoder.MakeBmp(&Bmp);
    Decoder.Close();
  }
  catch (PLTextException e)
  {
    if (e.GetCode() == PL_ERRFORMAT_NOT_COMPILED)
    {
      cerr << "      --> Test skipped.\n";
    }
    else
    {
      Decoder.Close();
      cerr << "    Caught exception: " << (const char *)e << endl;
      Test(false);
    }
  }
  cerr << endl;
}

void PLTestExif::testEncode()
{
  PLJPEGDecoder Decoder;
  PLJPEGEncoder Encoder;
  cerr << "  Testing encode...\n";
  try
  {
    PLAnyBmp Bmp;
    Decoder.OpenFile ((PLTester::m_sTestFileDir+"fujifilm-finepix40i.jpg").c_str());
    PLExif ExifData;
    Decoder.GetExifData(ExifData);
    const PLExifTagList & ev = ExifData.GetAllTags();
    int NumTags = ev.size();
    cerr << "    NumTags !=0...\n";
    Test(NumTags != 0);
    Decoder.MakeBmp(&Bmp);
    Decoder.Close();

    Encoder.SetExifData(ExifData);
    Encoder.MakeFileFromBmp("test.jpg", &Bmp);
    
    Decoder.OpenFile ("test.jpg");
    Decoder.GetExifData(ExifData);
    Test(NumTags == ExifData.GetAllTags().size());
  }
  catch (PLTextException e)
  {
    if (e.GetCode() == PL_ERRFORMAT_NOT_COMPILED)
    {
      cerr << "      --> Test skipped.\n";
    }
    else
    {
      Decoder.Close();
      Test(false);
    }
  }

}

/*
/--------------------------------------------------------------------
|
|      $Log: pltestexif.cpp,v $
|      Revision 1.12  2004/09/11 14:15:38  uzadow
|      Comitted testimages, resized most of them.
|
|      Revision 1.11  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.10  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.9  2004/04/15 20:22:22  uzadow
|      - All test output goes to cerr now.
|      - Fixed behaviour when test images were not available.
|
|      Revision 1.8  2004/04/15 19:09:38  uzadow
|      - Moved TestBmpList to test source, where it is versioned.
|      - All test output goes to cerr now.
|
|      Revision 1.7  2004/03/12 10:31:26  uzadow
|      More robust handling of broken exif data in otherwise correct jpegs.
|
|      Revision 1.6  2003/08/03 16:41:45  uzadow
|      More bigendian combatibility changes.
|
|      Revision 1.5  2003/04/19 19:03:59  uzadow
|      Exif save (windows)
|
|      Revision 1.4  2003/04/19 12:36:03  uzadow
|      Only one exif testpic.
|
|      Revision 1.3  2003/04/18 20:50:24  uzadow
|      no message
|
|      Revision 1.2  2003/04/18 20:35:44  uzadow
|      Linux exif changes, pt 2
|
|      Revision 1.1  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|      Revision 1.1  2003/04/13 20:13:21  uzadow
|      Added counted pointer classes (windows ver.)
|
|
|
\--------------------------------------------------------------------
*/
