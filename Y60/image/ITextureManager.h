//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_ITextureManager_h_
#define _ac_y60_ITextureManager_h_

#include <asl/base/Ptr.h>
#include <asl/base/os_functions.h>
#include <asl/base/Logger.h>
#include <y60/base/typedefs.h>

namespace y60 {

    static const char * Y60_TEXTURE_SIZE_LIMIT_ENV = "Y60_TEXTURE_SIZE_LIMIT";

    class Texture;
    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;

    const unsigned DEFAULT_TEXTURE_SIZE_LIMIT = 0; // ask gl for max texture size

    /**
     * @ingroup Y60image
     * 
     * Abstract Interface for a Texturemanager. The Interface for the TextureManager
     * is made here, because it is needed in the image module. 
     */
    class ITextureManager {
    public:
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
        //virtual void setTexturePriority(const TexturePtr & theTexture, float thePriority) = 0;

        /**
         * unbinds the Texture given in theTexture from the graphics hardware.
         * @param theTexture texture to unbind.
         */
        virtual void unbindTexture(Texture * theTexture) = 0;
        /**
         * @return A (weak) pointer to myself.
         */
        virtual asl::WeakPtr<ITextureManager> getSelf() const = 0;
        /**
         * @return the maximum texture size supported by the current hardware
         */
        virtual int getMaxTextureSize(int theDimensions) const = 0; 

        /**
         * @upload image data
         */
        //virtual void uploadTexture(const TexturePtr & theTexture) = 0;
        static unsigned getTextureSizeLimit() {
            unsigned myTextureLimit=DEFAULT_TEXTURE_SIZE_LIMIT;
            std::string myLimitString;
            if (asl::get_environment_var(Y60_TEXTURE_SIZE_LIMIT_ENV, myLimitString)) {
                if (!asl::fromString(myLimitString, myTextureLimit)) {
                    AC_WARNING << "Invalid "<<Y60_TEXTURE_SIZE_LIMIT_ENV<<" environment variable, using default value = "<<DEFAULT_TEXTURE_SIZE_LIMIT;
                }
            }
            return myTextureLimit;
        }
 
    };

    typedef asl::Ptr<ITextureManager> ITextureManagerPtr;
}

#endif
