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

#define DEBUG_LEVEL 1
#define DEBUG_TIME 

#include "RenderApp.h"
#include "RenderAppModule.h"

#include <asl/numeric_functions.h>
#include <asl/file_functions.h>

#define  GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>  
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h> 


using namespace std;  // automatically added!


// glut internal definition
extern int __glutScreenHeight;
extern int __glutScreenWidth;


namespace video {

// ----------------------------------------------------------------------------

const int   DefaultWindowHeight = 96;
const int   DefaultWindowWidth  = 128;

const char* BorderName[] = { "top", "bottom", "left", "right"};

RenderApp * RenderApp::_instance = 0;

asl::Arguments::AllowedOption gRenderAppAllowedOptions[] = {
    {"--start-module",   "%s"},
    {"--fps",            "%d"},
    {"--remote-control", ""  },
    {"--port",           "%d"},
    {"--verbose",        ""  },
    {"--statistic",      ""  },
    {"--help",           ""  },
    {"--window",         ""  },
    {"--fullscreen",     ""  },
    {"--blend-top",      "%d"},
    {"--blend-bottom",   "%d"},
    {"--blend-left",     "%d"},
    {"--blend-right",    "%d"},
    {"--blend-to",       "%s"},
    {"--gamma-top",      "%f"},
    {"--gamma-bottom",   "%f"},
    {"--gamma-left",     "%f"},
    {"--gamma-right",    "%f"},
    {"--correct-red",    "%f"},
    {"--correct-green",  "%f"},
    {"--correct-blue",   "%f"},
    {"--correct-alpha",  "%f"},
    {"--correct-mode",   "%s"},
    {"--write-color-correction", "%s"},    
    {"--realtime",          ""},
    {"--pos-x",             "%d"},
    {"--pos-y",             "%d"},
    {"--color-correction",  "%s"},
    {"--display-config",    "%s"},
    {"",                    ""  }
};

RenderApp::RenderApp() :
    _currentRenderModule(0), 
    _defaultOptions(0), 
    _fps(30),
    _fullscreen(false), _verbose(false), 
    _originalWindow(-1), _statistic(false),
    _showCursor(true), _keepEvents(false),
    _screenWidth(0), _screenHeight(0),
    _desiredWindowWidth(DefaultWindowWidth), 
    _desiredWindowHeight(DefaultWindowHeight),
    _windowInitialized(false),
    _colorCorrectionActive(false),
    _selectedVertex(0),
    _redValue(1.f), _greenValue(1.f), 
    _blueValue(1.f), _alphaValue(0.f)
{
    AC_DEBUG << "RenderApp::RenderApp()";

    assert(_instance == 0);
    _instance = this;
    
    for (int i=0; i<4; i++) {
        _borderGamma[i] = 1.0;
    }
}

RenderApp::~RenderApp() {
    AC_DEBUG << "RenderApp::~RenderApp()";

    shutdown();

    _currentRenderModule = 0;
    RenderModuleMap::iterator iter(_renderModules.begin());
    while (iter != _renderModules.end()) {
        IRenderAppModule * module = iter->second;
        assert(module);
        module->shutdown(true);
        _renderModules.erase(iter);
        iter = _renderModules.begin();
        delete module;
    }
    AC_DEBUG << "RenderApp::~RenderApp() finished";
}


RenderApp & RenderApp::instance() {
    assert(_instance);
    return *_instance;
}

void
RenderApp::shutdown() {
    AC_DEBUG << "RenderApp::shutdown()";

    RenderModuleMap::iterator iter(_renderModules.begin());
    while (iter != _renderModules.end()) {
        IRenderAppModule * module = iter->second;
        assert(module);
        module->shutdown(false);
        iter++;
    }
    AC_DEBUG << "RenderApp::shutdown() finished";
}


bool 
RenderApp::setRealtimePriority() {
    AC_DEBUG << "RenderApp::setRealtimePriority()";
    
    struct sched_param schp;
    /*
     * set the process to realtime privs
     */
    memset(&schp, 0, sizeof(schp));
    schp.sched_priority = sched_get_priority_max(SCHED_FIFO);

    if (sched_setscheduler(0, SCHED_FIFO, &schp) != 0) {
        perror("sched_setscheduler");
        return false;
    }

    return true;

}


void
RenderApp::setupColorCorrection() {
     if (_arguments.haveOption("--correct-red")) {
        _redValue = asl::as<float>(_arguments.getOptionArgument("--correct-red"));
    }

    if (_arguments.haveOption("--correct-green")) {
        _greenValue = asl::as<float>(_arguments.getOptionArgument("--correct-green"));
    }

    if (_arguments.haveOption("--correct-blue")) {
        _blueValue = asl::as<float>(_arguments.getOptionArgument("--correct-blue"));
    }

    if (_arguments.haveOption("--correct-alpha")) {
        _alphaValue = asl::as<float>(_arguments.getOptionArgument("--correct-alpha"));
    }
    if (_arguments.haveOption("--correct-mode")) {
        string correctMode = _arguments.getOptionArgument("--correct-mode");
        setupColorCorrection(correctMode, _redValue, _greenValue, _blueValue, _alphaValue);
    }
}

void
RenderApp::setupColorCorrection(const string & modeName, float r, float g, float b, float a) {
    AC_DEBUG <<"RenderApp::setupColorCorrection " << modeName 
        << " " << r
        << " " << g
        << " " << b
        << " " << a
        << endl;
    _colorCorrectionActive = true;
}
 
void 
RenderApp::printUsage()  
{
    AC_DEBUG <<"usage: renderslave moviefilename1 [moviefilename2]"<<endl;
    _arguments.printUsage();
}

void debugGLUT  () {
    AC_PRINT  << "debugGLUT ():" << endl
        << "\tcurrent window id: " << glutGetWindow ()
        << endl ;
    AC_PRINT  << "debugGLUT ():" << endl
        << "\tGLUT_GAME_MODE_ACTIVE : " << glutGameModeGet (GLUT_GAME_MODE_ACTIVE) 
        << endl
        << "\tGLUT_GAME_MODE_POSSIBLE : " << glutGameModeGet (GLUT_GAME_MODE_POSSIBLE) 
        << endl
        << "\tGLUT_GAME_MODE_WIDTH : " << glutGameModeGet (GLUT_GAME_MODE_WIDTH) 
        << endl
        << "\tGLUT_GAME_MODE_HEIGHT : " << glutGameModeGet (GLUT_GAME_MODE_HEIGHT) 
        << endl
        << "\tGLUT_GAME_MODE_PIXEL_DEPTH : " << glutGameModeGet (GLUT_GAME_MODE_PIXEL_DEPTH) 
        << endl
        << "\tGLUT_GAME_MODE_REFRESH_RATE : " << glutGameModeGet (GLUT_GAME_MODE_REFRESH_RATE)
        << endl
        << "\tGLUT_GAME_MODE_DISPLAY_CHANGED : " 
        << glutGameModeGet (GLUT_GAME_MODE_DISPLAY_CHANGED) 
        << endl ;
}

void
RenderApp::handleMotion(int x, int y) {
    if (_currentRenderModule) {
        _currentRenderModule->handleMotion(x, y);
    }
}

void
RenderApp::handleMouse(int button, int state, int x, int y) {
    if (_currentRenderModule) {
        _currentRenderModule->handleMouse(button, state, x, y);
    }
}

void 
RenderApp::handleDisplay(void) {
    
    AC_DEBUG << "RenderApp::handleDisplay()";

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_currentRenderModule) {
        _currentRenderModule->renderPreSync();
    }
    
