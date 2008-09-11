//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _IRESOURCEMANAGER_INCLUDED
#define _IRESOURCEMANAGER_INCLUDED

#include <asl/base/Ptr.h>
#include <asl/dom/ThreadingModel.h>

#include <string>

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
    };

    typedef asl::Ptr<IResourceManager,dom::ThreadingModel> IResourceManagerPtr;

}
#endif
