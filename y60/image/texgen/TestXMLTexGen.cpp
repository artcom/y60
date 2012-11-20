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
//    $RCSfile: TestXMLTexGen.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
//
// Description:
//
//=============================================================================

#include "TestXMLTexGen.h"

#include <string>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include <asl/base/settings.h>
#include <asl/dom/Nodes.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planydec.h>
#include <paintlib/plrect.h>
#include <paintlib/Filter/plfiltercrop.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "TerrainTexGen.h"

using namespace asl;
using namespace TexGen;
using namespace std;

const string myTestDir("../../");

TestXMLTexGen::TestXMLTexGen (): UnitTest ("TestXMLTexGen") {
    _myTextureDefinitionMap =  createColorDefinitions();
}

TestXMLTexGen::~TestXMLTexGen () {
    TextureDefinitionMap::iterator iter;
    for (iter=_myTextureDefinitionMap->begin();
            iter != _myTextureDefinitionMap->end();
            ++iter)
    {
        delete (iter->second);
    }
}

TexGen::TextureDefinitionMap *
TestXMLTexGen::createColorDefinitions () {
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

string TestXMLTexGen::makeXMLTileDef (const string & theFileName) {

    // Load XML file
    ifstream theXMLFile ((myTestDir+theFileName).c_str());
    if (!theXMLFile) {
        throw invalid_argument ("Can't open xml tile definition file.");
    }

    ostringstream tempStream;
    tempStream << theXMLFile.rdbuf();

    return tempStream.str();
}

void TestXMLTexGen::runFileNotFoundTest () {
    // Generate XML string.
    string theDefStr = makeXMLTileDef("noblend_test.xml");
    dom::Document theDocument (theDefStr);

    // Change indexfile to bogus.
    theDocument("Texture")("Countryside")("Indexmap")("#text").nodeValue( "blafasel");

    bool ok = false;
    try {
        // Make filter from XML string.
        TerrainTexGen theFilter (*_myTextureDefinitionMap,
                                 theDocument("Texture")("Countryside"), 1);
    } catch (const PLTextException &) {
//        cerr << getTracePrefix() << (const char *)e;
        ok = true;
    }
    ENSURE_MSG (ok, "File not found exception handling.");
}

void TestXMLTexGen::runTypeErrorTest () {
    string theDefStr = makeXMLTileDef("noblend_test.xml");
    int charIndex = theDefStr.find("\"0\"");

    // Change index to non-numeric value.
    theDefStr[charIndex+1] = 'a';
    dom::Document theDocument (theDefStr);

    bool ok = false;
    try {
        // Make filter from XML string.
        TerrainTexGen theFilter (*_myTextureDefinitionMap,
                                 theDocument("Texture")("Countryside"), 1);
    } catch (invalid_argument e) {
//        cerr << getTracePrefix() << e.what() << endl;
        ok = true;
    }
    ENSURE_MSG (ok, "Argument type exception handling.");
}

void TestXMLTexGen::runArgMissingErrorTest () {
    // Create XML Texture definition with field missing...
    string s;
    s = string("<?xml version=\"1.0\"?>\n") +
        "<Texture>" +
        "  <Tile/>" +
        "</Texture>\n";
    dom::Document theDocument (s);

    bool ok = false;
    try {
        // Make filter from XML string.
        TerrainTexGen theFilter (*_myTextureDefinitionMap,
                                 theDocument("Texture")("Countryside"), 1);
    } catch (invalid_argument e) {
//        cerr << getTracePrefix() << e.what() << endl;
        ok = true;
    }
    ENSURE_MSG (ok, "Argument missing exception handling.");
}

void TestXMLTexGen::runGenerator (const TerrainTexGen & theFilter,
                                  const string & expectedResultFileName)
{
    PLAnyPicDecoder theDecoder;

    PLAnyBmp tempBmp;
    PLAnyBmp resultBmp;

    // Test generating complete texture
    theFilter.createTexture (PLPoint (128,128), tempBmp);
    resultBmp.CreateCopy (tempBmp, PLPixelFormat::X8R8G8B8);


    // Comment this in whenever the expected results change to generate new
    // result bitmaps.
/*    PLPNGEncoder theEncoder;
    theEncoder.MakeFileFromBmp ((myTestDir+expectedResultFileName+".png").c_str(), &resultBmp);
    exit(1);
*/
    ENSURE (resultBmp.GetWidth() == 128);
    ENSURE (resultBmp.GetHeight() == 128);
    PLAnyBmp expected_resultBmp;
    theDecoder.MakeBmpFromFile ((myTestDir+expectedResultFileName+".png").c_str(),
            &expected_resultBmp, PLPixelFormat::X8R8G8B8);
    ENSURE (expected_resultBmp.AlmostEqual (resultBmp, 4));

}

void TestXMLTexGen::runNoBlendTest () {
    // Generate XML string.
    string theDefStr = makeXMLTileDef("noblend_test.xml");
    dom::Document theDocument (theDefStr);

    PLAnyPicDecoder theDecoder;
    PLAnyBmp * theIndexBmp = new PLAnyBmp;
    theDecoder.MakeBmpFromFile ((myTestDir+"TestImages/index_small.png").c_str(), theIndexBmp);

    // Make filter.
    TerrainTexGen theFilter (*_myTextureDefinitionMap,
                             theDocument("Texture")("Countryside"), 1);

    // Run filter.
    runGenerator (theFilter, "TestImages/xml_noblend_expectedresult");
}

void TestXMLTexGen::runBlendTest () {
    string theDefStr = makeXMLTileDef("blend_test.xml");
    dom::Document theDocument (theDefStr);

    PLAnyPicDecoder theDecoder;
    PLAnyBmp *theIndexBmp = new PLAnyBmp;
    theDecoder.MakeBmpFromFile ((myTestDir+"TestImages/xml_blend_index.png").c_str(),
                                theIndexBmp);

    // Make filter.
    TerrainTexGen theFilter (*_myTextureDefinitionMap,
                             theDocument("Texture")("Countryside"),1);

    runGenerator (theFilter, "TestImages/xml_blend_expectedresult");
}

void TestXMLTexGen::run() {
    try {
        runNoBlendTest ();
        runBlendTest ();

        runFileNotFoundTest ();
        runTypeErrorTest ();
        runArgMissingErrorTest ();
    } catch (const exception& e) {
        cerr << "Exception in XMLTestGen: " << e.what() << endl;
        throw;
    } catch (const PLTextException& e) {
        cerr << "paintlib exception in XMLTestGen: " << (const char *)e << endl;
        throw;
    }
}


