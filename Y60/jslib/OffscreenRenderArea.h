//============================================================================
// Copyright (C) 2005-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_jslib_OffscreenRenderArea_h_
#define _ac_jslib_OffscreenRenderArea_h_

#include "AbstractRenderWindow.h"
#include <asl/Logger.h>
#include <dom/Nodes.h>
#include <dom/Value.h>
#include <y60/OffscreenBuffer.h>

/**
 * @ingroup Y60jslib
 */ 
namespace jslib {

    class y60::Texture;
    typedef asl::Ptr<y60::Texture, dom::ThreadingModel> TexturePtr;
    
    class OffscreenRenderArea : public y60::OffscreenBuffer, public AbstractRenderWindow {
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
            void renderToCanvas(bool theCopyToImageFlag = false, unsigned theCubemapFace = 0); 

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
            void activate(unsigned theCubemapFace = 0); 
            void deactivate(bool theCopyToImageFlag = false);

        protected:
            // AbstractRenderWindow
            virtual y60::TTFTextRendererPtr createTTFRenderer() {
                return y60::TTFTextRendererPtr(0);
            }

        private:
            dom::ResizeableRasterPtr ensureRaster(TexturePtr theTexture);

            const TexturePtr getTexture() const;
            TexturePtr getTexture();

            unsigned _myWidth;
            unsigned _myHeight;
    };
}

#endif // _ac_jslib_OffscreenRenderArea_h_
