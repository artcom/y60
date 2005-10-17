//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

#ifndef _ac_scene_ResourceManager_h_
#define _ac_scene_ResourceManager_h_

#include <asl/Ptr.h>
#include <y60/Image.h>
#include "IShader.h"
#include "VertexData.h"

namespace y60 {

class TextureManager;
/**
 * Abstract ResourceManager used by the TextureManager to call the Rendering
 * specific ResourceManager. It also holds references to the Shaderlibrary
 * and Vertexbuffers and thereby kindof wraps the rendering context.
 *
 * @ingroup Y60scene
 */ 
class ResourceManager {
    public:
        virtual int getMaxTextureSize(int theDimensions) const = 0;
        virtual void updateTextureData(ImagePtr theImage) = 0;
        virtual void rebindTexture(ImagePtr theImage) = 0;
        virtual unsigned getMaxTextureUnits() const = 0;
        /**
         * Sets the Priority of the Texture theImage to thePriority. The Texture priority
         * is used to determine which texture should be removed from the graphics hardware
         * and which one should be there.
         * A priority of TEXTURE_PRIORITY_IDLE means that the texture can be safely removed
         * while a priority of TEXTURE_PRIORITY_IN_USE means it is currently in use.
         *
         * @param theImage Image to set the priority for
         * @param thePriority priority to set.
         */
        virtual void setTexturePriority(Image * theImage, float thePriority) = 0;
        /**
         * unbinds the Texture given in theImage from the graphics hardware.
         * @param theImage texture to unbind.
         */
        virtual void unbindTexture(Image * theImage) = 0;
        virtual unsigned setupTexture(ImagePtr theImage) = 0;

        virtual IShaderLibraryPtr getShaderLibrary() const { return IShaderLibraryPtr(0); }
        virtual void loadShaderLibrary(const std::string & theShaderLibraryFile) {};

        VertexDataFactory1f & getVertexDataFactory1f() { return _myVertexDataFactory1f; }
        VertexDataFactory2f & getVertexDataFactory2f() { return _myVertexDataFactory2f; }
        VertexDataFactory3f & getVertexDataFactory3f() { return _myVertexDataFactory3f; }
        VertexDataFactory4f & getVertexDataFactory4f() { return _myVertexDataFactory4f; }
    protected:
        VertexDataFactory1f _myVertexDataFactory1f;
        VertexDataFactory2f _myVertexDataFactory2f;
        VertexDataFactory3f _myVertexDataFactory3f;
        VertexDataFactory4f _myVertexDataFactory4f;        
    private:
};

typedef asl::Ptr<ResourceManager> ResourceManagerPtr;

} // namespace y60

#endif // _ac_scene_ResourceManager_h_

