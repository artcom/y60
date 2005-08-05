/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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
using namespace y60;

class ImageLoaderTest : public UnitTest {
    public:
        ImageLoaderTest() : UnitTest("ImageLoaderTest") {  }
        void run() {
            PLAnyPicDecoder myDecoder;
            PLAnyBmp myReferenceBmp;
            myDecoder.MakeBmpFromFile("../../testfiles/testbild00.rgb", &myReferenceBmp, PLPixelFormat::X8R8G8B8);

            ImageLoader myAdapter("../../testfiles/testbild00.rgb",0,ITextureManagerPtr(0),1);

            ENSURE(myReferenceBmp == myAdapter);

            ImageLoader my3DAdapter("../../testfiles/test3dtexture.jpg", 0, ITextureManagerPtr(0), 8);
            my3DAdapter.ensurePowerOfTwo(IMAGE_RESIZE_SCALE, SINGLE, 8);
            SUCCESS("Loaded 3D-Texture image");
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup();
        addTest(new ImageLoaderTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

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
