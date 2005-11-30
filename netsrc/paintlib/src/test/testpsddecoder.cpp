/*
/--------------------------------------------------------------------
|
|      $Id: testpsddecoder.cpp,v 1.13 2004/09/11 12:41:36 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "testpsddecoder.h"
#include "pltester.h"

#include "plpsddec.h"
#include "planybmp.h"
#include "plpngenc.h"

#include <string>
#include <iostream>

using namespace std;

PLTestPSDDecoder::PLTestPSDDecoder ()
{

}

PLTestPSDDecoder::~PLTestPSDDecoder ()
{
}

void PLTestPSDDecoder::RunTests ()
{
  test (string ("rgb8.psd"));
  test (string ("rgb8alpha.psd"));
  test (string ("rgb24.psd"));
  test (string ("rgb24alpha.psd"));
  test (string ("rgb24alpha2.psd"));
  test (string ("rgb24layers.psd"));
}

void PLTestPSDDecoder::test (const string& sFName)
{
  PLPSDDecoder Decoder;
  PLAnyBmp Bmp;
  string sFilePos = PLTester::m_sTestFileDir+sFName;
  cerr << "  Testing " << sFilePos << "...\n";
  cerr << "    Testing MakeBmpFromFile...\n";
  Decoder.MakeBmpFromFile (sFilePos.c_str(), &Bmp);

  {
    PLPSDDecoder Decoder;
//    PLPNGEncoder Encoder;
    cerr << "    Testing OpenFile...\n";
    Decoder.OpenFile (sFilePos.c_str());
    int n = Decoder.GetNumLayers ();
    for (int i = 0; i<n; i++)
    {
      PLAnyBmp LayerBmp;
      cerr << "    Testing GetNextLayer...\n";
      Decoder.GetNextLayer (LayerBmp);
      PLPoint Pt = Decoder.GetLayerOffset ();
/*
      char sz[256];
      sprintf (sz, "c:\\test%i.png", i);
      Encoder.MakeFileFromBmp (sz, &LayerBmp);
*/
    }
    PLAnyBmp FlatBmp;
    FlatBmp.Create (Decoder);
    cerr << "    Testing GetImage...\n";
    Decoder.GetImage (FlatBmp);
    PLASSERT (FlatBmp == Bmp);
    Decoder.Close ();
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: testpsddecoder.cpp,v $
|      Revision 1.13  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.12  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.11  2004/04/15 19:09:38  uzadow
|      - Moved TestBmpList to test source, where it is versioned.
|      - All test output goes to cerr now.
|
|      Revision 1.10  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|      Revision 1.9  2002/11/18 14:45:51  uzadow
|      Added PNG compression support, fixed docs.
|
|      Revision 1.8  2002/10/20 22:11:53  uzadow
|      Partial doc update.
|      Fixed tests when images were not available.
|
|      Revision 1.7  2002/08/07 20:39:54  uzadow
|      no message
|
|      Revision 1.6  2002/08/07 20:32:31  uzadow
|      Updated Makefile.in, fixed psd decoder layer stuff.
|
|      Revision 1.5  2002/08/07 18:50:39  uzadow
|      More cygwin compatibility changes.
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/01/27 18:20:18  uzadow
|      Updated copyright message; corrected pcx decoder bug.
|
|      Revision 1.2  2001/10/21 17:54:40  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|
\--------------------------------------------------------------------
*/

