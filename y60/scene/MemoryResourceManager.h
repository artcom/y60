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

#ifndef _ac_scene_MemoryResourceManager_h_
#define _ac_scene_MemoryResourceManager_h_

#include "y60_scene_settings.h"

#include "ResourceManager.h"
#include "VertexData.h"
#include "VertexMemoryBase.h"
#include "Texture.h"


namespace y60 {

/**
 * Implementation of a ResourceManager that does not depend on any
 * rendering hardware. Use this ResourceManager to have access to
 * Vertexbuffers of a scene without actually having it on any rendering
 * hardware.
 * @ingroup Y60scene
 */
class MemoryResourceManager : public ResourceManager {
    public:
        MemoryResourceManager() {
            initVertexDataFactories();
        }

        int getMaxTextureSize(int theDimensions) const {
            return 0;
        }

        unsigned setupTexture(TexturePtr & theTexture) {
            return 0;
        }
        void updateTextureData(const TexturePtr & theTexture) {}

        /**
         * Sets the Priority of the Texture theTexture to thePriority. The Texture priority
         * is used to determine which texture should be removed from the graphics hardware
         * and which one should be there.
         * A priority of TEXTURE_PRIORITY_IDLE means that the texture can be safely removed
         * while a priority of TEXTURE_PRIORITY_IN_USE means it is currently in use.
         *
         * @param theTexture Texture to set the priority for
         * @param thePriority priority to set.
         */
        void setTexturePriority(const TexturePtr & theTexture, float thePriority) {}

        /**
         * unbinds the Texture given in theTexture from the graphics hardware.
         * @param theTexture texture to unbind.
         */
        void unbindTexture(Texture * theTexture) {}
        void updateTextureParams(const TexturePtr & theTexture) {}
        bool imageMatchesGLTexture(TexturePtr theTexture) const { return true; }
        IShaderLibraryPtr getShaderLibrary() const {
            return IShaderLibraryPtr();
        }

    private:
        template<class T>
        static asl::Ptr<VertexData<T> > create(const VertexBufferUsage & theUsage) {
            return asl::Ptr<VertexData<T> >(new VertexMemoryBase<T>());
        };

        void initVertexDataFactories() {
            _myVertexDataFactory1f.setFactoryMethod(& create<float> );
            _myVertexDataFactory2f.setFactoryMethod(& create<asl::Vector2f> );
            _myVertexDataFactory3f.setFactoryMethod(& create<asl::Vector3f> );
            _myVertexDataFactory4f.setFactoryMethod(& create<asl::Vector4f> );
        }
};

typedef asl::Ptr<MemoryResourceManager> MemoryResourceManagerPtr;

} // namespace y60

#endif // _ac_scene_MemoryResourceManager_h_
