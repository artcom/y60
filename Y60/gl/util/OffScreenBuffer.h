
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

#include <asl/Ptr.h>
#include <dom/ThreadingModel.h>

#ifndef _ac_y60_OffScreenBuffer_h_
#define _ac_y60_OffScreenBuffer_h_


namespace y60 {
    class Image;

    class OffScreenBuffer {
        public:

        inline void setGLExtensionUsage(bool theFlag) {
            _myGLExtensionUsageFlag = theFlag;
        }
        inline bool getGLExtensionUsage() {
            return _myGLExtensionUsageFlag;
        }
        
        /**
         * Allows Offscreen rendering into a texture.
         * Renders by default into the frame buffers back buffer
         * Renders into an off screen framebuffer using the 
         * EXT_framebuffer_object GL extension if 
         * my rendercaps include y60::FRAMEBUFFER_SUPPORT
         */
        OffScreenBuffer(bool theGLExtensionUsageFlag = true);
        virtual ~OffScreenBuffer() {}

        /**
         * prepares GL-things to render a scene
         * onto theTexture  
         */
        void preOffScreenRender(asl::Ptr<Image, dom::ThreadingModel> theTexture); 
        /**
         * finishes GL-things to render a scene
         * onto theTexture  
         * @param theCopyToImageFlag if true 
         * the underlying raster value is ignored
         * else the texture is copied into the raster.
         */
        void postOffScreenRender(asl::Ptr<Image, dom::ThreadingModel> theImage, 
                bool theCopyToImageFlag = false); 

    private:
        void copyTextureToImage(asl::Ptr<Image, dom::ThreadingModel> theImage);
        void copyFrameBufferToTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);
        void bindTexture(asl::Ptr<Image, dom::ThreadingModel> theTexture);
        void bindOffScreenFrameBuffer(asl::Ptr<Image, dom::ThreadingModel> theTexture);
        
        bool _myGLExtensionUsageFlag;
        unsigned _myOffScreenBuffer ,_myDepthBuffer;
    };
}

#endif
