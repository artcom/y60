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

#if !defined(INCL_FILTERTEXGEN)
#define INCL_FILTERTEXGEN

#include <map>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel32.h>
#include <paintlib/plpoint.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "TextureDefinition.h"

#include <asl/dom/Nodes.h>

//#define TEXGEN_GENERATE_STATISTICS

#ifdef TEXGEN_GENERATE_STATISTICS
#include <pm/ptime.h>
#endif

class PLBmp;
class PLAnyBmp;
class PLRect;

namespace TexGen {

#ifdef TEXGEN_GENERATE_STATISTICS

class Statistic {
    public:
       Statistic() : time(0.0f), counter(0) {}
       double time;
       int counter;
};

#endif

class TerrainTexGen
{
public:
    TerrainTexGen (const TextureDefinitionMap & myTextureMap,
            double myBlendRadius, PLAnyBmp * myIndexBmp,
            PLAnyBmp * myAttenuationMapBmp, int myScale);
    TerrainTexGen (const TextureDefinitionMap & myTextureMap, const dom::Node& myMapNode,
            int myScale);

    virtual ~TerrainTexGen();
    void createTexture (const PLPoint& resultSize,
                        PLBmp & resultBmp) const;
    void createPartialTexture (const PLRect& srcRect,
                               const PLPoint resultSize, PLBmp & resultBmp) const;

    void createMipmaps ();
    void reallyCreateTexture (const PLRect& srcRect,
                              const PLPoint& resultSize,
                              PLBmp & resultBmp);
    void getTexture (const PLRect& srcRect, const PLPoint& resultSize,
            PLBmp & resultBmp);

  private:
    TerrainTexGen();
    TerrainTexGen(const TerrainTexGen &);
    const TerrainTexGen & operator=(const TerrainTexGen &);

    // Initialization.
    TextureDefinition * loadTextureDefinition (const dom::Node & myNode, int * theIndex);
    void init();
//    void initTextures (const xml::Node &myTextureNode,
//            const string& myDirectory);
    void loadBitmap (const dom::Node &myIndexNode,
        const std::string& myFileName, PLBmp* myBmp);

    // Algorithm
    TextureDefinition * getSafeTextureDefinition (int x, int y,
                                                  double theScaleFactor) const;
    void createNonBlendArea (const PLRect & theArea,
            const TextureDefinition * theTextureDefinition,
            const PLPoint & theTileOffset,
            PLBmp& resultBmp) const;
    void createBlendedQuadrant (TextureDefinition*(& theDefinitions)[2][2],
            const PLRect & theDestRect,
            const PLRect & theBlendRect,
            const PLPoint & theTileOffset,
            PLBmp & resultBmp) const;
    void createBlendedTile (const PLPoint& theSrcOffset,
            const PLPoint& theTilePos,
            double theXScaleFactor,
            double theYScaleFactor,
            PLBmp & resultBmp) const;
    void createLargeTexture (const PLRect& srcRect,
            double theXScaleFactor,
            double theYScaleFactor,
            PLBmp & resultBmp) const;
    void createSmallTexture (const PLRect& srcRect,
            const PLPoint& resultSize,
            PLBmp & resultBmp) const;
    void createPalette () const;
    void applyAttenuationMap (const PLRect& srcRect, const PLPoint& resultSize,
        PLBmp & resultBmp) ;
    void dumpRect (const std::string& myName, const PLRect& myRect);

    const TextureDefinitionMap& _myTextureDefinitionMap;
    TextureDefinition * _myMissingTexDef;  // Red error texture.

    double _myBlendRadius;

    PLAnyBmp* _myIndexBmp;
    PLAnyBmp* _myAttenuationMapBmp;

    std::vector<PLBmp*> _myMipMaps;
    int                 _myScale;
    double              _myLightingFactor;

    mutable int                 _myLastTextureIndex;
    mutable TextureDefinition * _myLastTextureDefinition;

#ifdef TEXGEN_GENERATE_STATISTICS
    mutable std::map<int, Statistic> _myStatisticMap;
    mutable ptime _myRunTime;
    void printStatistic() const;

#endif
};

}

#endif

