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

#if !defined(INCL_TEXTUREGENERATOR)
#define INCL_TEXTUREGENERATOR

#include "TerrainTexGen.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plrect.h>
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace dom {
    class Node;
}
class PLBmp;

namespace TexGen {

class TextureGenerator
{
public:
    // TODO: The dummy parameter is here so we don't have to change
    // libRoamPerceptor/TextureManager.c++. Delete after we're sure the version
    // in the repository is going to be used!
    TextureGenerator (const dom::Node* myNode, bool dummy = true);
    virtual ~TextureGenerator();

    // Create Texture with indexmap rectangle, terrain index std::map will be used
    void createTexture (PLRect& srcRect, const PLPoint& resultSize,
                        PLBmp & resultBmp, bool myWhitenBorder = false) const;

private:
    TextureGenerator();
    TextureGenerator(const TextureGenerator &);
    const TextureGenerator & operator=(const TextureGenerator &);

    void init();
    void loadTiles (const dom::Node & myTilesNode);
    void blendBitmaps (PLBmp & myResultBmp, const PLBmp & myBlendBmp,
                       double myBlendFactor) const;
    void kaputt (const char * msg);

    TerrainTexGen *     _myTerrainTexGen;
    TerrainTexGen *     _myCityTexGen;

    TextureDefinitionMap _myTextureDefinitionMap;

    // city region in virtual coords
    PLRect              _myCityPosition;
};

}

#endif


