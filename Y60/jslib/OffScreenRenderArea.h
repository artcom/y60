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

#ifndef _ac_jslib_OffScreenRenderArea_h_
#define _ac_jslib_OffScreenRenderArea_h_

#include "AbstractRenderWindow.h"
#include <asl/Logger.h>
#include <dom/Nodes.h>
#include <dom/Value.h>

/**
 * 
 * @ingroup Y60jslib
 */ 
namespace jslib {
    class y60::Image;
    
class OffScreenRenderArea : public AbstractRenderWindow {
    public:
        static asl::Ptr<OffScreenRenderArea> create();
        
        /**
         * Allows Offscreen rendering into a texture.
         * Renders by default into the frame buffers back buffer
         * Renders into an off screen framebuffer using the 
         * EXT_framebuffer_object GL extension if 
         * my rendercaps include y60::FRAMEBUFFER_SUPPORT
         */
        OffScreenRenderArea();
        virtual ~OffScreenRenderArea();

        /**
         * renders my current scene onto the texture 
         * given by the target attribute of my canvas. 
         * @param theCopyToImageFlag if true 
         * the underlying raster value is ignored
         * else the texture is copied into the raster.
         */
        void renderToTexture(bool theCopyToImageFlag = false); 


        /**
         * sets my canvas (calling base function)
         * the canvas SHOULD have a target attribute pointing to an existing image.
         * ensures a valid raster for that image.
         */
        bool setCanvas(const dom::NodePtr & theCanvas);
        
        // IFrameBuffer
        virtual int getWidth() const;
        virtual int getHeight() const;

        // IEventSink
        virtual void handle(y60::EventPtr theEvent);

        // IGLContext
        virtual void activateGLContext();
        virtual void deactivateGLContext(); 

        // AbstractRenderWindow
        virtual void initDisplay();

        //TODO make some stuff from AbstractRenderWindow private

    protected:
        // AbstractRenderWindow
        virtual y60::TTFTextRendererPtr createTTFRenderer() {
            return y60::TTFTextRendererPtr(0);
        }
        

    private:
        void copyTextureToImage(asl::Ptr<y60::Image, dom::ThreadingModel> theImage);
        dom::ResizeableRasterPtr ensureRaster(asl::Ptr<y60::Image, dom::ThreadingModel> theImage);
        const asl::Ptr<y60::Image, dom::ThreadingModel> getImage() const;
        asl::Ptr<y60::Image, dom::ThreadingModel> getImage();
        void copyFrameBufferToTexture(asl::Ptr<y60::Image, dom::ThreadingModel> theImage);
        void bindTexture(asl::Ptr<y60::Image, dom::ThreadingModel> theTexture);
        void bindOffScreenFrameBuffer(asl::Ptr<y60::Image, dom::ThreadingModel> theTexture);
        
        unsigned _myOffScreenBuffer ,_myDepthBuffer;
};

}

#endif // _ac_jslib_OffScreenRenderArea_h_
