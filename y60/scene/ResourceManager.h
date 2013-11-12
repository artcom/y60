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

#ifndef _ac_scene_ResourceManager_h_
#define _ac_scene_ResourceManager_h_

#include "y60_scene_settings.h"

#include <y60/base/IResourceManager.h>

#include <asl/base/Ptr.h>
#include "IShader.h"
#include "VertexData.h"

namespace y60 {

    class Texture;
    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;

    /**
     * Abstract ResourceManager used by the TextureManager to call the Rendering
     * specific ResourceManager. It also holds references to the Shaderlibrary
     * and Vertexbuffers and thereby kindof wraps the rendering context.
     *
     * @ingroup Y60scene
     */
    class Y60_SCENE_DECL ResourceManager : public IResourceManager {
        public:
            ResourceManager() :_myHaveValidGLContextFlag(false) {}
            virtual IShaderLibraryPtr getShaderLibrary() const { return IShaderLibraryPtr(); }
            virtual bool hasGLContext() const { return _myHaveValidGLContextFlag;}
            virtual void validateGLContext(bool theFlag) { _myHaveValidGLContextFlag = theFlag;}

#if 0
            virtual void loadShaderLibrary(const std::string & theShaderLibraryFile,
                    const std::string & theVertexProfileName,
                    const std::string & theFragmentProfileName) = 0;
            virtual void prepareShaderLibrary(const std::string & theShaderLibraryFile,
                    const std::string & theVertexProfileName,
                    const std::string & theFragmentProfileName) = 0;
            virtual void loadShaderLibrary() = 0;
#endif

            virtual unsigned applyTexture(TexturePtr & theTexture) { return 0; }
            virtual void updateTextureParams(const TexturePtr & theTexture) {}

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
            bool _myHaveValidGLContextFlag;

    };

    typedef asl::Ptr<ResourceManager> ResourceManagerPtr;

} // namespace y60

#endif // _ac_scene_ResourceManager_h_
