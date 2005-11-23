//=============================================================================
//
// Copyright (C) 2000-2004, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.

#include "plfiltersubtract.h"
#include "plfilter3x3.h"

#include <asl/Arguments.h>
#include <asl/file_functions.h>
#include <asl/directory_functions.h>
#include <asl/Logger.h>

#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>

using namespace std;
using namespace asl;

////////////////////////////////////////////////////////////////////////
//
// For file format specification see som/Movie.h
//
////////////////////////////////////////////////////////////////////////

void printVersion();

asl::Arguments::AllowedOption myOptions[] = {
                                             {"--tolerance", "%f"},
                                             {"--threshold", "%s"},
                                             {"--version",   ""  },
                                             {"--help",      ""  },
                                             {"", ""}
                                            };
asl::Arguments myArguments(myOptions);

void
printHelp() {
    // TODO: Please help to maintain this function
    myArguments.printUsage();
    cout << "Command line options:" << endl
         << "  --tolerance     maximum number of 'bright' pixels in the difference bitmap allowed (in %)" << endl
         << "  --threshold     when a pixel is considered 'bright' (0..255)" << endl
         << "  --version       print version information and exit" << endl
         << "  --help          print this help text and exit" << endl
         << endl;
}

int main( int argc, char *argv[])  {
    string myArgDesc = string("image1 image2\nSee '") + string(getFilenamePart(argv[0])) +
                              " --help' for more information.";
    myArguments.setArgumentDescription(myArgDesc.c_str());

    if (!myArguments.parse( argc, argv )) {
        return 1;
    }
    if (myArguments.haveOption("--help")) {
        printHelp();
        return 0;
    }
    if (myArguments.haveOption("--version")) {
        printVersion();
        return 0;
    }

    if ( myArguments.getCount() != 2) {
        string myErrorMsg = string("Please specify two images to compare.");
        perror(myErrorMsg.c_str());
        return 1;
    }

    try {

        PLAnyPicDecoder myDecoder;
        PLPNGEncoder Encoder;

        PLAnyBmp myFirstBitmap;
        AC_INFO << "loading '" << myArguments.getArgument(0) << endl;
        myDecoder.MakeBmpFromFile(myArguments.getArgument(0).c_str(), &myFirstBitmap, PLPixelFormat::A8R8G8B8);
        AC_INFO << "loading '" << myArguments.getArgument(1) << endl;
        PLAnyBmp mySecondBitmap;
        myDecoder.MakeBmpFromFile(myArguments.getArgument(1).c_str(), &mySecondBitmap, PLPixelFormat::A8R8G8B8);

        string myBaseName = asl::removeExtension(myArguments.getArgument(0));  
        float myTolerance = 0.0f;
        int myThreshold = 128;

        if ( myArguments.haveOption("--tolerance")) {
            myTolerance = asl::as<float>(myArguments.getOptionArgument("--tolerance"));
        }

        if ( myArguments.haveOption("--threshold")) {
            myThreshold = asl::as<int>(myArguments.getOptionArgument("--threshold"));
        }
        AC_INFO << "Subtracting: " << endl;
        PLFilterSubtract mySubtractionFilter(&mySecondBitmap);
        myFirstBitmap.ApplyFilter(mySubtractionFilter);
        myFirstBitmap.SetHasAlpha(false);
        Encoder.MakeFileFromBmp(string(myBaseName+".diff.png").c_str(), &myFirstBitmap);

        AC_INFO << "Smoothing: " << endl;
        Matrix3f mySmoothMatrix;
        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                mySmoothMatrix[i][j] = 1.0f/9.0f;
            }
        }
        PLFilter3x3 mySmoothFilter(mySmoothMatrix);
        myFirstBitmap.ApplyFilter(mySmoothFilter);
        Encoder.MakeFileFromBmp(string(myBaseName+".smooth.png").c_str(), &myFirstBitmap);

        // create histogramm
        std::vector<unsigned int> myHistogramm(256,0);
        for (int y = 0; y < myFirstBitmap.GetHeight(); ++y) {
            for (int x = 0; x < myFirstBitmap.GetWidth(); ++x) {
                PLPixel32 myPixel = myFirstBitmap.GetPixel32(x,y);
                int myValue = maximum(maximum(myPixel.GetR(), myPixel.GetG()), myPixel.GetB());
                myHistogramm[myValue]++;
            }
        }
        // now count the number of "bright" pixels
        unsigned int myBrightPixelCount = 0;
        for (int i = 0; i < 256; ++i) {
            if (i >= myThreshold) {
                myBrightPixelCount += myHistogramm[i];
            }
            AC_DEBUG << "H(" << i << ")=" << myHistogramm[i];
        }
        AC_INFO << "Threshold: " << myThreshold;
        AC_INFO << "Tolerance: " << myTolerance << "%";
        AC_INFO << "Bright Pixel Count: " << myBrightPixelCount;
        float myBrightPixelAverage = 100.0f * float(myBrightPixelCount) / (myFirstBitmap.GetHeight() * myFirstBitmap.GetWidth()) ;
        AC_INFO << "Average Bright: " << myBrightPixelAverage << "%";

        if ( myBrightPixelAverage <= myTolerance) {
            AC_INFO << "Images are almost equal";
            return 0;
        }
        AC_WARNING << "Images are too different Average Bright: " << myBrightPixelAverage << "%";
        return 1;
    }
    catch (asl::Exception & e) {
        cerr << "### ERROR: " << e << endl;
        return 2;
    }
    catch (PLTextException & e) {
        cerr << "### ERROR: " << e << endl;
        return 2;
    }
    catch (...) {
        cerr << "### ERROR: Unknown exception occured." << endl;
        return 2;
    }
    return 0;
}

#ifdef WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif

void
printVersion() {
    AC_INFO << "CVS $Revision: 1.6 $ $Date: 2005/03/09 17:11:16 $." << endl;
    AC_INFO << "Build at " << __DATE__ << " " << __TIME__ << "." << endl;
}
