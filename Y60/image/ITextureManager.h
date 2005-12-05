//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ITextureManager.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//  Description: This class performs texture loading and management.
//
//=============================================================================

#ifndef _ac_y60_ITextureManager_h_
#define _ac_y60_ITextureManager_h_

#include <asl/Ptr.h>

namespace y60 {
    static const char * Y60_TEXTURE_SIZE_LIMIT_ENV = "Y60_TEXTURE_SIZE_LIMIT";
    const float TEXTURE_PRIORITY_IDLE   = 0.0f;     ///< Minimum priority for Texture management
    const float TEXTURE_PRIORITY_IN_USE = 1.0f;     ///< Priority of a Texture that is currently in use.

    class Image;
    /**
     * @ingroup Y60image
     * 
     * Abstract Interface for a Texturemanager. The Interface for the TextureManager
     * is made here, because it is needed in the image module. 
     * 
     */
    class ITextureManager {
    public:
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
        virtual void setPriority(Image * theImage, float thePriority) = 0;
        /**
         * unbinds the Texture given in theImage from the graphics hardware.
         * @param theImage texture to unbind.
         */
        virtual void unbindTexture(Image * theImage) = 0;
        /**
         * @return A (weak) pointer to myself.
         */
        virtual asl::WeakPtr<ITextureManager> getSelf() const = 0;
        /**
         * @return the maximum texture size supported by the current hardware
         */
        virtual int getMaxTextureSize(int theDimensions) const = 0; 
    };

    typedef asl::Ptr<ITextureManager> ITextureManagerPtr;
}
#endif

