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

#ifndef _ac_y60_ITextureManager_h_
#define _ac_y60_ITextureManager_h_

#include "y60_image_settings.h"

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
    class Y60_IMAGE_DECL  ITextureManager {
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
        virtual ~ITextureManager() {}

    };

    typedef asl::Ptr<ITextureManager> ITextureManagerPtr;
}

#endif
