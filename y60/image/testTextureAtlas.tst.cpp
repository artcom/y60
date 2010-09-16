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
#include "TextureAtlas.h"
#include "Image.h"

using namespace std; 
using namespace asl;

namespace y60 {

class TextureAtlasUVTest : public UnitTest {
public:
    TextureAtlasUVTest()
        : UnitTest("TextureAtlasUVTest") {}

    void run() {
        {  // test UV Coord translation - only scale
           /* 
            *  subtexture:     atlas:
            *   (1x2)          (4x8)
            *                  X...    
            *      X           Y...
            *      Y           ....
            *                  ....
            *                  ....
            *                  ....
            */
            Vector2<AC_SIZE_TYPE> atlasSize(4,8);
            Vector2<AC_SIZE_TYPE> bitmapSize(1,2);
            Vector2<AC_SIZE_TYPE> bitmapPosition(0u,0u);
            bool rotated = false;

            Matrix4f translation = TextureAtlas::createUVTranslation(atlasSize, bitmapSize, bitmapPosition, rotated);

            ENSURE_ALMOSTEQUAL(Vector4f(0,0,0,1) * translation, Vector4f(0.0 ,0.0 ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,0,0,1) * translation, Vector4f(0.25,0.0 ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(0,1,0,1) * translation, Vector4f(0.00,0.25,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,1,0,1) * translation, Vector4f(0.25,0.25,0,1));

        }
        {  // test UV Coord translation - scale & reposition
           /* 
            *  subtexture:     atlas:
            *   (1x2)          (4x8)
            *                  ....    
            *      X           ..X.
            *      Y           ..Y.
            *                  ....
            *                  ....
            *                  ....
            */
            Vector2<AC_SIZE_TYPE> atlasSize(4,8);
            Vector2<AC_SIZE_TYPE> bitmapSize(1,2);
            Vector2<AC_SIZE_TYPE> bitmapPosition(2,1);
            bool rotated = false;

            Matrix4f translation = TextureAtlas::createUVTranslation(atlasSize, bitmapSize, bitmapPosition, rotated);

            ENSURE_ALMOSTEQUAL(Vector4f(0,0,0,1) * translation, Vector4f(0.50,0.125,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,0,0,1) * translation, Vector4f(0.75,0.125,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(0,1,0,1) * translation, Vector4f(0.50,0.375,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,1,0,1) * translation, Vector4f(0.75,0.375,0,1));

        }
        {  // test UV Coord translation - scale & rotate
           /* 
            *  subtexture:     atlas:
            *   (1x2)          (4x8)
            *                  XY..    
            *      X           ....
            *      Y           ....
            *                  ....
            *                  ....
            *                  ....
            */
            Vector2<AC_SIZE_TYPE> atlasSize(4,8);
            Vector2<AC_SIZE_TYPE> bitmapSize(1,2);
            Vector2<AC_SIZE_TYPE> bitmapPosition(0u,0u);
            bool rotated = true;

            Matrix4f translation = TextureAtlas::createUVTranslation(atlasSize, bitmapSize, bitmapPosition, rotated);

            ENSURE_ALMOSTEQUAL(Vector4f(0,0,0,1) * translation, Vector4f(0.00,0.125,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,0,0,1) * translation, Vector4f(0.00,0.000,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(0,1,0,1) * translation, Vector4f(0.50,0.125,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,1,0,1) * translation, Vector4f(0.50,0.000,0,1));

        }
        {  // test UV Coord translation - scale, rotate & reposition
           /* 
            *  subtexture:     atlas:
            *   (1x2)          (4x8)
            *                  ....    
            *      X           .XY.
            *      Y           ....
            *                  ....
            *                  ....
            *                  ....
            */
            Vector2<AC_SIZE_TYPE> atlasSize(4,8);
            Vector2<AC_SIZE_TYPE> bitmapSize(1,2);
            Vector2<AC_SIZE_TYPE> bitmapPosition(1,1);
            bool rotated = true;

            Matrix4f translation = TextureAtlas::createUVTranslation(atlasSize, bitmapSize, bitmapPosition, rotated);

            ENSURE_ALMOSTEQUAL(Vector4f(0,0,0,1) * translation, Vector4f(0.25,0.25 ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,0,0,1) * translation, Vector4f(0.25,0.125,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(0,1,0,1) * translation, Vector4f(0.75,0.25 ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,1,0,1) * translation, Vector4f(0.75,0.125,0,1));

        }
        {  // test UV Coord translation - scale, rotate & reposition
           /* 
            *  subtexture:            atlas:
            *   (1x2)rotated          (8x10)
            */
            Vector2<AC_SIZE_TYPE> atlasSize(8,10);
            Vector2<AC_SIZE_TYPE> bitmapSize(1,2);
            Vector2<AC_SIZE_TYPE> bitmapPosition(5,8);
            bool rotated = true;

            Matrix4f translation = TextureAtlas::createUVTranslation(atlasSize, bitmapSize, bitmapPosition, rotated);
            ENSURE_ALMOSTEQUAL(Vector4f(0,0,0,1) * translation, Vector4f(5.0f/8.0f,9.0f/10.0f ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,0,0,1) * translation, Vector4f(5.0f/8.0f,8.0f/10.0f,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(0,1,0,1) * translation, Vector4f(7.0f/8.0f,9.0f/10.0f ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,1,0,1) * translation, Vector4f(7.0f/8.0f,8.0f/10.0f,0,1));

        }
        {  // test UV Coord translation - scale, rotate & reposition
           /* 
            *  subtexture:            atlas:
            *   (2x1)rotated          (8x10)
            */
            Vector2<AC_SIZE_TYPE> atlasSize(8,10);
            Vector2<AC_SIZE_TYPE> bitmapSize(2,1);
            Vector2<AC_SIZE_TYPE> bitmapPosition(5,8);
            bool rotated = true;

            Matrix4f translation = TextureAtlas::createUVTranslation(atlasSize, bitmapSize, bitmapPosition, rotated);
            ENSURE_ALMOSTEQUAL(Vector4f(0,0,0,1) * translation, Vector4f(5.0f/8.0f,10.0f/10.0f ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,0,0,1) * translation, Vector4f(5.0f/8.0f,8.0f/10.0f,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(0,1,0,1) * translation, Vector4f(6.0f/8.0f,10.0f/10.0f ,0,1));
            ENSURE_ALMOSTEQUAL(Vector4f(1,1,0,1) * translation, Vector4f(6.0f/8.0f,8.0f/10.0f,0,1));

        }
    }

};

class TextureAtlasUnitTest : public UnitTest {
public:
    TextureAtlasUnitTest()
        : UnitTest("TextureAtlasUnitTest") {}


    void makeSubtexture(const std::string & theName, int theWidth, int theHeight, const Vector4f & theColor) {
        dom::ResizeableRasterPtr raster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, theWidth, theHeight));
        raster->fillRect(0,0,theWidth, theHeight, theColor);
        _mySubtextures.insert(make_pair(theName, raster));
    }

    void textPixels(const TextureAtlas & myAtlas, const string & theName, const Vector4f & theExpectedColor) {
        Matrix4f textureTranslation;
        ENSURE(myAtlas.findTextureTranslation(theName, textureTranslation) );
        ENSURE(myAtlas.getRaster());

        Vector4f oneCorner = Vector4f(0,0,0,1) * textureTranslation;
        Vector4f otherCorner = Vector4f(1,1,0,1) * textureTranslation;
        int minX = floor(min(oneCorner[0],otherCorner[0])*myAtlas.getRaster()->width()+0.5);
        int maxX = floor(max(oneCorner[0],otherCorner[0])*myAtlas.getRaster()->width()+0.5);
        int minY = floor(min(oneCorner[1],otherCorner[1])*myAtlas.getRaster()->height()+0.5);
        int maxY = floor(max(oneCorner[1],otherCorner[1])*myAtlas.getRaster()->height()+0.5);
        setSilentSuccess(true);
        for (int y = minY-1; y <= maxY; ++y) {
           for (int x = minX-1; x <= maxX; ++x) {
               ENSURE_EQUAL(theExpectedColor, myAtlas.getRaster()->getPixel(x,y));
           }
        }
        setSilentSuccess(false);
    }

    void testBitmap() {
        _mySubtextures.clear();
        makeSubtexture("red", 5, 1, Vector4f(1,0,0,1));
        makeSubtexture("blue", 2, 2, Vector4f(0,0,1,1));
        makeSubtexture("yellow", 2, 1, Vector4f(1,1,0,1));
        makeSubtexture("green", 3, 2, Vector4f(0,1,0,1));

        /*
        for (int i = 0; i < 100; ++i) {
            dom::ResizeableRasterPtr blueRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, random(2,20), random(2,20)));
            AC_TRACE << "[" << i << "]" << blueRaster->width() << " x " << blueRaster->height();
            blueRaster->fillRect(0,0,blueRaster->width(), blueRaster->height(),asl::Vector4f(random(0.0f,1.0f),random(0.0f, 1.0f),random(0.0f, 1.0f),1));
            myNames.push_back("blue");
            myRasters.push_back(blueRaster);
        }
        */

        TextureAtlas myAtlas(_mySubtextures, true, false);

        Matrix4f textureTranslation;
        ENSURE( ! myAtlas.findTextureTranslation("doesn't exist", textureTranslation) );
       
        textPixels(myAtlas, "red", Vector4f(1,0,0,1));
        textPixels(myAtlas, "blue", Vector4f(0,0,1,1));
        textPixels(myAtlas, "yellow", Vector4f(1,1,0,1));
        textPixels(myAtlas, "green", Vector4f(0,1,0,1));
    }
    
    void testLoadSave() {
        _mySubtextures.clear();
        // first some error cases
        ENSURE_EXCEPTION(TextureAtlas t(Path("does-not-exist", UTF8)), asl::Exception);
        ENSURE_EXCEPTION(TextureAtlas t(Path("", UTF8)), asl::Exception);

        // now load & save an atlas
        makeSubtexture("red", 5, 1, Vector4f(1,0,0,1));
        makeSubtexture("blue", 2, 2, Vector4f(0,0,1,1));

        TextureAtlas firstAtlas(_mySubtextures, true, false);
        firstAtlas.saveToFile(asl::Path("test.xml", UTF8));

        TextureAtlas secondAtlas(asl::Path("test.xml", UTF8));

        Matrix4f textureTranslation;
        ENSURE( ! secondAtlas.findTextureTranslation("doesn't exist", textureTranslation) );
        textPixels(secondAtlas, "red", Vector4f(1,0,0,1));
        textPixels(secondAtlas, "blue", Vector4f(0,0,1,1));

        asl::Vector2<AC_SIZE_TYPE> textureSize;
        ENSURE(secondAtlas.findTextureSize("red", textureSize));
        ENSURE_EQUAL(textureSize, Vector2<AC_SIZE_TYPE>(5,1));
    }

    void testDuplicates() {
        _mySubtextures.clear();
        makeSubtexture("red", 5, 1, Vector4f(1,0,0,1));
        makeSubtexture("red", 5, 1, Vector4f(1,0,0,1)); // 2nd texture with same name should be discarded
        TextureAtlas myAtlas(_mySubtextures, true, false);
        ENSURE_EQUAL(1, myAtlas.getTextureCount());
        ENSURE_EQUAL(Vector2<AC_SIZE_TYPE>(7,3), myAtlas.getRaster()->getSize());
    }

    void run() {
        testDuplicates();
        testBitmap();
        testLoadSave();
    }

    TextureAtlas::Subtextures _mySubtextures;
};


}

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new y60::TextureAtlasUVTest());
        addTest(new y60::TextureAtlasUnitTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

