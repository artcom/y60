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
//    $RCSfile: TextureGenerator.cpp,v $
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

#include <assert.h>
#include <stdexcept>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <math.h>


#include <asl/base/settings.h>
#include <asl/base/string_functions.h>
#include <asl/dom/Nodes.h>

#include "TextureGenerator.h"
#include "LitTerrainTexGen.h"
#include "XmlHelper.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/plpoint.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfiltercrop.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#include <paintlib/Filter/plfilterfill.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace dom;
using namespace asl;

namespace TexGen {

TextureGenerator::TextureGenerator (const dom::Node* myNode, bool dummy)
    : _myCityTexGen(0)
{
    try {
        const Node & theTextureNode = (*myNode)("Texture");
        if (!theTextureNode) {
            throw (invalid_argument ("Texture node missing."));
        }
        const Node & theCountrysideNode = (theTextureNode)("Countryside");
        if (!theCountrysideNode) {
            throw (invalid_argument ("Countryside node missing."));
        }
        const Node & myTileNode = (theTextureNode)("Tiles");
        loadTiles (myTileNode);
        _myTerrainTexGen = new TerrainTexGen
                (_myTextureDefinitionMap, theCountrysideNode, 1);

        const Node & theCityNode = (theTextureNode)("City");
        if (theCityNode) {
            string myPosString = theCityNode("Pos")("#text").nodeValue();
            sscanf (myPosString.c_str(), "%i,%i,%i,%i",
                    &_myCityPosition.tl.x, &_myCityPosition.tl.y,
                    &_myCityPosition.br.x, &_myCityPosition.br.y);
            cerr <<"Virtual city coords x1: "<<_myCityPosition.tl.x<<
                " y1: "<<_myCityPosition.tl.y<<
                " x2: "<<_myCityPosition.br.x<<
                " y2: "<<_myCityPosition.br.y<<endl;
            _myCityTexGen = new TerrainTexGen (_myTextureDefinitionMap, theCityNode, 8);
/*            if (fabs((double(_myCityPosition.Width())/_myCityPosition.Height() -
                double(_myCityIndexBmp.GetWidth())/_myCityIndexBmp.GetHeight())) > 0.001) {
                throw (invalid_argument
                    ("City bitmap proportions and coordinates in xml definition file don't match."));
            }
*/
        } else {
            cout << "Warning: no city texture!" << endl;
        }
    } catch (const PLTextException & e) {
        kaputt (e);
        throw;
    } catch (const exception& e) {
        kaputt (e.what());
        throw;
    }
}



TextureGenerator::~TextureGenerator() {
    delete _myTerrainTexGen;
    delete _myCityTexGen;
    TextureDefinitionMap::iterator iter;
    for (iter=_myTextureDefinitionMap.begin();
         iter != _myTextureDefinitionMap.end();
         ++iter)
    {
        delete (iter->second);
    }

}

void
TextureGenerator::loadTiles (const dom::Node & myTilesNode) {
    using namespace dom;
    string myTileDirectory = getRequiredXMLField(myTilesNode, "directory");
    myTileDirectory += "/";

    int i=0;
    while (const Node & curTileNode = myTilesNode("Tile",i)) {
        TextureDefinition * newTextureDefinition =
                new TextureDefinition(curTileNode, myTileDirectory, true);
        _myTextureDefinitionMap[newTextureDefinition->getIndex()] =
                newTextureDefinition;
        i++;
    }
};

// Create Texture with in virtual map size coords
void TextureGenerator::createTexture (PLRect& srcRect,
        const PLPoint& resultSize, PLBmp & resultBmp,
        bool myWhitenBorder) const
{
    if (srcRect.Width() != srcRect.Height()) {
        cout << "Width: " << srcRect.Width() << ", Height: " << srcRect.Height() << endl;
        assert (false);
    }
    assert (resultSize.x == resultSize.y);
    assert (srcRect.Width()>0 && srcRect.Height()>0);
    assert (srcRect.tl.x >= 0 && srcRect.tl.y >= 0 &&
            srcRect.br.x <= 65536 && srcRect.br.y <= 65536);

    PLRect normedRect;
    if (_myCityTexGen &&
        srcRect.tl.x >= _myCityPosition.tl.x &&
        srcRect.tl.y >= _myCityPosition.tl.y &&
        srcRect.br.x <= _myCityPosition.br.x &&
        srcRect.br.y <= _myCityPosition.br.y)
    {
        // Inside city area.
        normedRect = PLRect (int(srcRect.tl.x-_myCityPosition.tl.x)/8,
                int(srcRect.tl.y-_myCityPosition.tl.y)/8,
                int(srcRect.br.x-_myCityPosition.tl.x)/8,
                int(srcRect.br.y-_myCityPosition.tl.y)/8);
        if (normedRect.Width() == 0 || normedRect.Height() == 0) {
            cerr << "Warning: Texture smaller than indexmap pixel can't be generated." << endl;
            cerr << "srcRect: (" << srcRect.tl.x << ", " << srcRect.tl.y << ") - (" <<
                                  srcRect.br.x << ", " << srcRect.br.y << ")" << endl;
        }
        _myCityTexGen->getTexture(normedRect , resultSize, resultBmp);

        // In the border area, we call both TerrainTexGens and blend.
        const int BORDER_WIDTH = 256;
//        const int BORDER_WIDTH = 1024;
        PLPoint myMiddle ((srcRect.br.x+srcRect.tl.x)/2, (srcRect.br.y+srcRect.tl.y)/2);
        int myDist = min (myMiddle.x - _myCityPosition.tl.x,
                          myMiddle.y - _myCityPosition.tl.y);
        myDist = min (myDist, _myCityPosition.br.x - myMiddle.x);
        myDist = min (myDist, _myCityPosition.br.y - myMiddle.y);
        if (myDist < BORDER_WIDTH) {
            normedRect = PLRect (int(srcRect.tl.x/8), int(srcRect.tl.y/8),
                    int(srcRect.br.x/8), int(srcRect.br.y/8) );
            PLAnyBmp myBlendBmp;
            _myTerrainTexGen->getTexture(normedRect, resultSize, myBlendBmp);
            double myBlendFactor = double(myDist)/BORDER_WIDTH;
            blendBitmaps (resultBmp, myBlendBmp, myBlendFactor);
        }
    } else {
        normedRect = PLRect (int(srcRect.tl.x/8), int(srcRect.tl.y/8),
                int(srcRect.br.x/8), int(srcRect.br.y/8) );
        if (normedRect.Width() == 0 || normedRect.Height() == 0) {
            cerr << "Warning: Texture smaller than indexmap pixel can't be generated." << endl;
            cerr << "srcRect: (" << srcRect.tl.x << ", " << srcRect.tl.y << ") - (" <<
                                  srcRect.br.x << ", " << srcRect.br.y << ")" << endl;
        }
        _myTerrainTexGen->getTexture(normedRect, resultSize, resultBmp);

    }
/*
    resultBmp.Create (resultSize.x, resultSize.y, 32, false);
    PLPixel32 white(255,255,255,255);
    PLPixel32 black(0,0,0,255);
    PLPixel32 ** myTextureBmpLines = (PLPixel32**)resultBmp.GetLineArray();
    for (int x = 0;x<resultSize.x;x++) {
        for(int y = 0;y<resultSize.y;y++) {
               myTextureBmpLines[y][x] =  black;
          }
    }

    bool drawBlack = false;
    for (int x = 0;x<resultSize.x;) {
        for(int y = 0;y<resultSize.y;) {
            if (drawBlack) {
                for(int xx = 0;xx<resultSize.x/8;xx++) {
                    for(int yy = 0;yy<resultSize.y/8;yy++) {
                       myTextureBmpLines[y+yy][x+xx] = black;
                     }
                }
            }
            else {
                for(int xx = 0;xx<resultSize.x/8;xx++) {
                    for(int yy = 0;yy<resultSize.y/8;yy++) {
                       myTextureBmpLines[y+yy][x+xx] = white;
                     }
                }
            }
            drawBlack=!drawBlack;
            y+=(resultSize.y/8);
        }
        drawBlack=!drawBlack;
        x+=(resultSize.x/8);
    }
*/
      // fill the border with white
    if (myWhitenBorder)
    {
         PLPixel32 white(255,255,255,255);
        PLPixel32 ** myTextureBmpLines = (PLPixel32**)resultBmp.GetLineArray();


        for (int x = 0;x<resultSize.x;x++) {
            myTextureBmpLines[resultSize.y-1][x] = white;
        }
        for (int y = 0;y<resultSize.y;y++) {
            myTextureBmpLines[y][resultSize.x-1] = white;
        }
    }
}

void
TextureGenerator::blendBitmaps
        (PLBmp & myResultBmp, const PLBmp & myBlendBmp, double myBlendFactor) const
{
    assert (myResultBmp.GetWidth() == myBlendBmp.GetWidth() &&
            myResultBmp.GetHeight() == myResultBmp.GetHeight());
    for (int y = 0; y < myResultBmp.GetHeight(); y++) {
        PLPixel32 * myResultLine = myResultBmp.GetLineArray32()[y];
        PLPixel32 * myBlendLine = myBlendBmp.GetLineArray32()[y];
        for (int x = 0; x < myResultBmp.GetWidth(); x++) {
            myResultLine[x].SetR( static_cast<PLBYTE>( int(myResultLine[x].GetR()*   myBlendFactor +
                                                           myBlendLine [x].GetR()*(1-myBlendFactor))) );
            myResultLine[x].SetG( static_cast<PLBYTE>( int (myResultLine[x].GetG()*   myBlendFactor +
                                                            myBlendLine [x].GetG()*(1-myBlendFactor))) );
            myResultLine[x].SetB( static_cast<PLBYTE>( int (myResultLine[x].GetB()*    myBlendFactor +
                                                            myBlendLine [x].GetB()*(1-myBlendFactor))) );
        }
    }

}

void
TextureGenerator::kaputt (const char * msg) {
    cerr << "################################################################" << endl;
    cerr << "################################################################" << endl;
    cerr << "################################################################" << endl;
    cerr << "|  " << msg << endl;
    cerr << "|  TextureGenerator aborting.                                  |" << endl;
    cerr << "################################################################" << endl;
    cerr << "################################################################" << endl;
    cerr << "################################################################" << endl;
}

}

