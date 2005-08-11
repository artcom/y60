//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _y60_abstractrenderwindow_included_
#define _y60_abstractrenderwindow_included_

#include "JSWrapper.h"
#include "IRendererExtension.h"
#include "Timeout.h"
#include "JSRequestWrapper.h"
#include "JSRequest.h"

#include <y60/Canvas.h>
#include <y60/IFrameBuffer.h>
#include <y60/Renderer.h>
#include <y60/TTFTextRenderer.h>
#include <y60/IGLContextManager.h>
#include <y60/RequestManager.h>

#include <y60/Scene.h>
#include <y60/IEventSink.h>
#include <y60/IEventSource.h>

#include <list>

namespace y60 {
    class GLContext;
}

namespace jslib {

    DEFINE_EXCEPTION(RenderWindowException, asl::Exception);

    /**
     * @ingroup y60jslib
     * Base class for RenderWindows. RenderWindows are used to render
     * scenes. 
     */
    class AbstractRenderWindow : public y60::IEventSink,
                                 public y60::IFrameBuffer,
                                 public y60::IGLContextManager {
    public:
        enum StatisticType {
            TEXT_STATISTIC     = 1,
            RENDERED_STATISTIC
        };

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
         * @return true if the scene was successfully set and is rendered in the
         *         window.
         * @warn Currently, theScene may not be Ptr(0)!
         */
        bool AbstractRenderWindow::setScene(const y60::ScenePtr & theScene);
        float getWorldSize(dom::NodePtr theCamera);

        JSObject * getEventListener() const;
        void setEventListener(JSObject * theListener);

        void setPause(bool aPause);
        bool getPause() const;

        long setObjectTimeout(JSObject *theObject, const std::string & myCommand,
                              float myMilliseconds);
        long setObjectInterval(JSObject *theObject, const std::string & myCommand,
                               float myMilliseconds);
        long setTimeout(const std::string & myCommand, float myMilliseconds);
        void clearTimeout(long myTimeoutId);
        long setInterval(const std::string & myCommand, float myMilliseconds);
        void clearInterval(long myIntervalId);

        bool hasCap(unsigned int theCap);
        bool hasCapAsString(const std::string & theCapStr);
        virtual void setRenderingCaps(unsigned int theRenderingCaps);
        unsigned int getRenderingCaps();
        bool printStatistics(unsigned int theOnScreenFlag, unsigned int theStatisticFlags);
        double getFrameRate() const;
        // Scene methods
        //asl::Matrix4f getLocalMatrix(dom::NodePtr theNode);
        y60::ImagePtr getImage(const std::string & theFileName);
        void loadMovieFrame(dom::NodePtr theMovieNode);
        void loadCaptureFrame(dom::NodePtr theCaptureNode);
        // TODO: adapt for other 1 and 3 byte pixel formats
        asl::Vector4i getImagePixel(dom::NodePtr theImageNode,
            unsigned long theX, unsigned long theY);
        bool setImagePixel(dom::NodePtr theImageNode, unsigned long theX,
            unsigned long theY, const asl::Vector4i & theColor);
        // Text Manager Methods
        void renderText(const asl::Vector2f & thePos, const std::string & theString,
                const std::string & theFont);
        void setTextColor(const asl::Vector4f & theTextColor, const asl::Vector4f & theBackColor);
        asl::Vector2i renderTextAsImage(dom::NodePtr theImageNode,
                const std::string & theString, const std::string & theFont,
                const unsigned int & theTargetWidth, const unsigned int & theTargetHeight);
        void setTextStyle(unsigned int theStyle);

        void setTextPadding(int topPadding, int bottomPadding, int leftPadding, int rightPadding);

        void setHTextAlignment(unsigned int theHAlignment);
        void setVTextAlignment(unsigned int theVAlignment);
        void setLineHeight(unsigned int theLineHeight);
        void setParagraph(unsigned int theTopMargin, unsigned int theBottomMargin);

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
        const y60::ScenePtr & getCurrentScene() const { return _myScene; };
        y60::ScenePtr & getCurrentScene() { return _myScene; };

        asl::Ptr<y60::TextureManager> getTextureManager() {
            return _myScene->getTextureManager();
        }

        void addExtension(y60::IRendererExtensionPtr theExtension);
        /// saves (frame-)buffer to file
        void saveBuffer(const std::string & theBuffer, const std::string & theFilename);

        void setJSContext(JSContext * cx);
        /// returns the viewport when in single viewport mode.

        /**
         * @throws an exception if more than one viewport exists in the canvas.
        */
        y60::ViewportPtr getSingleViewport() const;

        dom::NodePtr getCanvas() const { return _myCanvas; }
        virtual bool setCanvas(const dom::NodePtr & theCanvas);
        /**
         * Set the current canvas to null so there is no connection from scene to window.
         */
        void unsetCanvas();

        void setFixedDeltaT(const float & theDeltaT);
        const float & getFixedDeltaT() const;

        inline std::string getOrientation() {
            return y60::LANDSCAPE_ORIENTATION; 
        } 

    protected:
        AbstractRenderWindow(const JSErrorReporter & theErrorReporter);
        virtual void preRender();
        virtual void preViewport(const dom::NodePtr & theViewport);
        virtual void render();
        virtual void postViewport(const dom::NodePtr & theViewport);
        virtual void postRender();
        virtual void handle(y60::EventPtr theEvent);
        virtual void onIdle();
        virtual void onKey(y60::Event & theEvent);
        virtual void onMouseButton(y60::Event & theEvent);
        virtual void onMouseWheel(y60::Event & theEvent);
        virtual void onMouseMotion(y60::Event & theEvent);
        virtual void onAxis(y60::Event & theEvent);
        virtual void onTouch(y60::Event & theEvent);
        virtual void onResize(y60::Event & theEvent);
        virtual void onButton(y60::Event & theEvent);

        virtual y60::TTFTextRendererPtr createTTFRenderer() = 0;

        y60::ScenePtr _myScene;
        dom::NodePtr _myCanvas;
        y60::RendererPtr _myRenderer;
        //asl::Ptr<y60::TextureManager> _myTextureManager;

        JSObject *       _myEventListener;
        JSContext *      _myJSContext;
        y60::GLContext * _myContext;

        unsigned int _myRenderingCaps;

        typedef std::list<y60::IRendererExtensionPtr> ExtensionList;
        ExtensionList _myExtensions;
        JSErrorReporter _myErrorReporter;
        inet::RequestManager   _myRequestManager;
    private:
        AbstractRenderWindow();
        asl::WeakPtr<AbstractRenderWindow> _mySelf;
        TimeoutQueue           _myTimeoutQueue;
        float                  _myFixedDeltaT;
        double                 _myPauseTime;
        bool                   _myPauseFlag;
        double                 _myElapsedTime;
    };
}

#endif
