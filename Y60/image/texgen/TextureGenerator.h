/*
/--------------------------------------------------------------------
|
|      $Id: TextureGenerator.h,v 1.1 2004/10/06 10:17:14 uzadow Exp $
|
\--------------------------------------------------------------------
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


