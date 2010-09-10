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

#include <string>
#include <iostream>

#include <asl/math/linearAlgebra.h>
#include <asl/math/numeric_functions.h>

#include <asl/base/UnitTest.h>

#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>

#include "PixelEncoding.h"
#include "TextureAtlas.h"
#include "Image.h"

using namespace std; 
using namespace asl;
using namespace y60;

class TextureAtlasUnitTest : public UnitTest {
public:
    TextureAtlasUnitTest()
        : UnitTest("TextureAtlasUnitTest") {}

    void run() {
        {
            std::vector<std::string> myNames;
            std::vector<dom::ResizeableRasterPtr> myRasters;

            /*
            dom::ResizeableRasterPtr redRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, 3, 17));
            redRaster->fillRect(0,0,3,17,asl::Vector4f(1,0,0,1));
            myNames.push_back("red");
            myRasters.push_back(redRaster);
            */
            for (int i = 0; i < 100; ++i) {
                dom::ResizeableRasterPtr blueRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, random(2,50), random(2,50)));
                AC_WARNING << "[" << i << "]" << blueRaster->width() << " x " << blueRaster->height();
                blueRaster->fillRect(0,0,blueRaster->width(), blueRaster->height(),asl::Vector4f(random(0.0f,1.0f),random(0.0f, 1.0f),random(0.0f, 1.0f),1));
                myNames.push_back("blue");
                myRasters.push_back(blueRaster);
            }

            TextureAtlasPtr myAtlas = TextureAtlas::generate(myNames, myRasters, false, false);
            ENSURE(myAtlas);

            {
                Path myPath("test.png", UTF8);
                PLAnyBmp myBmp;
                PLPixelFormat pf;
                mapPixelEncodingToFormat(y60::RGBA, pf);

                myBmp.Create( myAtlas->getRaster()->width(), myAtlas->getRaster()->height(), pf,
                        const_cast<unsigned char*>(myAtlas->getRaster()->pixels().begin()), myAtlas->getRaster()->width() * 4);
                PLPNGEncoder myEncoder;
                myEncoder.MakeFileFromBmp(myPath.toLocale().c_str(), &myBmp);
            }
        }
    }

};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new TextureAtlasUnitTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

