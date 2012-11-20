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
//
//    $RCSfile: TestTerrainTexGen.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description:
//
//=============================================================================

#include "TestTerrainTexGen.h"

#include <typeinfo>
#include <stdexcept>
#include <assert.h>
#include <sstream>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/plpixel32.h>
#include <paintlib/plpixel24.h>
#include <paintlib/plpixel8.h>
#include <paintlib/plrect.h>
#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/Filter/plfilterfillrect.h>
#include <paintlib/Filter/plfiltercrop.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/base/settings.h>

#include "TerrainTexGen.h"
#include "LayerDefinition.h"
#include "TextureDefinition.h"

using namespace TexGen;
using namespace std;

// #define CREATE_TESTCASES

const string myTestImgDir("../../TestImages/");

TextureDefinitionMap * TestTerrainTexGen::createColorDefinitions () {
    TextureDefinitionMap * theDefinitions =
        new TextureDefinitionMap;

    for (int i=0; i<8; i+=2) {
        PLPixel32 theTileColor;
        switch (i) {
            case 0:
                theTileColor.Set (255, 0, 0);
                break;
            case 2:
                theTileColor.Set (0, 255, 0);
                break;
            case 4:
                theTileColor.Set (0, 0, 255);
                break;
            case 6:
                theTileColor.Set (255, 255, 255);
                break;
        }

        (*theDefinitions)[i] = new TextureDefinition (i);
        (*theDefinitions)[i]->addLayer (new LayerDefinition (theTileColor, 1.0));
    }

    return theDefinitions;
}

TextureDefinitionMap * TestTerrainTexGen::createBmpDefinitions() {
    TextureDefinitionMap * theDefinitions =
        new TextureDefinitionMap;
    PLAnyPicDecoder theDecoder;

    for (int i=0; i<8; i+=2) {
        PLAnyBmp * theBmp = new PLAnyBmp;

        stringstream st;
        st << myTestImgDir << "tex" << i << ".png";
        string s = st.str();

		theDecoder.MakeBmpFromFile (s.c_str(), theBmp, PLPixelFormat::X8R8G8B8);
        (*theDefinitions)[i] = new TextureDefinition (i);
        (*theDefinitions)[i]->addLayer (new LayerDefinition (theBmp, 1, 1, 1.0));
    }

    return theDefinitions;
}

void TestTerrainTexGen::runTestsWithOneSize
    (const TerrainTexGen& theTerrainTexGen,
     PLBmp* theIndexBmp,
     const string& theResultFileName,
     bool runPartialTest,
     const PLPoint& theResultSize)
{
    PLAnyPicDecoder theDecoder;
    PLAnyBmp theResultBmp;
    theTerrainTexGen.createTexture ( theResultSize, theResultBmp);
#ifdef CREATE_TESTCASES
    // Write expected results out to file.
    PLPNGEncoder theEncoder;
    theEncoder.MakeFileFromBmp((myTestImgDir + theResultFileName).c_str(), &theResultBmp);
#else
    PLAnyBmp expectedResultBmp;
    theDecoder.MakeBmpFromFile ((myTestImgDir + theResultFileName).c_str(), &expectedResultBmp, PLPixelFormat::X8R8G8B8);
    ENSURE (theResultBmp == expectedResultBmp);
    if (!(theResultBmp == expectedResultBmp)) {
        PLPNGEncoder theEncoder;
        theEncoder.MakeFileFromBmp((string("kaputt_")+theResultFileName).c_str(), &theResultBmp);
    }

    if (runPartialTest) {
        PLRect theSrcRect (theIndexBmp->GetWidth()/4, theIndexBmp->GetHeight()/4,
                           3*theIndexBmp->GetWidth()/4, 3*theIndexBmp->GetHeight()/4);
        theTerrainTexGen.createPartialTexture (theSrcRect,
                                               theResultSize/2, theResultBmp);
        expectedResultBmp.ApplyFilter(PLFilterCrop (theResultSize.x/4, theResultSize.y/4,
                            3*theResultSize.x/4, 3*theResultSize.y/4));
        ENSURE (theResultBmp == expectedResultBmp);
    }
#endif
}

void TestTerrainTexGen::runTestsWithIndexBmp
    (float myBlendRadius,
     const string & theIndexFileName,
     const string & theBigResultFileName,
     const string & theSmallResultFileName,
     bool runPartialTest,
     bool useColorDefinition)
{
    try {
        TextureDefinitionMap * theDefinitions;
        if (useColorDefinition)
            theDefinitions = createColorDefinitions();
        else
            theDefinitions = createBmpDefinitions();

        PLAnyPicDecoder theDecoder;
        PLAnyBmp * theIndexBmp = new PLAnyBmp;
        theDecoder.MakeBmpFromFile ((myTestImgDir + theIndexFileName).c_str(), theIndexBmp);
        assert (theIndexBmp->GetBitsPerPixel() == 8);

        TerrainTexGen theTerrainTexGen(*theDefinitions, myBlendRadius,
                theIndexBmp, NULL, 1);

        runTestsWithOneSize (theTerrainTexGen, theIndexBmp, theSmallResultFileName,
                runPartialTest, PLPoint (64,64));
        runTestsWithOneSize (theTerrainTexGen, theIndexBmp, theBigResultFileName,
                runPartialTest, PLPoint (256,256));
    } catch (PLTextException e) {
        cout << (const char *) e << endl;
    } catch (const std::exception e) {
        cout << e.what() << endl;
    }
}

