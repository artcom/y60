//============================================================================
// Copyright (C) 2005-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include <asl/base/Ptr.h>
#include <asl/dom/ThreadingModel.h>

#include "y60_glutil_settings.h"

#include <GL/glew.h>

#ifndef _ac_y60_OffscreenBuffer_h_
#define _ac_y60_OffscreenBuffer_h_

DEFINE_EXCEPTION( OffscreenRendererException, asl::Exception );

namespace y60 {
    class Texture;
    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;

    class Y60_GLUTIL_EXPORT OffscreenBuffer {
        public:        
            /**
            * Allows Offscreen rendering into a texture.
            * Renders to an FBO by default but can fall-back to
            * backbuffer rendering.
            */
            OffscreenBuffer();
            virtual ~OffscreenBuffer();

            /**
            * activate the texture as render target and initializes FBO 
            * if necessary  
            */
            void activate(TexturePtr theTexture,
                          unsigned theSamples = 1, unsigned theCubemapFace = 0); 

            /**
            * deactivates the texture as render target   
            * @param theCopyToImageFlag copy result to texture raster.
            */
            void deactivate(TexturePtr theTexture, 
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

            void copyToImage(TexturePtr theTexture);

        private:
            void copyFrameBufferToTexture(TexturePtr theTexture);

            void reset();

            void bindOffscreenFrameBuffer(TexturePtr theTexture,
                                          unsigned theSamples = 0, unsigned theCubemapFace = 0);
            void attachCubemapFace(unsigned theCubemapFace);
            
            bool     _myUseFBO;
            bool     _myHasFBO;
            bool     _myHasFBOMultisample;

            asl::Unsigned64 _myTextureNodeVersion;
            unsigned _myBlitFilter;

            // OpenGL id of frame buffer object
            GLuint _myFrameBufferObject[2];

            // OpenGL id(s) of color buffer
            GLuint _myColorBuffer[2];

            // OpenGL id(s) of depth buffer 
            GLuint _myDepthBuffer[2];
    };
}

#endif
