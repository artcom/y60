//============================================================================
//
// Copyright (C) 2002-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef AC_Y60_RENDER_APP_MODULE_INCLUDED
#define AC_Y60_RENDER_APP_MODULE_INCLUDED

#include <asl/Arguments.h>
#include <asl/Time.h>

#include <string.h>



// XXX dummy ... to be removed
namespace SceneSyncMaster {
    struct SceneSyncPacket { };
}


namespace video {


class RenderApp;

class IRenderAppModule {
public:

    virtual const std::string getName() = 0;

    virtual bool    shutdown(bool isFinalShutdown) = 0;

    virtual void    addAllowedOptions(asl::Arguments & theArguments) = 0;
    virtual bool    init(asl::Arguments & theArguments) = 0;

    virtual void    renderPreSync() = 0;
    virtual void    renderPostSync() = 0;
    virtual void    renderPostEdgeBlend() = 0;
    virtual void    handleReshape(int w, int h) = 0;
    virtual bool    handleKeyboard(unsigned char key, int x, int y) = 0;
    virtual bool    handleSpecialKeys(int key, int x, int y) = 0;
    virtual void    handleMouse(int button, int state, int x, int y) = 0;
    virtual void    handleMotion(int x, int y) = 0;
    virtual void    handlePeriodic(double theRunningTime, 
            SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, bool & theNeedsRedraw) = 0;
    virtual void    handleVisible(bool isVisible) = 0;
    
    virtual void    handleResetCommand() = 0;
    virtual void    handleQuitCommand() = 0;

    virtual void    initWindow() = 0;
    virtual void    updateWindow() = 0;
    virtual void    activateWindow() = 0;
    virtual void    deactivateWindow() = 0;
    
    //  called when activated
    virtual bool    activate() = 0;
    //  called when deactivated
    virtual bool    deactivate() = 0;
    virtual bool    isActive() = 0;
};



class RenderAppModule : public IRenderAppModule {
public:

    RenderAppModule(RenderApp & theRenderApp, const std::string & theName) : _isActive(false), 
        _renderApp(theRenderApp), _name(theName)
    {}

    virtual ~RenderAppModule()
    {}

    virtual const std::string getName() 
    { return _name; }

    virtual bool    shutdown(bool isFinalShutdown)
    { return true;} 

    virtual void    addAllowedOptions(asl::Arguments & theArguments)
    {}
    virtual bool    init(asl::Arguments & theArguments)
    { return true; }

    virtual void    renderPreSync()
    {}
    virtual void    renderPostSync()
    {}
    virtual void    renderPostEdgeBlend()
    {}
    virtual void    handleReshape(int w, int h)
    {}
    virtual bool    handleKeyboard(unsigned char key, int x, int y)
    { return false; } // not handled here
    virtual bool    handleSpecialKeys(int key, int x, int y) 
    { return false; } // not handled here
    virtual void    handleMouse(int button, int state, int x, int y)
    {}
    virtual void    handleMotion(int x, int y)
    {}
    virtual void    handlePeriodic(double theRunningTime, 
            SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, bool & theNeedsRedraw)
    {}
    virtual void    handleVisible(bool isVisible)
    {}
    
    virtual void    handleResetCommand()
    {}
    virtual void    handleQuitCommand()
    {}

    
    // called during startup
    virtual void    initWindow()
    {}
    virtual void    updateWindow()
    {}
    virtual void    activateWindow()
    {}
    virtual void    deactivateWindow()
    {}

    //  called when activated
    virtual bool    activate() 
    { return true; }
    //  called when deactivated
    virtual bool    deactivate() 
    { return true; }
    virtual bool    isActive() 
    { return _isActive; }

protected:
    RenderApp & _renderApp;
    std::string      _name;
    bool        _isActive;

};

class BlankAppModule : public RenderAppModule {
public:

    BlankAppModule(RenderApp & theRenderApp);
    
    virtual void    renderPostSync();
    
    virtual void    handlePeriodic(double theRunningTime, 
            SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, bool & theNeedsRedraw);
private:
    unsigned int _myColorABGR;
};


}; // namespace video


#endif // AC_Y60_RENDER_APP_MODULE_INCLUDED
