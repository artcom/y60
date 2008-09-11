//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include "TextureGenerator.h"

#include <asl/dom/Nodes.h>

#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/plrect.h>

#include <stdexcept>
#include <fstream>

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


