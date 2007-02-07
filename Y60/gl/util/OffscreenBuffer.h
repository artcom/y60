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
            * Renders to an FBO by default but can fall-back to
            * backbuffer rendering.
            */
            OffscreenBuffer();
            virtual ~OffscreenBuffer() {}

            /**
            * activate the image as render target and initializes FBO 
            * if necessary  
            */
            void activate(asl::Ptr<Image, dom::ThreadingModel> theImage,
                          unsigned theSamples = 1); 

            /**
            * deactivates the image as render target   
            * @param theCopyToImageFlag copy result to image raster.
            */
            void deactivate(asl::Ptr<Image, dom::ThreadingModel> theImage, 
                    bool theCopyToImageFlag = false); 

        protected:
            /**
            * set to true if you want to render on a offscreen EXT_framebuffer_object
            * if false we render on the framebuffers back buffer
            */
            void setUseFBO(bool theFlag);
            inline bool getUseFBO() const {
                return _myUseFBO;
            }

            void copyToImage(asl::Ptr<Image, dom::ThreadingModel> theImage);

        private:
            void copyFrameBufferToTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);

            void reset();

            void bindOffscreenFrameBuffer(asl::Ptr<Image, dom::ThreadingModel> theTexture,
                                          unsigned theSamples = 0);
            
            bool     _myUseFBO;
            unsigned _myImageNodeVersion;
            unsigned _myBlitFilter;

            // OpenGL id of frame buffer object
            unsigned _myFrameBufferObject[2];

            // OpenGL id(s) of color buffer
            unsigned _myColorBuffer[2];

            // OpenGL id(s) of depth buffer 
            unsigned _myDepthBuffer[2];
    };
}

#endif
