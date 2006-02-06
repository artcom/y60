//============================================================================
//
// Copyright (C) 2005-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include <asl/Ptr.h>
#include <dom/ThreadingModel.h>

#ifndef _ac_y60_OffscreenBuffer_h_
#define _ac_y60_OffscreenBuffer_h_

DEFINE_EXCEPTION( OffscreenRendererException, asl::Exception );

namespace y60 {
    class Image;

    class OffscreenBuffer {
        public:        
            /**
            * Allows Offscreen rendering into a texture.
            * Renders by default into the framebuffers back buffer
            * Renders into an off screen framebuffer using the 
            * EXT_framebuffer_object GL extension if 
            * my rendercaps include y60::FRAMEBUFFER_SUPPORT
            */
            OffscreenBuffer(bool theUseGLFramebufferObject = true);
            virtual ~OffscreenBuffer() {}

            /**
            * prepares GL-things to render a scene
            * onto theTexture  
            */
            void preOffscreenRender(asl::Ptr<Image, dom::ThreadingModel> theTexture); 
            /**
            * finishes GL-things to render a scene
            * onto theTexture  
            * @param theCopyToImageFlag if true 
            * the underlying raster value is ignored
            * else the texture is copied into the raster.
            */
            void postOffscreenRender(asl::Ptr<Image, dom::ThreadingModel> theImage, 
                    bool theCopyToImageFlag = false); 

            /**
            * set to true if you want to render on a offscreen EXT_framebuffer_object
            * if false we render on the framebuffers back buffer
            */
            inline void setUseGLFramebufferObject(bool theFlag) {
                _myUseGLFramebufferObject = theFlag;
            }
            inline bool getUseGLFramebufferObject() {
                return _myUseGLFramebufferObject;
            }

        protected:
            void copyFrameBufferToImage(asl::Ptr<Image, dom::ThreadingModel> theImage);
        private:
            void copyFrameBufferToTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);
            void bindOffscreenFrameBuffer(asl::Ptr<Image, dom::ThreadingModel> theTexture);
            
            bool _myUseGLFramebufferObject;

            // OpenGl id of the frame buffer object
            unsigned _myFrameBufferObjectId; 

            // OpenGl id of the offscreen depth buffer 
            unsigned _myDepthBufferId;

            // OpenGl id of the offscreen color buffer
            unsigned _myColorBufferId;
    };
}

#endif
