/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: Test for Cg conforming types.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "ImageLoader.h"

#include <asl/string_functions.h>
#include <asl/UnitTest.h>
#include <iostream>
// Dumb hack since paintlib is installed as a non-debug variant
#ifdef _DEBUG
#undef _DEBUG
#include <paintlib/planybmp.h>
#include <paintlib/plbitmap.h>
#include <paintlib/planydec.h>
#define _DEBUG
#else
#include <paintlib/planybmp.h>
#include <paintlib/plbitmap.h>
#include <paintlib/planydec.h>
#endif


using namespace std;
using namespace asl;
using namespace dom;
using namespace y60;

class RasterTest : public UnitTest {
    public:
        RasterTest() : UnitTest("RasterTest") {  }
        void run() {
            testImageFile("../../../shader/shadertex/one_white_pixel.png");
            testImageFile("../../testfiles/white_square_4x4.PNG");
            testImageFile("../../testfiles/grey_square_4x4.PNG");
            testImageFile("../../testfiles/checker_square_4x4.PNG");
            testImageFile("../../testfiles/stripe_square_4x4.PNG");
            testImageFile("../../testfiles/testbild00.rgb");
            /*
            {

                PLAnyPicDecoder myDecoder;
                PLAnyBmp myReferenceBmp;
                myDecoder.MakeBmpFromFile("../../testfiles/white_square_4x4.png", &myReferenceBmp, PLPixelFormat::R8G8B8);
                ImageLoader myImageLoader("../../testfiles/white_square_4x4.png", ".", 1);
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
                ImageLoader myImageLoader(theFileName);
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
