// ----------------------------------------------------------------------------
//
// Copyright (C) 2002-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
// ----------------------------------------------------------------------------
//
// Name: viewCompressedTex
//
// Purpose:
//
// $RCSfile: RenderApp.h,v $
// $Revision: 1.2 $
// $Date: 2002/09/06 18:17:28 $
// $Author: valentin $
//
// ----------------------------------------------------------------------------


#ifndef AC_Y60_RENDER_APP_INCLUDED
#define AC_Y60_RENDER_APP_INCLUDED

#include <asl/Arguments.h>
#include <asl/Time.h>
#include <dom/Nodes.h>

//#include <video/SceneSyncMaster.h>
//#include <video/SceneSyncSlave.h>
//#include <video/EdgeBlender.h>
//#include <video/ColorCorrection.h>


#include <string.h>

namespace video {


class IRenderAppModule;


class RenderApp {
public:
    RenderApp();
    virtual ~RenderApp();

    static RenderApp & instance();

    virtual bool    preInit();
    virtual bool    init();
    virtual void    initWindow();
    virtual void    shutdown();

    virtual bool    handleArguments(int argc, char *argv[]);
    virtual bool    startMainLoop();
    bool            setRealtimePriority();
    
    bool            registerModule(IRenderAppModule & theModule);
    bool            activateModule(IRenderAppModule * theModule);
    bool            activateModule(const std::string & theModuleName);


    virtual void    handleDisplay();
    virtual void    handleReshape(int w, int h);
    virtual void    handleKeyboard(unsigned char key, int x, int y);
    virtual void    handleMouse(int button, int state, int x, int y);
    virtual void    handleMotion(int x, int y);
    virtual void    handlePeriodic(); 
    virtual void    handleVisible(bool isVisible);
    virtual void    handleSpecialKeys(int key, int x, int y);
    virtual void    handleSceneCommand(int theSceneCommand);

    bool    getVerbose() const 
    { return _verbose; }
    
    bool    getStatistic() const 
    { return _statistic; }
    
    double  getFps() const 
    { return _fps; }
    
    bool    getShowCursor() const
    { return _showCursor; }
    
    bool    getFullscreen() const
    { return _fullscreen; }
     
    double  getStartTime() const
    { return _startTime; }

    /*
    SceneSyncMaster::SceneSyncPacket &  getCurrentSyncPacket()
    { return _currentSyncPacket; }
    
    SceneSyncMaster::SceneSyncPacket &  getNewSyncPacket()
    { return _newSyncPacket; }
    
    SceneSyncSlave *                    getSyncSlave() {
        return _syncSlave;
    }
    */
    bool                                isRemotelyControlled() const {
        return false;
        //return (_syncSlave != 0);
    }


    int getScreenWidth() const {
        return _screenWidth;
    }
    
    int getScreenHeight() const {
        return _screenHeight;
    }
   
    void    setDesiredWindowSize(int theWidth, int theHeight) {
        _desiredWindowWidth = theWidth;
        _desiredWindowHeight = theHeight;
    }

private:
    typedef std::map<std::string, IRenderAppModule *>  RenderModuleMap;
    
    static RenderApp *_instance;
    
    //EdgeBlender         _edgeBlender;
    //ColorCorrection33   _colorCorrection;

    RenderModuleMap     _renderModules;
    IRenderAppModule *  _currentRenderModule;
    
    asl::Arguments::AllowedOption * _defaultOptions;
    asl::Arguments                  _arguments;

    int                 _originalWindow;
    bool                _fullscreen;
    bool                _verbose;
    bool                _statistic;
    int                 _blendRange[4];
    float               _borderGamma[4];
    //EdgeBlender::BORDER _changeBlendRange;
    bool                _keepEvents;
    bool                _windowInitialized;
    std::string              _startModule;
    bool                _colorCorrectionActive;
    int                 _selectedVertex;
    int                 _correctionMode;
    float               _redValue, _greenValue, 
                        _blueValue, _alphaValue;
    std::string              _colorCorrectionFileName;
                            
    //SceneSyncMaster::SceneSyncPacket    _currentSyncPacket;
    //SceneSyncMaster::SceneSyncPacket    _newSyncPacket;
    //SceneSyncSlave *                    _syncSlave;
    double                              _fps;
    asl::Time                           _startTime;
    bool                                _showCursor;
    int                                 _desiredWindowWidth;
    int                                 _desiredWindowHeight;
    int                                 _screenWidth;
    int                                 _screenHeight;
    
    void    setupBlending();
    void    setupColorCorrection();
    void    setupColorCorrection(const std::string & modeName, float r, float g, float b, float a);
    void    setupDisplayConfiguration(dom::Node & theConfig);
    void    writeDisplayConfiguration();

    void    printUsage();

    //void    setGamma(EdgeBlender::BORDER border, float gamma);

    void    registerGlutCallbacks();

    // static GLUT callbacks
    static void motion(int x, int y);
    static void mouse(int button, int state, int x, int y);
    static void display();
    static void reshape(int w, int h);
    static void periodic();
    static void key(unsigned char key, int x, int y);
    static void visible(int vis);
    static void specialKeys(int key, int x, int y);
};




};



#endif // AC_Y60_RENDER_APP_INCLUDED
