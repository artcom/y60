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

#ifndef _IRESOURCEMANAGER_INCLUDED
#define _IRESOURCEMANAGER_INCLUDED

#include "y60_base_settings.h"

#include <asl/base/Ptr.h>
#include <asl/dom/ThreadingModel.h>

#include <string>

#if defined(WIN32)
#   pragma warning( disable : 4275 )
#endif /* defined(WIN32) */

namespace y60 {

    class Texture;
    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;

    class IShaderLibrary;
    typedef asl::Ptr<IShaderLibrary> IShaderLibraryPtr;

    const float TEXTURE_PRIORITY_IDLE   = 0.0f;     ///< Minimum priority for Texture management
    const float TEXTURE_PRIORITY_IN_USE = 1.0f;     ///< Priority of a Texture that is currently in use.

    class IResourceManager {
        public:
            virtual IShaderLibraryPtr getShaderLibrary() const = 0;

            virtual unsigned applyTexture(TexturePtr & theTexture) = 0;

            virtual unsigned setupTexture(TexturePtr & theTexture) = 0;
            virtual void updateTextureData(const TexturePtr & theTexture) = 0;
            virtual void updateTextureParams(const TexturePtr & theTexture) = 0;

            /**
             * unbinds the Texture given in theTexture from the graphics hardware.
             * @param theTexture texture to unbind.
             */
            virtual void unbindTexture(Texture * theTexture) = 0;

            /**
             * Sets the Priority of the Texture theTexture to thePriority. The Texture priority
             * is used to determine which texture can be removed from the graphics hardware
             * and which ones should stay resident.
             * A priority of TEXTURE_PRIORITY_IDLE means that the texture can be safely removed
             * while a priority of TEXTURE_PRIORITY_IN_USE means it is currently in use.
             *
             * @param theTexture Texture to set the priority for
             * @param thePriority priority to set.
             */
            virtual void setTexturePriority(const TexturePtr & theTexture,
                                            float thePriority) = 0;
            virtual int getMaxTextureSize(int theDimensions) const = 0;
            virtual bool imageMatchesGLTexture(TexturePtr theTexture) const  = 0;
            virtual ~IResourceManager() {}
    };

    typedef asl::Ptr<IResourceManager,dom::ThreadingModel> IResourceManagerPtr;

}
#endif
