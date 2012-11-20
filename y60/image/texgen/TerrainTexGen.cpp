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
//    $RCSfile: TerrainTexGen.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.3 $
//
//
// Description:
//
//=============================================================================

#include "TerrainTexGen.h"

#include <assert.h>
#include <stdexcept>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <math.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/plpoint.h>
#include <paintlib/plrect.h>
#include <paintlib/Filter/plfiltercrop.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#include <paintlib/Filter/plfilterfill.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/base/settings.h>
#include <asl/base/string_functions.h>
#include <asl/dom/Nodes.h>

#include "TextureDefinition.h"
#include "XmlHelper.h"
#include "LitTerrainTexGen.h"
#include "FilterIntDownscale.h"


using namespace asl;
using namespace std;

namespace TexGen {

const int MAX_TEXTURE_SIZE=2048;

TerrainTexGen::TerrainTexGen (const TextureDefinitionMap & myTextureDefinitionMap,
        double myBlendRadius, PLAnyBmp * myIndexBmp,
        PLAnyBmp * myAttenuationMapBmp,
        int myScale)
    : _myTextureDefinitionMap (myTextureDefinitionMap),
      _myBlendRadius (myBlendRadius),
      _myIndexBmp (myIndexBmp),
      _myAttenuationMapBmp (myAttenuationMapBmp),
      _myScale(myScale),
      _myLightingFactor(4)
{
    if (_myAttenuationMapBmp) {
        assert(_myIndexBmp->GetWidth() == _myAttenuationMapBmp->GetWidth() &&
               _myIndexBmp->GetHeight() == _myAttenuationMapBmp->GetHeight() );
    }
    if (_myTextureDefinitionMap.empty()) {
        throw out_of_range ("No texture bitmaps available.");
    }

    init();
}

TerrainTexGen::TerrainTexGen (const TextureDefinitionMap & myTextureMap,
            const dom::Node& myMapNode, int myScale)
    : _myScale(myScale), _myTextureDefinitionMap(myTextureMap), _myAttenuationMapBmp(0)
{
    _myIndexBmp = new PLAnyBmp;
    _myBlendRadius = getFloatXMLParam (myMapNode, "blend");
    if (_myBlendRadius > 0.5 || _myBlendRadius < 0) {
        throw (invalid_argument ("Blend radius must be < 0.5 and >= 0"));
    }

    _myLightingFactor = getFloatXMLParam (myMapNode, "lightingfactor");
    if (_myLightingFactor < 1) {
        cerr << "WARNING: Lighting set to very low value." << endl;
    }

    string myMapDirectory = getRequiredXMLField(myMapNode, "directory");
    myMapDirectory += "/";
    string myFileName  = myMapNode("Indexmap")("#text").nodeValue();
    loadBitmap (myMapNode, myMapDirectory + myFileName, _myIndexBmp);

    PLAnyBmp myLightMapBmp;
    if (myMapNode("Lightmap")) {
       myFileName  = myMapNode("Lightmap")("#text").nodeValue();
       loadBitmap (myMapNode, myMapDirectory + myFileName, &myLightMapBmp);
    }
    if (myMapNode("Attenuationmap")) {
         _myAttenuationMapBmp = new PLAnyBmp;
         myFileName  = myMapNode("Attenuationmap")("#text").nodeValue();
         loadBitmap (myMapNode, myMapDirectory + myFileName, _myAttenuationMapBmp);
         if (myLightMapBmp.GetWidth() != _myAttenuationMapBmp->GetWidth() ||
                 myLightMapBmp.GetHeight() != _myAttenuationMapBmp->GetHeight()) {
             string s;
             s = string("Lightmap (") + myMapNode("Lightmap")("#text").nodeValue() + ", "+
                 as_string(myLightMapBmp.GetWidth()) + "x" +
                 as_string(myLightMapBmp.GetHeight()) +
                 ") \nand Attenuationmap (" + myMapNode ("Attenuationmap")("#text").nodeValue()+ ", "+
                 as_string(_myAttenuationMapBmp->GetWidth()) + "x" +
                 as_string(_myAttenuationMapBmp->GetHeight()) +
                 ") have different sizes";
             throw (invalid_argument(s));
         }
         // build attenuation map
         generateMultiplicationMap(myLightMapBmp, *_myAttenuationMapBmp);

         // Optional: Shadow map
         if (myMapNode("Shadowmap")) {
             PLAnyBmp myShadowMapBmp;
             myFileName  = myMapNode("Shadowmap")("#text").nodeValue();
             loadBitmap (myMapNode, myMapDirectory + myFileName, &myShadowMapBmp);
             generateMultiplicationMap (myShadowMapBmp, *_myAttenuationMapBmp);
         }
    }
    init();
}

TerrainTexGen::~TerrainTexGen() {

    for (std::vector<PLBmp*>::size_type i=0; i<_myMipMaps.size(); i++)
        delete _myMipMaps[i];

    delete _myIndexBmp;
    delete _myAttenuationMapBmp;
    delete _myMissingTexDef;
#ifdef TEXGEN_GENERATE_STATISTICS
    printStatistic();
#endif
}

void
TerrainTexGen::init() {
    createPalette();
    createMipmaps();

    _myLastTextureIndex = -1;
    _myLastTextureDefinition = NULL;

    _myMissingTexDef = new TextureDefinition (-1);
    _myMissingTexDef->addLayer
            (new LayerDefinition (PLPixel32 (255,0,0), 1.0f));

}

void
TerrainTexGen::loadBitmap (const dom::Node& myIndexNode,
                           const std::string& myFileName, PLBmp* myBmp)
{
    PLAnyPicDecoder myDecoder;
    cout << "Loading bitmap " << myFileName << "..." <<  endl;
    myDecoder.MakeBmpFromFile (myFileName.c_str(), myBmp);
}

#ifdef TEXGEN_GENERATE_STATISTICS
void
TerrainTexGen::printStatistic() const {
    // log statistics
    {
        cout <<"#########################"<<endl;
        std::map<int,Statistic>::iterator iter;
        for (iter=_myStatisticMap.begin(); iter != _myStatisticMap.end(); ++iter) {
            cout <<"Anfrage: " <<endl;
            cout <<"   Ratio:         "<<float(iter->first)/256.0<<endl;
            cout <<"   Counter:       "<<(iter->second.counter)<<endl;
            cout <<"   total Time   : "<<(iter->second.time)<<endl;
            cout <<"   avg.  Time   : "<<
                   float(iter->second.time)/(iter->second.counter)<<endl;
        }
        cout <<"#########################"<<endl;
     }
}
#endif

TextureDefinition * TerrainTexGen::getSafeTextureDefinition
        (int x, int y, double theScaleFactor) const
{
    if (x<0 || y<0 || x>=_myIndexBmp->GetWidth() || y>=_myIndexBmp->GetHeight()) {
        return NULL;
    }
    int theIndex = (_myIndexBmp->GetLineArray()[y])[x];
    // WARNING
    // Do not cache a texturedefinition with one const size
    // and return this sized texturedefinition by the next
    // index,  maybe demanding a DIFFERENT SIZE.
    // OR resize as well
    if (theIndex == _myLastTextureIndex) {
        _myLastTextureDefinition->setTileSize (theScaleFactor * double(_myScale));
        return _myLastTextureDefinition;
    } else {
        TextureDefinitionMap::const_iterator curDefinitionIter;
        curDefinitionIter = _myTextureDefinitionMap.find(theIndex);

        // Check if a texture bitmap with this index is available.
        if (curDefinitionIter == _myTextureDefinitionMap.end()) {
            _myMissingTexDef->setTileSize (theScaleFactor * double(_myScale));
            return _myMissingTexDef;
        } else {
            // scale this texture
            curDefinitionIter->second->setTileSize (theScaleFactor * double(_myScale));

            _myLastTextureIndex = theIndex;
            _myLastTextureDefinition = curDefinitionIter->second;
            return curDefinitionIter->second;
        }
    }
}

void TerrainTexGen::createNonBlendArea (const PLRect & theArea,
                                        const TextureDefinition * theTextureDefinition,
                                        const PLPoint & theTileOffset,
                                        PLBmp& resultBmp) const
{
    if (theArea.Width() > 0) {
        for (int y = theArea.tl.y; y < theArea.br.y; ++y) {

            if (theTextureDefinition->getLayerCount() == 1 &&
                theArea.Width() <= theTextureDefinition->getCurSize())
            {
                PLPixel32* resultLine =
                    (PLPixel32 *)(resultBmp.GetLineArray()[y]);
                const PLPixel32* textureLine =
                    theTextureDefinition->getPixelLine(theTileOffset.x+theArea.tl.x,
                            y+theTileOffset.y);
                memcpy(resultLine+theArea.tl.x, textureLine, theArea.Width()*4);
            } else {
                for (int x = theArea.tl.x; x < theArea.br.x; ++x) {
                    int theTileX = x+theTileOffset.x;
                    int theTileY = y+theTileOffset.y;
                    resultBmp.SetPixel (x,y, theTextureDefinition->getPixel(theTileX, theTileY));
                }
            }
        }
    }
}

void TerrainTexGen::createBlendedQuadrant (TextureDefinition*(& theDefinitions)[2][2],
                                              const PLRect & theDestRect,
                                              const PLRect & theBlendRect,
                                              const PLPoint & theTileOffset,
                                              PLBmp & resultBmp) const
{

    int myXWeights[MAX_TEXTURE_SIZE];
    assert (theDestRect.Width()< MAX_TEXTURE_SIZE);
    bool bBlendTop = (theDefinitions[0][0] != theDefinitions[0][1]);
    bool bBlendBottom = (theDefinitions[1][0] != theDefinitions[1][1]);

    for (int i=0; i<theDestRect.Width(); i++) {
        myXWeights[i] = (256*(theBlendRect.br.x-i-theDestRect.tl.x))/theBlendRect.Width();
        if (myXWeights[i] < 0) {
            myXWeights[i] = 0;
        }
        if (myXWeights[i] > 256) {
            myXWeights[i] = 256;
        }
    }

    for (int y=theDestRect.tl.y; y < theDestRect.br.y; ++y) {

        int curYWeight = (256*(theBlendRect.br.y-y))/theBlendRect.Height();
        if (curYWeight < 0) {
            curYWeight = 0;
        }
        if (curYWeight > 256) {
            curYWeight = 256;
        }
        int theTileY = y+theTileOffset.y;
        int theTileX = theDestRect.tl.x+theTileOffset.x;
        PLPixel32 topColor;
        PLPixel32 bottomColor;
        for (int x=theDestRect.tl.x; x < theDestRect.br.x; ++x) {
            if (bBlendTop) {
                topColor = PLPixel32::Blend (
                        myXWeights[x-theDestRect.tl.x],
                        theDefinitions[0][0]->getPixel (theTileX, theTileY),
                        theDefinitions[0][1]->getPixel (theTileX, theTileY));
            } else {
                topColor = theDefinitions[0][0]->getPixel (theTileX, theTileY);
            }

            if (bBlendBottom) {
                bottomColor = PLPixel32::Blend (
                        myXWeights[x-theDestRect.tl.x],
                        theDefinitions[1][0]->getPixel (theTileX, theTileY),
                        theDefinitions[1][1]->getPixel (theTileX, theTileY));
            } else {
                bottomColor = theDefinitions[1][0]->getPixel (theTileX, theTileY);
            }
            resultBmp.SetPixel (x,y,
                    PLPixel32::Blend (curYWeight, topColor, bottomColor));
            theTileX++;
        }
    }
}

void TerrainTexGen::createBlendedTile (const PLPoint& theSrcOffset,
        const PLPoint& theTilePos,
        double theXScaleFactor,
        double theYScaleFactor,
        PLBmp & resultBmp) const
{
    PLPoint theSrcTilePos (theTilePos.x-theSrcOffset.x, theTilePos.y-theSrcOffset.y);

    PLRect tileRect(int(theXScaleFactor*theSrcTilePos.x),
                    int(theYScaleFactor*theSrcTilePos.y),
                    int(theXScaleFactor*(theSrcTilePos.x+1)),
                    int(theYScaleFactor*(theSrcTilePos.y+1)));

    // in case of scaleFactors below 1.0, like 0.5 !!!! ;-)
    // tileRect dimensions can be zero (x1==x2 && y1==y2)-> act

    if (tileRect.Width()==0){
        tileRect.br.x = tileRect.br.x + 1;
    }
    if (tileRect.Height()==0){
        tileRect.br.y = tileRect.br.y + 1;
    }


    PLPoint curTileOffset (int(theXScaleFactor*theSrcOffset.x),
                           int(theYScaleFactor*theSrcOffset.y));

    PLPoint theBlendDist (int(_myBlendRadius*tileRect.Width()),
            int(_myBlendRadius*tileRect.Height()));
    if ((theBlendDist.x == 0) || theXScaleFactor < 1.01) {
        TextureDefinition* texture = getSafeTextureDefinition (theTilePos.x, theTilePos.y,
                theXScaleFactor);
        createNonBlendArea (tileRect,
                            texture,
                            curTileOffset,
                            resultBmp);
    } else {
        // Create 3x3 Matrix of the relevant tile definitions.
        TextureDefinition* theDefinitionMatrix[3][3];
        int y;
        int x;
        for (y=0; y<3; ++y) {
            for (int x=0; x<3; ++x) {
                TextureDefinition * curDefinition = getSafeTextureDefinition (
                                                     theTilePos.x+x-1, theTilePos.y+y-1,
                                                     theXScaleFactor);
                if (curDefinition == NULL) {
                    // For the edges of the image, we use the middle tile.
                    curDefinition = getSafeTextureDefinition (theTilePos.x,
                                                              theTilePos.y,
                                                              theXScaleFactor);
                }
                theDefinitionMatrix[y][x] = curDefinition;
            }
        }

        // for each quadrant do...
        for (y=0; y<2; ++y) {
            for (x=0; x<2; ++x) {
                // Find relevant 4 tiles
                TextureDefinition * curDefinitionMatrix[2][2];
                curDefinitionMatrix[0][0] = theDefinitionMatrix[y][x];
                curDefinitionMatrix[0][1] = theDefinitionMatrix[y][x+1];
                curDefinitionMatrix[1][0] = theDefinitionMatrix[y+1][x];
                curDefinitionMatrix[1][1] = theDefinitionMatrix[y+1][x+1];

                // Calculate quadrant boundaries
                PLRect curDestRect (tileRect.tl.x+(x*tileRect.Width())/2,
                                    tileRect.tl.y+(y*tileRect.Height())/2,
                                    tileRect.tl.x+((x+1)*tileRect.Width())/2,
                                    tileRect.tl.y+((y+1)*tileRect.Height())/2);

                if ( (curDefinitionMatrix[0][0] == curDefinitionMatrix[0][1] &&
                      curDefinitionMatrix[0][0] == curDefinitionMatrix[1][0] &&
                      curDefinitionMatrix[0][0] == curDefinitionMatrix[1][1]))  {
                    // All tiles are the same -> no blending nessesary
                    createNonBlendArea (curDestRect, curDefinitionMatrix[0][0],
                                        curTileOffset, resultBmp);
                }
                else
                {
                    // Calculate blend coordinates
                    PLPoint curCorner (tileRect.tl.x+x*tileRect.Width(),
                            tileRect.tl.y+y*tileRect.Height());
                    PLRect curBlendRect (curCorner-theBlendDist,curCorner+theBlendDist);

                    createBlendedQuadrant (curDefinitionMatrix, curDestRect,
                            curBlendRect, curTileOffset, resultBmp);
                }
            }
        }
    }
}

void TerrainTexGen::createSmallTexture (const PLRect& srcRect,
                                        const PLPoint& resultSize,
                                        PLBmp & resultBmp) const
{
    PLAnyBmp theTempBmp;
    assert (srcRect.Width() > 0 && srcRect.Height() > 0);
    if (srcRect.tl.x == 0 && srcRect.tl.y == 0 &&
        srcRect.Width() == _myIndexBmp->GetWidth() &&
        srcRect.Height() == _myIndexBmp->GetHeight()) {
        cout << ".";
        resultBmp.CreateCopy (*_myIndexBmp, PLPixelFormat::X8R8G8B8);
    } else {
        PLAnyBmp theTempBmp;
        cout << ".";
        theTempBmp.CreateFilteredCopy (*_myIndexBmp, PLFilterCrop (srcRect));
        resultBmp.CreateCopy (theTempBmp, PLPixelFormat::X8R8G8B8);
    }
    cout << "." ;
    resultBmp.ApplyFilter (PLFilterResizeBilinear(resultSize.x, resultSize.y));
}


void TerrainTexGen::createLargeTexture (const PLRect& srcRect,
        double theXScaleFactor,
        double theYScaleFactor,
        PLBmp & resultBmp) const
{
    if ((theXScaleFactor-floor(theXScaleFactor) >0.001) && (theXScaleFactor>1.0) ||
        ((1.0f/theXScaleFactor-floor(1.0f/theXScaleFactor) >0.001)&&(theXScaleFactor<1.0)) )
        cout <<"theXScaleFactor: "<<theXScaleFactor<<" not an integer"<<endl;

    for (int y=srcRect.tl.y; y<srcRect.br.y; ++y) {
        for (int x=srcRect.tl.x; x<srcRect.br.x; ++x) {
            createBlendedTile (srcRect.tl, PLPoint(x, y),
                               theXScaleFactor, theYScaleFactor, resultBmp);
        }
    }
}

void TerrainTexGen::createPartialTexture (const PLRect& srcRect,
        const PLPoint resultSize,
        PLBmp & resultBmp) const
{
    assert(srcRect.tl.x >=0 && srcRect.tl.y >=0 &&
           srcRect.br.x <= _myIndexBmp->GetWidth() &&
           srcRect.br.y <= _myIndexBmp->GetHeight());
    assert (srcRect.Width() > 0 && srcRect.Height() > 0);
    double theXScaleFactor = double(resultSize.x)/srcRect.Width();
    double theYScaleFactor = double(resultSize.y)/srcRect.Height();

//    if (theXScaleFactor > 0.125 || theYScaleFactor > 0.125) {
    if (theXScaleFactor > 0.25 || theYScaleFactor > 0.25) {
         // upscaling
        resultBmp.Create (resultSize.x, resultSize.y, PLPixelFormat::X8R8G8B8);
        createLargeTexture(srcRect, theXScaleFactor, theYScaleFactor,
                           resultBmp);
    } else {
         // downscaling
        createSmallTexture(srcRect, resultSize, resultBmp);
    }
}


void TerrainTexGen::createTexture (const PLPoint & resultSize,
                                   PLBmp & resultBmp) const
{
    PLRect srcRect (0,0, _myIndexBmp->GetWidth(), _myIndexBmp->GetHeight());
    createPartialTexture (srcRect, resultSize, resultBmp);
}


void TerrainTexGen::createPalette () const {
    for (int i=0; i<256; i++) {
        _myIndexBmp->SetPaletteEntry ( static_cast<PLBYTE>(i), 255, 0, 0, 255);
    }
    for (TextureDefinitionMap::const_iterator iter=_myTextureDefinitionMap.begin();
         iter != _myTextureDefinitionMap.end();
         ++iter)
    {
        TextureDefinition* curDefinition = iter->second;
        PLPixel32 curColor = curDefinition->getAvgColor();
        _myIndexBmp->SetPaletteEntry(static_cast<PLBYTE>(iter->first),
                                     curColor.GetR(), curColor.GetG(), curColor.GetB(),
                                     255 );
    }

}

void TerrainTexGen::getTexture (const PLRect& mySrcRect,
        const PLPoint& resultSize,
        PLBmp & resultBmp)
{
    PLRect srcRect = mySrcRect;
    srcRect.tl.x *= _myScale;
    srcRect.tl.y *= _myScale;
    srcRect.br.x *= _myScale;
    srcRect.br.y *= _myScale;

#ifdef TEXGEN_GENERATE_STATISTICS
    ptime curTime;
    double t = (curTime-_myRunTime);
    //cout <<t<<endl;
    if (t >20.0) {
        printStatistic();
        _myRunTime.SetNow();
    }
    ptime StartTime;
#endif

    assert (resultSize.x == resultSize.y);
    double Scale = srcRect.Width()/resultSize.x;
    double logscale =  log (double(Scale))/log(2.0);
    double epsilon = fabs(logscale-floor(logscale+0.5));
    if ((logscale > 1) && (epsilon<0.001)) {
        assert (logscale < _myMipMaps.size()+2);
        int curMipMapLevel = (_myMipMaps.size()+1)-int (floor (logscale+0.5));
        PLBmp * curMipMap = _myMipMaps[curMipMapLevel];
        PLRect destRect (int(srcRect.tl.x/Scale), int(srcRect.tl.y/Scale),
                         int(srcRect.br.x/Scale), int(srcRect.br.y/Scale));
        assert (destRect.Width() > 0 && destRect.Height() > 0);
        resultBmp.CreateFilteredCopy (*curMipMap, PLFilterCrop (destRect));
     } else {
        if (logscale >1)
            cerr <<"Strange scale"<<endl;
        reallyCreateTexture (srcRect, resultSize, resultBmp);
    }

#ifdef TEXGEN_GENERATE_STATISTICS
    ptime EndTime;
    double time = (EndTime - StartTime);
    int ratio = ((srcRect.Height()*256) / resultSize.x);
    Statistic myStat;
     if (_myStatisticMap.find(ratio) !=_myStatisticMap.end()) {
        myStat = _myStatisticMap[ratio];
        myStat.counter++;
        myStat.time += time;
     }
    else {
        myStat.time = time;
        myStat.counter = 1;
    }
    _myStatisticMap[ratio] = myStat;
#endif
}

void TerrainTexGen::createMipmaps () {
    assert (_myMipMaps.empty());
    if (_myIndexBmp->GetWidth() > 2048) {
        PLRect mySrcRect (0, 0, _myIndexBmp->GetWidth(), _myIndexBmp->GetHeight());
        int i;
        int myMipmapCount = int(floor(log(double(min(_myIndexBmp->GetHeight(),
                                              _myIndexBmp->GetWidth())+0.5))
                                     /log(2.0)) -1);
        for (i=0; i<myMipmapCount; i++) {
            _myMipMaps.push_back(new PLAnyBmp());
        }
        cerr <<"Creating MipMaps "<<endl;
        // Create the biggest mipmap.
        PLBmp * curMipMap = _myMipMaps[myMipmapCount-1];
        PLPoint resultSize (_myIndexBmp->GetWidth()/4,_myIndexBmp->GetHeight()/4);
        cerr << myMipmapCount-1 << ":" << 4;
        curMipMap->CreateFilteredCopy (*_myIndexBmp, FilterIntDownscale (4));
        applyAttenuationMap (mySrcRect, resultSize, *curMipMap);

        for (i=myMipmapCount-2; i>=0; i--) {
            int curFactor = int (pow (2.0,(myMipmapCount-i+1)));
            cerr <<"."<<i<<":"<<curFactor;
            PLAnyBmp theTempBmp;
            theTempBmp.CreateCopy (*_myMipMaps[i+1]);
            _myMipMaps[i]->CreateCopy (theTempBmp, PLPixelFormat::X8R8G8B8);
            _myMipMaps[i]->ApplyFilter (
                    PLFilterResizeBilinear(_myIndexBmp->GetWidth()/curFactor,
                                           _myIndexBmp->GetHeight()/curFactor) );
        }
        cerr <<" done"<<endl;
    }
}

void TerrainTexGen::reallyCreateTexture (const PLRect& srcRect,
        const PLPoint& resultSize,
        PLBmp & resultBmp)
{
    createPartialTexture(srcRect, resultSize, resultBmp);

    applyAttenuationMap (srcRect, resultSize, resultBmp);
}

void TerrainTexGen::applyAttenuationMap (const PLRect& srcRect, const PLPoint& resultSize,
        PLBmp & resultBmp)
{
    if (_myAttenuationMapBmp) {
        applyMultiplicationMap(resultBmp, *_myAttenuationMapBmp,
                srcRect, _myLightingFactor);
    }
}

void TerrainTexGen::dumpRect (const string& myName, const PLRect& myRect) {
    cout << myName << " (" << myRect.tl.x << ", " << myRect.tl.y << ") - ("
         << myRect.br.x << ", " << myRect.br.y << ")" << endl;
}

}