    if (_currentRenderModule) {
        _currentRenderModule->renderPostSync();
    }
    
    // do the color correction
    if (_colorCorrectionActive) {

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        glOrtho( -0.5, 0.5, -.5, 0.5, -101., 101);
        glMatrixMode( GL_MODELVIEW );

        glLoadIdentity();

        glColor4f(1, 1, 1, 1);
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glLineWidth(1.);
        //_colorCorrection.display();
    }

    if (_currentRenderModule) {
        _currentRenderModule->renderPostEdgeBlend();
    }
    
    glutSwapBuffers();
}

void 
RenderApp::handleReshape(int w, int h)
{
    if (_verbose) {
        AC_DEBUG << "reshape(): w " << w << " h " << h << endl;
    }
    _screenWidth = w;
    _screenHeight = h;

    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    if (_currentRenderModule) {
        _currentRenderModule->handleReshape(w, h);
    }
}


void 
RenderApp::handlePeriodic() {
    
    asl::Time  nowTime;
    double  runningTime = (double(nowTime) - double(_startTime)) ;

    bool needsRedraw = false;

    if (_currentRenderModule) {

        _currentRenderModule->handlePeriodic(runningTime, 0, needsRedraw);
    }
    
    if (needsRedraw) { 
        glutPostRedisplay();
    }
}

