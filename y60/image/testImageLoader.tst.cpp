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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

*/

#include <iostream>

#include <y60/image/y60image_paths.h>

#include <asl/zip/PackageManager.h>
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
using namespace y60;

class ImageLoaderTest : public UnitTest {
    public:
        ImageLoaderTest() : UnitTest("ImageLoaderTest") {  }
        void run() {

#define IMAGE_DIR CMAKE_CURRENT_SOURCE_DIR"/testfiles"

            PLAnyPicDecoder myDecoder;
            PLAnyBmp myReferenceBmp;
            myDecoder.MakeBmpFromFile(IMAGE_DIR "/testbild00.rgb", &myReferenceBmp, PLPixelFormat::X8R8G8B8);

            ImageLoader myAdapter(IMAGE_DIR "/testbild00.rgb");

            ENSURE(myReferenceBmp == myAdapter);

            ImageLoader my3DAdapter(IMAGE_DIR "/test3dtexture.jpg", PackageManagerPtr(), ITextureManagerPtr(), 8);
            my3DAdapter.ensurePowerOfTwo(IMAGE_RESIZE_SCALE, 8);
            SUCCESS("Loaded 3D-Texture image");
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup();
        addTest(new ImageLoaderTest);
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
