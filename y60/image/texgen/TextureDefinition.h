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


