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

#include <iostream>

#include <y60/image/y60image_paths.h>

#include <asl/base/string_functions.h>
#include <asl/base/UnitTest.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/plbitmap.h>
#include <paintlib/planydec.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "ImageLoader.h"


using namespace std;
using namespace asl;
using namespace dom;
using namespace y60;

class RasterTest : public UnitTest {
    public:
        RasterTest() : UnitTest("RasterTest") {  }
        void run() {


#define IMAGE_DIR CMAKE_CURRENT_SOURCE_DIR"/testfiles"

            testImageFile(IMAGE_DIR "/white_square_4x4.PNG");
            testImageFile(IMAGE_DIR "/grey_square_4x4.PNG");
            testImageFile(IMAGE_DIR "/checker_square_4x4.PNG");
            testImageFile(IMAGE_DIR "/stripe_square_4x4.PNG");
            testImageFile(IMAGE_DIR "/testbild00.rgb");
            testImageFile(IMAGE_DIR "/one_white_pixel.png");
            /*
            {

                PLAnyPicDecoder myDecoder;
                PLAnyBmp myReferenceBmp;
                myDecoder.MakeBmpFromFile(IMAGE_DIR "/white_square_4x4.png", &myReferenceBmp, PLPixelFormat::R8G8B8);
                ImageLoader myImageLoader( std::string(IMAGE_DIR "/white_square_4x4.png"), ".", 1);
                ENSURE(myReferenceBmp == myImageLoader);

                ResizeableRasterPtr myRaster = myImageLoader.getRaster();
                dom::ValuePtr myRasterValue = myImageLoader.getData();
                string myRasterString = myRasterValue->getString();

                cerr << myRasterString << endl;

                ValuePtr myNewRasterValue = createRasterValue(myImageLoader.getEncoding(), myImageLoader.GetWidth(), myImageLoader.GetHeight());
                myNewRasterValue->setString(myRasterString);

                ResizeableRasterPtr myNewResizableRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(myNewRasterValue);
                ENSURE(myRaster->pixels() == myNewResizableRaster->pixels());
            }
            {
                ImageLoader myImageLoader("../../../shader/shadertex/one_white_pixel.png", ".", 1);
                dom::ValuePtr myRasterValue = myImageLoader.getData();
                cerr << "ONE WHITE PIXEL:" << endl;
                cerr << myRasterValue->getString() << endl;

            }*/
        }
    private:
            void testImageFile(string theFileName) {
                cerr << "Testing image: " << theFileName << endl;
                PLAnyPicDecoder myDecoder;
                PLAnyBmp myReferenceBmp;
                myDecoder.MakeBmpFromFile(theFileName.c_str(), &myReferenceBmp);
                myReferenceBmp.SetResolution(PLPoint(0,0)); // ignore dpi in file
                ImageLoader myImageLoader(theFileName);
                ENSURE_EQUAL(myReferenceBmp.GetBitsPerPixel(), myImageLoader.GetBitsPerPixel());
                myImageLoader.SetResolution(PLPoint(0,0)); // ignore dpi in file
                ENSURE(myReferenceBmp == myImageLoader);

                dom::ValuePtr myRasterValue = createRasterValue(myImageLoader.getEncoding(), myImageLoader.GetWidth(), myImageLoader.GetHeight(), *myImageLoader.getData());
                ResizeableRasterPtr myRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(myRasterValue); 
                string myRasterString = myRasterValue->getString();

                //cerr << myRasterString << endl;

                ValuePtr myNewRasterValue = createRasterValue(myImageLoader.getEncoding(), myImageLoader.GetWidth(), myImageLoader.GetHeight());
                myNewRasterValue->setString(myRasterString);

                ResizeableRasterPtr myNewResizableRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(myNewRasterValue);
                ENSURE(myRaster->pixels() == myNewResizableRaster->pixels());
            }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup();
        addTest(new RasterTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.run();
    } catch (const PLTextException & ex) {
        cerr << "PLException during test execution: " << ex << endl;
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
