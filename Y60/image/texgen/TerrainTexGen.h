/*
/--------------------------------------------------------------------
|
|      $Id: TerrainTexGen.h,v 1.2 2004/10/18 16:22:57 janbo Exp $
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_FILTERTEXGEN)
#define INCL_FILTERTEXGEN

#include "TextureDefinition.h"

#include <dom/Nodes.h>

//#define TEXGEN_GENERATE_STATISTICS

#ifdef TEXGEN_GENERATE_STATISTICS
#include <pm/ptime.h>
#endif

#include <paintlib/plpixel32.h>
#include <paintlib/plpoint.h>

#include <map>

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
    void TerrainTexGen::dumpRect (const std::string& myName, const PLRect& myRect);

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

