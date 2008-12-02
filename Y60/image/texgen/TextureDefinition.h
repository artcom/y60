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
//    $RCSfile: TextureDefinition.h,v $
//
//     $Author: uzadow $
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TILEDEFINITION)
#define INCL_TILEDEFINITION

#include <assert.h>
#include <vector>
#include <string>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel32.h>
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "LayerDefinition.h"

namespace dom {
    class Node;
}

namespace TexGen {


class TextureDefinition {
public:
    TextureDefinition (const dom::Node& myNode, const std::string& myDirectory, 
                       bool showProgress, 
                       bool myCreateMipMaps = false, float myIndexTextureSize = 1.0f);
    TextureDefinition (int myIndex);

    virtual ~TextureDefinition();
   
    int getLayerCount() const { return _myLayers.size(); } 
    void setTileSize (double myTileSize);
    int getCurSize () const  
        { return _myLayers[0]->getCurSize(); };
    const PLPixel32 getPixel(int x, int y) const;
    int getIndex () const;
    PLPixel32 getAvgColor () const 
    { 
        PLPixel32 answer(0,0,0,0); 
        for (std::vector<LayerDefinition*>::size_type i=0;i<_myLayers.size(); answer += _myLayers[i++]->getAvgColor() );
        return answer;
    }
    void addLayer (LayerDefinition* myLayer);
    
    const PLPixel32* getPixelLine(int x, int y) const;

private:
    TextureDefinition(const TextureDefinition &);
    const TextureDefinition & operator=(const TextureDefinition &);
    void load (const dom::Node & curNode, const std::string& myDirectory, 
               bool showProgress);   
    void createMipmaps (int myRapport);
    int _myIndex;
    std::vector<LayerDefinition*> _myLayers;
};

typedef std::map<int, TextureDefinition *> TextureDefinitionMap;

inline const PLPixel32 TextureDefinition::getPixel(int x, int y) const {
    PLPixel32 answer(0,0,0,0); 
    for (std::vector<LayerDefinition*>::size_type i=0;i<_myLayers.size(); answer += _myLayers[i++]->getPixel(x,y) );
    return answer;
}

inline const PLPixel32* TextureDefinition::getPixelLine(int x, int y) const {
    assert(_myLayers.size() == 1);
    return _myLayers[0]->getPixelLine(x,y);
}

}

#endif


