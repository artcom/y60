/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <asl/base/Arguments.h>
#include <asl/base/file_functions.h>
#include <asl/base/Logger.h>

#include "plfiltersubtract.h"
#include "plfilter3x3.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

////////////////////////////////////////////////////////////////////////
//
// For file format specification see som/Movie.h
//
////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[])  {
    Arguments::AllowedOptionWithDocumentation myOptions[] = {
        {"--tolerance", "%f", "maximum number of 'bright' pixels in the difference bitmap allowed (in %)"},
        {"--threshold", "%s", "when a pixel is considered 'bright' (0..255)"},
        {"", "image1", "first image to compare"},
        {"", "image2", "second image to compare"},
        {"", ""}
    };
    asl::Arguments myArguments;
    myArguments.addAllowedOptionsWithDocumentation(myOptions);
    
    if (!myArguments.parse( argc, argv )) {
        return 1;
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
        myFirstBitmap.SetHasAlpha(false);
        mySecondBitmap.SetHasAlpha(false);

        string myBaseName = asl::removeExtension(myArguments.getArgument(0));
        Encoder.MakeFileFromBmp(string(myBaseName+".1st.png").c_str(), &myFirstBitmap);
        Encoder.MakeFileFromBmp(string(myBaseName+".2nd.png").c_str(), &mySecondBitmap);


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
    }
    catch (PLTextException & e) {
        cerr << "### ERROR: " << e << endl;
    }
    catch (...) {
        cerr << "### ERROR: Unknown exception occured." << endl;
    }
    return 2;
}

#ifdef WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
