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

#ifndef _ac_jslib_OffscreenRenderArea_h_
#define _ac_jslib_OffscreenRenderArea_h_

#include "y60_jslib_settings.h"

#include "AbstractRenderWindow.h"
#include <asl/base/Logger.h>
#include <asl/dom/Nodes.h>
#include <asl/dom/Value.h>
#include <y60/glutil/OffscreenBuffer.h>


namespace y60 {
    class Texture;
}

/**
 * @ingroup Y60jslib
 */
namespace jslib {

    typedef asl::Ptr<y60::Texture, dom::ThreadingModel> TexturePtr;

    class OffscreenRenderArea : public AbstractRenderWindow {
        public:
            static asl::Ptr<OffscreenRenderArea> create();

            /**
             * Allows Offscreen rendering into a texture.
             * Renders by default into the backbuffer
             * Renders into an offscreen framebuffer using the
             * EXT_framebuffer_object GL extension if
             * my rendercaps include y60::FRAMEBUFFER_SUPPORT
             */
            OffscreenRenderArea();
            virtual ~OffscreenRenderArea();

            /**
             * renders my current scene onto the texture
             * given by the target attribute of my canvas.
             * @param theCopyToImageFlag if true
             * the underlying raster value is ignored
             * else the texture is copied into the raster.
             */
            void renderToCanvas(bool theCopyToImageFlag = false, unsigned theCubemapFace = 0, bool theClearColorBufferFlag = true, bool theClearDepthBufferFlag = true);

            /**
             * sets my canvas (calling base function)
             * the canvas SHOULD have a target attribute pointing to an existing image.
             * ensures a valid raster for that image.
             */
            bool setCanvas(const dom::NodePtr & theCanvas);

            /** Sets the width of the viewport in pixels. All other
             * implementations of AbstractRenderWindow are resized by
             * the window system. For OffscreenRenderArea the client is
             * responseable to set these values.
             */
            void setWidth(unsigned theWidth);
            /** Sets the height of the viewport in pixels.
             * @see setWidth() */
            void setHeight(unsigned theHeight);

            /** Downloads only those parts of the framebuffer that have been
             * set by setWidth() and setHeight(). The texture used as a render
             * traget has the usual power of two constrains. This function can
             * be used to retrive a partial rectangle of arbitrary size.
             */
            void downloadFromViewport(const dom::NodePtr & theTextureNode);

            // IFrameBuffer
            virtual int getWidth() const;
            virtual int getHeight() const;

            // IEventSink
            virtual void handle(y60::EventPtr theEvent);

            // IGLContextManager
            virtual void activateGLContext() {}
            virtual void deactivateGLContext() {}

            // AbstractRenderWindow
            virtual void initDisplay();
            virtual void setRenderingCaps(unsigned int theRenderingCaps);

            //TODO make some stuff from AbstractRenderWindow private

            // activates the offscreen buffer as render target
            // simple api methods using overriden private methods
            void activate(unsigned theCubemapFace = 0);
            void deactivate(bool theCopyToImageFlag = false);

        protected:
            // AbstractRenderWindow
            virtual y60::TTFTextRendererPtr createTTFRenderer() {
                return y60::TTFTextRendererPtr();
            }

        private:
            void activate(std::vector<TexturePtr> & theTextures, unsigned theCubemapFace = 0);
            void deactivate(const std::vector<TexturePtr> & theTextures, bool theCopyToImageFlag = false);
            const std::vector<TexturePtr> getRenderTargets() const;
            void ensureRenderTargets(std::vector<TexturePtr> & theTextures);
            y60::OffscreenBufferPtr _myOffscreenBuffer;
            unsigned _myWidth;
            unsigned _myHeight;
    };
}

#endif // _ac_jslib_OffscreenRenderArea_h_
