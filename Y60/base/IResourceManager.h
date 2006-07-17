//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: IResourceManager.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: IResourceManager.h,v $
//   $Author: valentin $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//=============================================================================

#ifndef _IRESOURCEMANAGER_INCLUDED
#define _IRESOURCEMANAGER_INCLUDED

#include <asl/Ptr.h>
#include <dom/ThreadingModel.h>

#include <string>

namespace y60 {
    class Image;
    typedef asl::Ptr<Image, dom::ThreadingModel> ImagePtr;
    class IShaderLibrary;
    typedef asl::Ptr<IShaderLibrary>   IShaderLibraryPtr; 
    class IResourceManager {
        public:

            virtual IShaderLibraryPtr getShaderLibrary() const = 0;
            virtual void loadShaderLibrary(const std::string & theShaderLibraryFile)  = 0;
        
            virtual void updateTextureData(ImagePtr theImage) = 0;

            /**
             * unbinds the Texture given in theImage from the graphics hardware.
             * @param theImage texture to unbind.
             */
            virtual void unbindTexture(Image * theImage) = 0;
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
            virtual unsigned setupTexture(ImagePtr theImage) = 0;
            virtual int getMaxTextureSize(int theDimensions) const = 0;
            //virtual void rebindTexture(ImagePtr theImage) = 0;

    };

    typedef asl::Ptr<IResourceManager,dom::ThreadingModel> IResourceManagerPtr;

}
#endif
