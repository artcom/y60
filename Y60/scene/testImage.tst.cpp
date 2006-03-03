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

#include "ImageBuilder.h"
#include "SceneBuilder.h"

#include <y60/Image.h>

#include <y60/Y60xsd.h>

#include <asl/string_functions.h>
#include <asl/UnitTest.h>
#include <dom/Nodes.h>
#include <dom/Schema.h>
#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

class ImageTest : public UnitTest {
    public:
        ImageTest() : UnitTest("ImageTest") {  }
        void run() {
            dom::DocumentPtr myDocument(new dom::Document);
            SceneBuilder mySceneBuilder(myDocument);
            ImageBuilder myImageBuilder("testImage", false);
            mySceneBuilder.appendImage(myImageBuilder);
            dom::NodePtr myImageNode = myImageBuilder.getNode();
            ImagePtr myImage = myImageNode->getFacade<Image>();
            myImage->setGraphicsId(1); // pretend it's been uploaded

            // change some attributes
            myImage->set<ImageMipmapTag>(1);

            // ensure dirty
            ENSURE( ! myImage->canReuseTexture());

            // fake upload & ensure clean
            myImage->storeTextureVersion();
            ENSURE(myImage->canReuseTexture());
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup();
        addTest(new ImageTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
