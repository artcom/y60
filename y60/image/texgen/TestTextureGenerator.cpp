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
//    $RCSfile: TestTextureGenerator.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
//
// Description:
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================

#include "TestTextureGenerator.h"

#include <stdexcept>
#include <fstream>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/plrect.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/dom/Nodes.h>

#include "TextureGenerator.h"

using namespace dom;
using namespace std;

const string myTestDir = "../";

//#define CREATE_TESTCASES

void TestTextureGenerator::run() {
/*
    light_withLightmap ("TestImages/lightmap_darkness.png",
                        "TestImages/darkness_expectedresult.png",
                        false);
    light_withLightmap ("TestImages/lightmap_bright.png",
                        "TestImages/brightness_expectedresult.png",
                        false);
    light_withLightmap ("TestImages/lightmap_gradient.png",
                        "TestImages/gradient_expectedresult.png",
                        false);
    light_withLightmap ("TestImages/lightmap_gradient.png",
                        "TestImages/gradientTiled_expectedresult.png",
                        true);
*/
}

void TestTextureGenerator::light_withLightmap (const string& myLightMapName,
                                               const string& myExpectedResultName,
                                               bool createPartialTexture)
{
/*
    try {
        Node theTexDef;
        string theTexDefFileName (myTestDir+"noblend_test.xml");
        ifstream theXMLFile (theTexDefFileName.c_str());
        if (!theXMLFile) {
            ENSURE_MSG (false, ("Can't open " +theTexDefFileName).c_str());
        }

        theXMLFile >> theTexDef;
        if (!theTexDef) {
            ENSURE_MSG (false, ("Can't parse " +theTexDefFileName).c_str());
        }

        PLAnyPicDecoder theDecoder;
        PLAnyBmp theIndexBmp;
        theDecoder.MakeBmpFromFile ((myTestDir+"TestImages/index_big.png").c_str(), &theIndexBmp );

        PLAnyBmp theLightMap;
        theDecoder.MakeBmpFromFile ((myTestDir+myLightMapName).c_str(), &theLightMap );

        TexGen::TextureGenerator theTextureGenerator (&theTexDef, myTestDir, false,
                                                      theIndexBmp, theLightMap);

        PLAnyBmp theResultBmp;
        theResultBmp.Create(128,128,32,false);
        if (createPartialTexture) {
            PLRect rect(32, 32, 96, 96);
            theTextureGenerator.createTexture(rect,PLPoint (128,128),theResultBmp);
        } else {
            PLRect rect(0, 0, 128, 128);
            theTextureGenerator.createTexture (rect, PLPoint (128, 128),
                                               theResultBmp);
        }

#ifdef CREATE_TESTCASES
        // create testcases
        PLPNGEncoder theEncoder;
        theEncoder.MakeFileFromBmp ((myTestDir+myExpectedResultName).c_str(), &theResultBmp);
#else
        PLAnyBmp tempResultBmp;
        theDecoder.MakeBmpFromFile ((myTestDir+myExpectedResultName).c_str(), &tempResultBmp );
        PLAnyBmp theReference;
        theReference.CreateCopy (tempResultBmp, 24);
        ENSURE_MSG (theReference == theResultBmp,
                (string("Result == ")+myExpectedResultName).c_str());

#endif
    } catch (const std::exception& e) {
        cout <<"Exception occurred :"<<e.what()<<endl;
    } catch (const PLTextException& e) {
        cout <<"Paintlib exception occurred :"<<(const char *)e<<endl;
    }
*/
}


