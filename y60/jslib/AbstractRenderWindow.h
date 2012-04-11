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

#ifndef _y60_abstractrenderwindow_included_
#define _y60_abstractrenderwindow_included_

#include "y60_jslib_settings.h"

#include "IRendererExtension.h"
#include "Timeout.h"
#include "JSRequestWrapper.h"
#include "JSRequest.h"

#include <y60/scene/Canvas.h>
#include <y60/scene/IFrameBuffer.h>
#include <y60/glrender/Renderer.h>
#include <y60/gltext/TTFTextRenderer.h>
#include <y60/glutil/IGLContextManager.h>
#include <y60/inet/RequestManager.h>
#include <y60/glutil/GLContext.h>
#include <y60/scene/Scene.h>
#include <y60/input/IEventSink.h>
#include <y60/input/IEventSource.h>

#include <list>

namespace y60 {
    class BufferToFile;
}
namespace jslib {

    DEFINE_EXCEPTION(RenderWindowException, asl::Exception);

    /**
     * @ingroup y60jslib
     * Base class for RenderWindows. RenderWindows are used to render
     * scenes.
     */
    class Y60_JSLIB_DECL AbstractRenderWindow : public y60::IEventSink,
                                 public y60::IFrameBuffer,
                                 public y60::IGLContextManager {
    public:
        ~AbstractRenderWindow();

        /**
         * Sets the weak self pointer of the RenderWindow. Each RenderWindow
         * keeps a asl::WeakPtr to itself. An asl::WeakPtr is also registered
         * with the ContextRegistry.
         * @param theSelfPtr pointer to the renderwindow itself.
         */
        void setSelf(asl::Ptr<AbstractRenderWindow> theSelfPtr);

        virtual void initDisplay() = 0;

        /**
         * Sets a previously loaded scene to be rendered.
         * @param theScene Scene to be rendered.
         * @param theCanvas the canvas to to render to. If 0, use the first canvas.
         * @return true if the scene was successfully set and is rendered in the
         *         window.
         * @warn Currently, theScene may not be Ptr(0)!
         */
        bool setSceneAndCanvas(const y60::ScenePtr & theScene,
                const dom::NodePtr & theCanvas = dom::NodePtr());

        /**
         * Sets a previously loaded scene to be rendered.
         * @param theScene Scene to be rendered.
         * @return true if the scene was successfully set and is rendered in the
         *         window.
         * @warn Currently, theScene may not be Ptr(0)!
         * @deprecated use setSceneAndCanvas
         */
        bool setScene(const y60::ScenePtr & theScene);

        /**
         * Creates the renderer and opens the renderwindow, if this has not been
         * done by a setSceneAndCanvas() call, before.
         */
        virtual bool go();

        virtual void setVisibility(bool theFlag);
        bool getVisibility() const;

        JSObject * getEventListener() const;
        void setEventListener(JSObject * theListener);

        void setPause(bool aPause);
        bool getPause() const;

        void setForceFullGC(bool theForceFullGC);
        bool getForceFullGC() const;

        long setTimeout(const std::string & myCommand, float myMilliseconds, JSObject * theObjectToCall);
        void clearTimeout(long myTimeoutId);
        long setInterval(const std::string & myCommand, float myMilliseconds, JSObject * theObjectToCall);
        void clearInterval(long myIntervalId);

        bool hasCap(unsigned int theCap);
        bool hasCapAsString(const std::string & theCapStr);
        virtual void setRenderingCaps(unsigned int theRenderingCaps);
        unsigned int getRenderingCaps();

        void setMultisamples(unsigned theSamples) {
            _myMultisamples = theSamples;
        }
        unsigned getMultisamples() const {
            return _myMultisamples;
        }

        std::string getGLVersionString();
        std::string getGLVendorString();
        std::string getGLRendererString();
        unsigned int getGLExtensionStrings(std::vector<std::string> & theTokens);

        void printStatistics();
        double getFrameRate() const;

        // Scene methods
        y60::ImagePtr getImage(const std::string & theFileName);

        // TODO: adapt for other 1 and 3 byte pixel formats
        asl::Vector4i getImagePixel(dom::NodePtr theImageNode,
            unsigned long theX, unsigned long theY);
        bool setImagePixel(dom::NodePtr theImageNode, unsigned long theX,
            unsigned long theY, const asl::Vector4i & theColor);

        // Text Manager Methods
        void renderText(const asl::Vector2f & thePixelPosition, const std::string & theString,
                const std::string & theFont, const y60::ViewportPtr & theViewport);
        void renderText(const asl::Vector2f & thePixelPosition, const std::string & theString, const std::string & theFont) {
                return renderText(thePixelPosition, theString, getDefaultFont(), getSingleViewport());
                }
        void renderText(const asl::Vector2f & thePixelPosition, const std::string & theString) {
            return renderText(thePixelPosition, theString, getDefaultFont(), getSingleViewport());
        }

        void setTextColor(const asl::Vector4f & theTextColor);
        const asl::Vector4f & getTextColor();

        asl::Vector2i renderTextAsImage(dom::NodePtr theImageNode,
                const std::string & theString, const std::string & theFont,
                const unsigned int & theTargetWidth=0, const unsigned int & theTargetHeight=0,
                const asl::Vector2i & theCursorPos=asl::Vector2i(0,0));
        void setTextStyle(unsigned int theStyle);
    
        const std::vector<asl::Vector2i> & getTextGlyphPositions() const;
        const asl::Vector2i & getTextCursorPosition() const;
        const std::vector<unsigned int> & getLineWidths() const;
        const unsigned int & getMaxWidth() const;
        void setTextPadding(int topPadding, int bottomPadding, int leftPadding, int rightPadding);
        void setTextIndentation(int theIndentation);

        void setHTextAlignment(unsigned int theHAlignment);
        void setVTextAlignment(unsigned int theVAlignment);
        void setLineHeight(unsigned int theLineHeight);
        void setParagraph(unsigned int theTopMargin, unsigned int theBottomMargin);

        bool getFontMetrics(const std::string & theFontName,
                int & theFontHeight,
                int & theFontAscent, int & theFontDescent,
                int & theFontLineSkip);

        bool getGlyphMetrics(const std::string & theFontName,
                             const std::string & theCharacter,
                             asl::Box2f & theGlyphBox, double & theAdvance);

        double getKerning(const std::string & theFontName,
                          const std::string & theFirstCharacter,
                          const std::string & theSecondCharacter);

        bool hasGlyph(const std::string & theFontName,
                       const std::string & theCharacter);

        void setTracking(float theTracking);

        // Animation Manager
        void runAnimations(float theTime);
        void playClip(float theTime,
                const std::string & theCharacterName,
                const std::string & theClipName);
        void setClipLoops(const std::string & theCharacterName,
                const std::string & theClipName,
                unsigned int theLoops);
        void setClipForwardDirection(const std::string & theCharacterName,
                const std::string & theClipName,
                bool theDirection);
        unsigned int getLoops(const std::string & theCharacterName, const std::string & theClipName);

        bool isClipActive(const std::string & theCharacterName, const std::string & theClipName);
        bool isCharacterActive(const std::string & theCharacterName);

        void stopCharacter(const std::string & theCharacterName);

        // HTTP
        void performRequest(const jslib::JSRequestPtr & theRequest);

        const y60::RendererPtr getRenderer() const;
        y60::RendererPtr getRenderer();

        const y60::ScenePtr & getCurrentScene() const;
        y60::ScenePtr & getCurrentScene();

        asl::Ptr<y60::TextureManager> getTextureManager() {
            return _myScene->getTextureManager();
        }

        void addExtension(y60::IRendererExtensionPtr theExtension);

        /// save framebuffer to file
        void saveBuffer(const std::string & theFilename, const unsigned int theComponents = 4);

        /// copy framebuffer to Texture (and optionally to Image raster).
        void copyBufferToTexture(dom::NodePtr & theTexture, const asl::Vector2i & theOffset, bool theCopyToRasterFlag = false);

        void setJSContext(JSContext * cx);

        /**
         * returns the viewport when in single viewport mode.
         * @throws an exception if more than one viewport exists in the canvas.
        */
        y60::ViewportPtr getSingleViewport() const;

        dom::NodePtr getCanvas();
        dom::NodePtr getCanvas() const;
        virtual bool setCanvas(const dom::NodePtr & theCanvas);

        /**
         * Set the current canvas to null so there is no connection from scene to window.
         */
        void unsetCanvas();

        void setFixedDeltaT(const double & theDeltaT);
        const double & getFixedDeltaT() const;

        inline std::string getOrientation() {
            return y60::LANDSCAPE_ORIENTATION;
        }

        static inline std::string getDefaultFont() {
            static std::string myDefaultFont = "Screen15";
            return myDefaultFont;
        }

        void getPixel(unsigned int theXPos, unsigned int theYPos, GLenum theFormat, asl::Unsigned8 & theValue);
        void getPixel(unsigned int theXPos, unsigned int theYPos, GLenum theFormat, float & theValue);

        y60::GLContextPtr getGLContext() const {
            return _myGLContext;
        }

        virtual void clearBuffers(unsigned int theBufferMask);
        virtual void preRender();
        virtual void render();
        virtual void postRender();
        virtual void swapBuffers() {}

        /**
         * Standard render function.
         * - updates all modified
         * - checks if 'onRender' is implemented and calls that
         * - else does a regular render with clear/pre/render/post/swap
         */
        void renderFrame();

    protected:
        AbstractRenderWindow(const JSErrorReporter & theErrorReporter);
        virtual void preViewport(const dom::NodePtr & theViewport);
        virtual void postViewport(const dom::NodePtr & theViewport);

        virtual void handle(y60::EventPtr theEvent);
        virtual void onFrame();
        virtual void onKey(y60::Event & theEvent);
        virtual void onMouseButton(y60::Event & theEvent);
        virtual void onMouseWheel(y60::Event & theEvent);
        virtual void onMouseMotion(y60::Event & theEvent);
        virtual void onAxis(y60::Event & theEvent);
        virtual void onTouch(y60::Event & theEvent);
        virtual void onResize(y60::Event & theEvent);
        virtual void onButton(y60::Event & theEvent);
        virtual void onGenericEvent(y60::Event & theEvent); // generic XML based event handler

        virtual y60::TTFTextRendererPtr createTTFRenderer() = 0;

        void setGLContext(y60::GLContextPtr theContext) {
            _myGLContext = theContext;
        }
   protected:
        y60::ScenePtr    _myScene;
        dom::NodePtr     _myCanvas;
        y60::RendererPtr _myRenderer;

        typedef std::list< asl::WeakPtr<y60::IRendererExtension> > ExtensionList;
        ExtensionList        _myExtensions;

        JSObject *           _myEventListener;
        JSContext *          _myJSContext;

        JSErrorReporter      _myErrorReporter;
        inet::RequestManager _myRequestManager;

        unsigned int         _myRenderingCaps;
        unsigned int         _myMultisamples;

        double               _myElapsedTime;
        bool                 _myVisiblityFlag;

        asl::WeakPtr<AbstractRenderWindow> _mySelf;


    private:
        AbstractRenderWindow();
        void ensureScene();

        y60::GLContextPtr _myGLContext;
        TimeoutQueue      _myTimeoutQueue;
        double            _myFixedDeltaT;
        double            _myStartTime;
        double            _myPauseTime;
        bool              _myPauseFlag;
        bool              _myForceFullGC;
        asl::Ptr<y60::BufferToFile> _myBufferToFileWriter;
    };

}

#endif