void TestTerrainTexGen::run() {


    {
        cerr << getTracePrefix() << "Testing without blending, colors as textures." << endl;
        cerr << getTracePrefix() << "  (2x2) index" << endl;
        runTestsWithIndexBmp ( 0.0, "index_small.png",
                              "noblend_col_expectedresult.png",
                              "noblend_col_small_expectedresult.png",
                              false, true);
        cerr << getTracePrefix() << "  (128x128) index" << endl;
        runTestsWithIndexBmp (0.0,"index_big.png",
                              "noblend_col_expectedresult.png",
                              "noblend_col_small_expectedresult.png",
                              true, true);
    }

    {
        cerr << getTracePrefix() << "Testing without blending, images as textures." << endl;
        cerr << getTracePrefix() << "  (2x2) index" << endl;

        runTestsWithIndexBmp (0.0, "index_small.png",
                              "noblend_bmp_2_expectedresult.png",
                              "noblend_bmp_2_small_expectedresult.png",
                              false, false);
        cerr << getTracePrefix() << "  (128x128) index" << endl;
        runTestsWithIndexBmp ( 0.0, "index_big.png",
                              "noblend_bmp_128_expectedresult.png",
                              "noblend_bmp_128_small_expectedresult.png",
                              true, false);

    }

    {
        cerr << getTracePrefix() << "Testing with blending, images as textures." << endl;
        cerr << getTracePrefix() << "  (2x2) index" << endl;

        runTestsWithIndexBmp (0.5, "index_small.png",
                              "blend_bmp_2_expectedresult.png",
                              "blend_bmp_2_small_expectedresult.png",
                              false, false);
        cerr << getTracePrefix() << "  (128x128) index" << endl;
        runTestsWithIndexBmp (0.5, "index_big.png",
                              "blend_bmp_128_expectedresult.png",
                              "blend_bmp_128_small_expectedresult.png",
                              true, false);

    }

    {
        cerr << getTracePrefix() << "Testing layers." << endl;
        TextureDefinitionMap * theDefinitions =
                new TextureDefinitionMap;
        (*theDefinitions)[0] = new TextureDefinition(0);

        PLAnyPicDecoder myDecoder;
        PLBmp * myBmp1 = new PLAnyBmp;
        myDecoder.MakeBmpFromFile ((myTestImgDir+"tex0.png").c_str(), myBmp1, PLPixelFormat::X8R8G8B8);
        assert (myBmp1->GetBitsPerPixel() == 32);

        (*theDefinitions)[0]->addLayer (new LayerDefinition(myBmp1, 1, 1, 0.5));
        PLBmp * myBmp2 = new PLAnyBmp;
        myDecoder.MakeBmpFromFile ((myTestImgDir+"tex2.png").c_str(), myBmp2, PLPixelFormat::X8R8G8B8);
        (*theDefinitions)[0]->addLayer (new LayerDefinition(myBmp2, 2, 2, 0.5));
        PLAnyBmp * myIndexBmp = new PLAnyBmp;
        myIndexBmp->Create (2, 2, PLPixelFormat::I8);
        myIndexBmp->SetPixel (0,0,PLPixel8(0));
        myIndexBmp->SetPixel (0,1,PLPixel8(0));
        myIndexBmp->SetPixel (1,0,PLPixel8(0));
        myIndexBmp->SetPixel (1,1,PLPixel8(0));
        TerrainTexGen theNoBlendGenerator (*theDefinitions, 0.0, myIndexBmp, NULL, 1);

        PLAnyBmp myResultBmp;
        theNoBlendGenerator.createTexture (PLPoint (128, 128), myResultBmp);
#ifdef CREATE_TESTCASES
        // Write expected results out to file.
        PLPNGEncoder theEncoder;
        theEncoder.MakeFileFromBmp((myTestImgDir + "layer_expectedresult.png").c_str(),
                &myResultBmp);
#else
        PLAnyBmp expectedResultBmp;
        myDecoder.MakeBmpFromFile ((myTestImgDir + "layer_expectedresult.png").c_str(),
                &expectedResultBmp, PLPixelFormat::X8R8G8B8);
        ENSURE (myResultBmp == expectedResultBmp);
#endif
    }

    bool ok;
    ok = false;
    TextureDefinitionMap * theDefinitions = createColorDefinitions();
    PLAnyPicDecoder theDecoder;
    PLAnyBmp * theIndexBmp = new PLAnyBmp;
    theDecoder.MakeBmpFromFile ((myTestImgDir + "index_small.png").c_str(),
            theIndexBmp);
    TerrainTexGen theGenerator (*theDefinitions, 0.0, theIndexBmp, NULL, 1);
    assert (theIndexBmp->GetBitsPerPixel() == 8);
    theIndexBmp->GetLineArray()[0][0] = 5;
    PLAnyBmp theResultBmp;
    theGenerator.createTexture (PLPoint (128,128), theResultBmp);

}

