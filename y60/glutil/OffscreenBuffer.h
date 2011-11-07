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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

    class Y60_GLUTIL_DECL OffscreenBuffer {
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
            void activate(std::vector<TexturePtr> & theTextures,
                          unsigned theSamples = 1, unsigned theCubemapFace = 0);

            /**
            * deactivates the texture as render target
            * @param theCopyToImageFlag copy result to texture raster.
            */
            void deactivate(std::vector<TexturePtr> & theTextures,
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

            void copyToImage(std::vector<TexturePtr> & theTextures);
            void copyToImage(TexturePtr theTexture);

        private:
            void reset();

            void bindOffscreenFrameBuffer(std::vector<TexturePtr> & theTextures,
                                          unsigned theSamples = 0, unsigned theCubemapFace = 0);
            void attachCubemapFace(unsigned theCubemapFace);

            bool     _myUseFBO;
            bool     _myHasFBOMultisample;

            asl::Unsigned64 _myTextureNodeVersion;
            unsigned int  _myTextureWidth;
            unsigned int  _myTextureHeight;
            
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