void 
RenderApp::handleVisible(bool isVisible) {

    if (_currentRenderModule) {
        _currentRenderModule->handleVisible(isVisible);
    }
    
	if (isVisible) {
		glutIdleFunc( periodic );
	} else {
		glutIdleFunc( NULL );
	}
}


void 
RenderApp::handleSpecialKeys( int key, int x, int y ) {
    if (_currentRenderModule) {
        _currentRenderModule->handleSpecialKeys(key, x, y);
    }
}


#define COLOR_KEY(c, C, bias, var)                                                      \
        case c:                                                                         \
            var = max(0.f, var - bias);                                                 \
            AC_DEBUG << "ColorCorrection: " << #var << " value now " << var << endl;        \
            _colorCorrection.setColor(_selectedVertex, _redValue, _greenValue,          \
                                      _blueValue, _alphaValue);                         \
            break;                                                                      \
        case C:                                                                         \
            var = min(1.f, var + bias);                                                 \
            AC_DEBUG << "ColorCorrection: " << #var << " value now " << var << endl;        \
            _colorCorrection.setColor(_selectedVertex, _redValue, _greenValue,          \
                                      _blueValue, _alphaValue);                         \
            break;                                                                      

void
RenderApp::handleKeyboard(unsigned char key, int x, int y) {
    
    // Escape quits
    if (key == '\033') {
        shutdown();
        exit(0);
    }

    // use return key to toggle between main app and current module
    if (key == 13) {
        _keepEvents = !_keepEvents;
    }
  
    bool doHandleEvents = _keepEvents;
    if (!_keepEvents) {
        if (_currentRenderModule) {
            // if module handles events, we ignore it
            doHandleEvents = !_currentRenderModule->handleKeyboard(key, x, y);
        }
    }
    if (! doHandleEvents) {
        return; // events are already handled
    }
    switch (key) {
        case 'c':
            _showCursor = !_showCursor;
            glutSetCursor( _showCursor ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
            break;
        case 'd':
            debugGLUT();
            break;

        case 'f':    // Toggle fullscreen mode  // XXX doesn't work yet
            // glutFullScreen(); // maximize but no fullscreen
            _fullscreen = !_fullscreen;
            initWindow();
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


void 
RenderApp::initWindow() {
    AC_DEBUG << "RenderApp::initWindow()";

    if (_verbose) {
        AC_DEBUG << "initWindow():" << endl
            << "\tcurrent window id " << glutGetWindow () << endl
            << "\tfullscreen " << (int) _fullscreen << endl
            << endl;
    }
    
    
    // check to toggle fullscreen

    if (! _fullscreen) {
        if (glutGameModeGet (GLUT_GAME_MODE_ACTIVE) && (glutGetWindow() != _originalWindow)) {
            glutLeaveGameMode(); 
        }
    }

    if (! _fullscreen) {

        // create window if its not there
        if (_originalWindow == -1) {
            glutInitWindowSize(_desiredWindowWidth, _desiredWindowHeight);
            //_edgeBlender.reshape( _desiredWindowWidth, _desiredWindowHeight );
            _originalWindow = glutCreateWindow(_arguments.getProgramName().c_str());
            if (_verbose) {
                AC_DEBUG << "initWindow(): created window with id " << _originalWindow << endl;
            }
        }
	
        // glutSetWindow(_originalWindow);
        glutPostWindowRedisplay (_originalWindow);
    } else {

        // Obsolete in LINUX-Environment. The game mode 'window' 
        // is always borderless and maximized.
        glutGameModeString("1280x1024:16@60");

        _screenWidth = __glutScreenWidth;
        _screenHeight = __glutScreenHeight;
        
        _originalWindow = glutEnterGameMode ();

    }
    
    if (!_windowInitialized) {
        registerGlutCallbacks();
    }
    
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, 0.5, -0.5, 0.5, -1.0, 1.0);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutSetCursor( _showCursor ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
    
    RenderModuleMap::iterator iter(_renderModules.begin());
    while (iter != _renderModules.end()) {
        IRenderAppModule * module = iter->second;
        assert(module);
        if (_windowInitialized) {
            module->updateWindow();
        } else {
            module->initWindow();
        }
        iter++;
    }
    AC_DEBUG << "RenderApp::initWindow() finished";
    
    _windowInitialized = true;
}


bool
RenderApp::handleArguments(int argc, char *argv[]) {
    AC_DEBUG << "RenderApp::handleArguments()";
    
    _arguments.parse( argc, argv );
    
    if (_arguments.haveOption("--help")) {
        printUsage();
        return false;
    }
    
    // INIT GLUT
    glutInit(&argc, argv);       

    return true;
}

bool
RenderApp::registerModule(IRenderAppModule & theModule) {
    AC_DEBUG << "RenderApp::registerModule " << theModule.getName();

    if (_renderModules[theModule.getName()]) {
        AC_DEBUG << "RenderApp::registerModule " << theModule.getName() << " failed";
        return false;
    }
    _renderModules[theModule.getName()] = &theModule;
    _currentRenderModule = &theModule;
    AC_DEBUG << "RenderApp::registerModule " << theModule.getName() << " OK";
    return true;
}


bool
RenderApp::activateModule(IRenderAppModule * theModule) {
    AC_DEBUG << "RenderApp::activateModule \"" << theModule->getName() << "\"";
    if (theModule == _currentRenderModule) {
        return true;
    }
    
    if (_currentRenderModule) {
        _currentRenderModule->deactivate();
    }
    _currentRenderModule = theModule;
    
    if (_currentRenderModule) {
        _currentRenderModule->activate();
        _currentRenderModule->updateWindow();
    }
    return true;
}
    
bool
RenderApp::activateModule(const string & theModuleName) {
    AC_DEBUG << "RenderApp::activateModule \"" << theModuleName << "\"";

    RenderModuleMap::iterator iter = _renderModules.find(theModuleName);
    if (iter !=  _renderModules.end()) {
        IRenderAppModule * newModule = iter->second;
    
        return activateModule(newModule);
    } else {
        return false;
    }
}

bool
RenderApp::startMainLoop() {
    AC_DEBUG << "RenderApp::startMainLoop()";

    if (_startModule.length()) {
        activateModule(_startModule);
    }
    _startTime.setNow();

    initWindow ();

    if (_currentRenderModule) {
        AC_DEBUG << "    current module is " << _currentRenderModule->getName();
        glutMainLoop();
    } else {
        AC_DEBUG << "RenderApp::startMainLoop(): No module running";
    }
    return true;
}

bool
RenderApp::preInit() { 
    AC_DEBUG << "RenderApp::preInit()";
    
    _defaultOptions = gRenderAppAllowedOptions;
    _arguments.addAllowedOptions(_defaultOptions);
    
    RenderModuleMap::iterator iter(_renderModules.begin());
    while (iter != _renderModules.end()) {
        IRenderAppModule * module = iter->second;
        assert(module);
        module->addAllowedOptions(_arguments);
        iter++;
    }
    return true;
}

bool
RenderApp::init() { 
    AC_DEBUG << "RenderApp::init()";
    

    if (_arguments.haveOption("--window")) {
        _fullscreen = false;
        _showCursor = true;
    }
    if (_arguments.haveOption("--fullscreen")) {
        _fullscreen = true;
        _showCursor = false;
    }
    if (_arguments.haveOption("--verbose")) {
        _verbose = 1;
    }
    if (_arguments.haveOption("--statistic")) {
        _statistic = true;
    }
    if (_arguments.haveOption("--fps")) {
        _fps = asl::as<double>(_arguments.getOptionArgument("--fps"));
    }
    if (_arguments.haveOption("--start-module")) {
        _startModule = _arguments.getOptionArgument("--start-module");
    }
    if (_arguments.haveOption("--realtime")) {
        if (!setRealtimePriority()) {
            AC_DEBUG << "Warning: Setting realtime priority failed (You have to be root!)" << endl;
        }
    }
    if (_arguments.haveOption("--color-correction")) {
        _colorCorrectionFileName = asl::as<string>(_arguments.getOptionArgument("--color-correction"));
    }
    if (_arguments.haveOption("--display-config")) {
        _colorCorrectionFileName = asl::as<string>(_arguments.getOptionArgument("--display-config"));
    }
    if (_colorCorrectionFileName.length() > 0) {

        string theDocumentFile = asl::readFile(_colorCorrectionFileName);
        if (theDocumentFile.length() == 0) {
            AC_DEBUG << "RenderApp: could not find display configuration xml file " 
                 << _colorCorrectionFileName << endl;
        }
        dom::Node colorConfig = dom::Document(theDocumentFile);
        if (!colorConfig) {
            AC_DEBUG << "RenderApp: could not parse xml file " << _colorCorrectionFileName << endl;
        }
        //AC_DEBUG << colorConfig << endl;   
        
    } else {
        setupColorCorrection();
    }

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // FRAME SYNC FACILITY 

    unsigned short udpPort = 1234; // XXX defaultUDPPort;
    if (_arguments.haveOption("--port")) {
        udpPort = asl::as<unsigned short>(_arguments.getOptionArgument("--port"));
    }
    
    RenderModuleMap::iterator iter(_renderModules.begin());
    while (iter != _renderModules.end()) {
        IRenderAppModule * module = iter->second;
        assert(module);
        module->init(_arguments);
        iter++;
    }
    
    return true;
}

void
RenderApp::registerGlutCallbacks() {
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialUpFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(periodic);
    glutVisibilityFunc(visible);
}

void 
RenderApp::motion(int x, int y) {
    RenderApp::instance().handleMotion(x, y);
}

void 
RenderApp::mouse(int button, int state, int x, int y) {
    RenderApp::instance().handleMouse(button, state, x, y);
}

void
RenderApp::display() {
    RenderApp::instance().handleDisplay();
}

void
RenderApp::reshape(int w, int h) {
    RenderApp::instance().handleReshape(w, h);
}

void
RenderApp::periodic() {
    RenderApp::instance().handlePeriodic();
}

void
RenderApp::key(unsigned char key, int x, int y) {
    RenderApp::instance().handleKeyboard(key, x, y);
}

void 
RenderApp::visible( int vis ) {
    RenderApp::instance().handleVisible(vis == GLUT_VISIBLE);
}

void 
RenderApp::specialKeys( int key, int x, int y ) {
    RenderApp::instance().handleSpecialKeys(key, x, y);
}
 

}; // namespace video


