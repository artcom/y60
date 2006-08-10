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

//#include <asl/Debug.h>
#include <asl/numeric_functions.h>
#include <asl/file_functions.h>
//#include <Yr/Time.h>
//#include <Yr/Sockets.h>

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

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------



//const char* BorderName[] = { "TOP", "BOTTOM", "LEFT", "RIGHT"};
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
/*
    {"--correct-hsteps",      "%d"},
    {"--correct-vsteps",      "%d"},
 */
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
   // _syncSlave(0),
    _fps(30),
    _fullscreen(false), _verbose(false), 
    _originalWindow(-1), _statistic(false),
    //_changeBlendRange(EdgeBlender::TOP), 
    _showCursor(true), _keepEvents(false),
    _screenWidth(0), _screenHeight(0),
    _desiredWindowWidth(DefaultWindowWidth), 
    _desiredWindowHeight(DefaultWindowHeight),
    _windowInitialized(false),
    _colorCorrectionActive(false),
    _selectedVertex(0),
    //_correctionMode(ColorCorrection33::CORRECT_REVERSE_SUBTRACT),
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

    /*
    if (_syncSlave) {
        delete _syncSlave;
        _syncSlave = 0;
    }
    */
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

/*
void
RenderApp::setGamma(EdgeBlender::BORDER border, float gamma) {
    if (_verbose) {
        AC_DEBUG <<"setGamma(): set BlendValues of Border: " << BorderName[border] <<
            " with Gamma: " << gamma << " resulting in: "<<endl;
    }
    vector<float> theBlendValues;
    int points = int(_edgeBlender.getWidth(border));
    for (int i = 0; i < points; i++) {
        float value =  pow((i/ float(points-1)), (1.0/gamma) );
        theBlendValues.push_back( 1.0 - value );
        if (_verbose) {
            AC_DEBUG << "\tpixel " << i << ": " << value << endl;
        }
    }
    if (_verbose) {
        cout << endl;
    }
    _edgeBlender.setBlendValues(border,  theBlendValues );
    _edgeBlender.setGamma(border,gamma);
}

ColorCorrection33::CorrectionMode
modeFromName(const string & modeName) {

    ColorCorrection33::CorrectionMode correctionMode = ColorCorrection33::CORRECT_ADD;
    if (modeName == "add") {
            correctionMode = ColorCorrection33::CORRECT_ADD;
        } else if (modeName == "subtract") {
            correctionMode = ColorCorrection33::CORRECT_SUBTRACT;
        } else if (modeName == "reversesubtract") {
            correctionMode = ColorCorrection33::CORRECT_REVERSE_SUBTRACT;
        } else {
            AC_DEBUG << "#ERROR: RenderApp::setupColorCorrection() invalid mode name "
                 << "'" << modeName << "'" << endl;
        }
        AC_DEBUG << "#INFO: RenderApp::setupColorCorrection() mode is " << modeName 
             << " (" << int(correctionMode) << ")" << endl;
    return correctionMode;
}
*/


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
    //_colorCorrection.setArea(-0.5f, -0.5f, 0.5f, 0.5f);
    //_colorCorrection.setArea(-500, -500, 500, 500);
    //_colorCorrection.setMode(modeFromName(modeName));
    /*
    for (int i = 0; i < 9; ++i) {
        _colorCorrection.setColor(i, r, g, b, a);
    }
    */
}
 
void
RenderApp::setupDisplayConfiguration(dom::Node & theConfig) {
    /*
    AC_DEBUG << "RenderApp::setupDisplayConfiguration()";
    _colorCorrectionActive = true;

    //_colorCorrection.setArea(-0.5f, -0.5f, 0.5f, 0.5f);
    
    try {

        // edge blending

        if (theConfig("color-correction").GetChild("edge-blending")) {

            const xml::Node & edgeBlendConfig(theConfig("color-correction")("edge-blending"));
       
            int i = 0;
            int numEdges = 0;
            while (const xml::NodePtr edgeNode = 
                    edgeBlendConfig.GetChild("edge", i)) 
            {
                const string & edgeName = (*edgeNode)["name"].Value();
                const string & edgeWidth = (*edgeNode)["width"].Value();
                const string & edgeGamma = (*edgeNode)["gamma"].Value();

                EdgeBlender::BORDER edgeCode = EdgeBlender::TOP;

                if (edgeName == "left") {
                    edgeCode = EdgeBlender::LEFT;
                } else if (edgeName == "right") {
                    edgeCode = EdgeBlender::RIGHT;
                } else if (edgeName == "top") {
                    edgeCode = EdgeBlender::TOP;
                } else if (edgeName == "bottom") {
                    edgeCode = EdgeBlender::BOTTOM;
                } else {
                    AC_ERROR("Edge name '" << edgeName << "' is not valid");
                } 

                float   gamma = 1.f;
                float   width = 0.f;

                if (sscanf(edgeGamma.c_str(), "%f", &gamma) != 1) {
                    AC_ERROR("edge gamma value '" << edgeGamma << "' is no float value");
                    gamma = 1.0f;
                }
                if (sscanf(edgeWidth.c_str(), "%f", &width) != 1) {
                    AC_ERROR("edge width value '" << edgeWidth << "' is no float value");
                    width = 0.0f;
                }
                AC_DEBUG << "Edge: " << edgeName << "  width= " << width << "  gamma= " << gamma; 
                _edgeBlender.addBorder(edgeCode, width, gamma);

                numEdges++;
                i++;
            }
        }
    }
    
    catch (...) {
        AC_WARNING("RenderApp:: in setupDisplayConfiguration():");
        AC_WARNING("No 'edge-blending' node");
    }

    try {
        // color correction values
        const xml::Node & colorConfig(theConfig("color-correction"));
          
        ColorCorrection33::CorrectionMode correctionMode = ColorCorrection33::CORRECT_ADD;

        string modeName("add");
        ::fromNode(colorConfig("mode")("string"), modeName);
        
        if (modeName == "add") {
            correctionMode = ColorCorrection33::CORRECT_ADD;
        } else if (modeName == "subtract") {
            correctionMode = ColorCorrection33::CORRECT_SUBTRACT;
        } else if (modeName == "reversesubtract") {
            correctionMode = ColorCorrection33::CORRECT_REVERSE_SUBTRACT;
        } else {
            AC_DEBUG << "#ERROR: RenderApp::setupDisplayConfiguration() "
                    "invalid mode name " << "'" << modeName << "'" << endl;
        }
        AC_DEBUG << "#INFO: RenderApp::setupDisplayConfiguration() mode is " << modeName 
             << " (" << int(correctionMode) << ")" << endl;

        _colorCorrection.setMode(correctionMode);

        int hSteps = 0, vSteps = 0;
        ::fromNode(colorConfig("hsteps")("int"), hSteps);
        ::fromNode(colorConfig("vsteps")("int"), vSteps);

        if ((hSteps != 3) || (vSteps != 3)) {
            AC_DEBUG << "#ERROR: RenderApp::setupDisplayConfiguration() hsteps "
                    "and vsteps are fixed "
                    " to 3" << endl;
            assert((hSteps == 3) && (vSteps == 3));
        }
        
        
        int i = 0;
        int numColorValues = 0;
        while (const xml::NodePtr myColorNode = 
            colorConfig("colorvalues").GetChild("color", i)) 
        {
            float r=1.f, g=1.f, b=1.f, a=0.f;
            ::fromNode((*myColorNode)("red")("float"), r);
            ::fromNode((*myColorNode)("green")("float"), g);
            ::fromNode((*myColorNode)("blue")("float"), b);
            ::fromNode((*myColorNode)("alpha")("float"), a);
            _colorCorrection.setColor(i, r, g, b, a);
            numColorValues++;
            i++;
        }
        
        if (numColorValues != (hSteps * vSteps)) {
            AC_DEBUG << "#ERROR: RenderApp::setupDisplayConfiguration() "
                    "number of values in colorvalues"
                    " must be hsteps * vsteps =" << (hSteps * vSteps) << " but is= " 
                 << numColorValues << endl;
        }
    
    }
    catch (const asl::Exception & ex) {
        AC_ERROR("RenderApp:: in setupDisplayConfiguration():");
        AC_ERROR(ex.what() << " AT " << ex.where());
        AC_ERROR("Aborting");
        exit(1);
    }
    catch (...) {
        AC_ERROR("RenderApp:: in setupDisplayConfiguration():");
        AC_ERROR("Aborting");
        exit(1);
    }
*/

}

void
RenderApp::writeDisplayConfiguration() {
    /*
    AC_DEBUG << "RenderApp::writeDisplayConfiguration(()";
    

    ofstream outFile(_colorCorrectionFileName.c_str());
    if (outFile) {
        AC_DEBUG << "ColorCorrection: writing XML file " << _colorCorrectionFileName << endl;
       

        
        outFile << "<color-correction>\n";
        outFile << "  <mode>\n";
        outFile << "    <string>";
        switch (_correctionMode) {
            case ColorCorrection33::CORRECT_ADD:
                outFile << "add";
            break;
            case ColorCorrection33::CORRECT_SUBTRACT:
                outFile << "subtract";
            break;
            case ColorCorrection33::CORRECT_REVERSE_SUBTRACT:
                outFile << "reversesubtract";
            break;
        }
        outFile << "</string>\n";
        outFile << "  </mode>\n";
        outFile << "  <hsteps>\n  <int>3</int>\n";
        outFile << "  </hsteps>\n";
        outFile << "  <vsteps>\n  <int>3</int>\n";
        outFile << "  </vsteps>\n";
        outFile << "  <colorvalues>\n";

        for (int i=0; i<9; i++) {
            float r, g, b, a;
            _colorCorrection.getColor(i, r, g, b, a);
            outFile << "    <!-- " << i << " -->\n";
            outFile << "    <color>\n";
            outFile << "      <red><float>" << r << "</float></red>\n";            
            outFile << "      <green><float>" << g << "</float></green>\n";            
            outFile << "      <blue><float>" << b << "</float></blue>\n";            
            outFile << "      <alpha><float>" << a << "</float></alpha>\n";            
            outFile << "    </color>\n";
           
        }
        
        outFile << "  </colorvalues>\n";

        outFile << "<edge-blending>\n";

        for (int border=0; border <4; border++) 
        {
            EdgeBlender::BORDER borderCode = (EdgeBlender::BORDER)border;
            outFile << "    <edge name=\"" << BorderName[borderCode] << "\" width=\""
                    << _edgeBlender.getWidth(borderCode) << "\" gamma=\""
                    << _edgeBlender.getGamma(borderCode) << "\"/>\n";
        }
        outFile << "</edge-blending>\n\n\n";
        
        outFile << "</color-correction>\n\n";
        
    }
    
    //_colorCorrectionFileName
    */
}


void
RenderApp::setupBlending() {
  /*
  AC_DEBUG << "RenderApp::setupBlending()";

    bool blendVecSet = false;
    vector<float> theBlendValues;
    
    if (_arguments.haveOption("--blend-to")) {
        blendVecSet = true;
        string blendTo = _arguments.getOptionArgument("--blend-to");
        istrstream is(blendTo.c_str());
        float myFloat;
        char mySepChar = ',';
        while (is >> myFloat) {
            theBlendValues.push_back(1.0 - myFloat);
            if (_verbose) {
                AC_DEBUG << "setupBlending(): Blend-to value " << myFloat << endl;
            }
            if (is >> mySepChar) {
                if (mySepChar != ',') {
                    AC_DEBUG << "*** WARNING strange blend-to value separator ignored :" 
                         << mySepChar << endl;
                }
            }
        }
    }

    if (_arguments.haveOption("--gamma-top")) {
        _borderGamma[EdgeBlender::TOP] = asl::as<float>(
                _arguments.getOptionArgument("--gamma-top"));
    }
    if (_arguments.haveOption("--gamma-bottom")) {
        _borderGamma[EdgeBlender::BOTTOM] = asl::as<float>(
                _arguments.getOptionArgument("--gamma-bottom"));
    }
    if (_arguments.haveOption("--gamma-left")) {
        _borderGamma[EdgeBlender::LEFT] = asl::as<float>(
                _arguments.getOptionArgument("--gamma-left"));
    }
    if (_arguments.haveOption("--gamma-right")) {
        _borderGamma[EdgeBlender::RIGHT] = asl::as<float>(
                _arguments.getOptionArgument("--gamma-right"));
    }

    _blendRange[EdgeBlender::TOP] = 0; 
    _blendRange[EdgeBlender::BOTTOM] = 0;
    _blendRange[EdgeBlender::LEFT] = 0;
    _blendRange[EdgeBlender::RIGHT] = 0;

    if (_arguments.haveOption("--blend-top")) {
        //int blendTop = 32;
        _blendRange[EdgeBlender::TOP] = asl::as<int>(
                _arguments.getOptionArgument("--blend-top"));
        _edgeBlender.addBorder(video::EdgeBlender::TOP, _blendRange[EdgeBlender::TOP] );
        if (blendVecSet) {
            _edgeBlender.setBlendValues(video::EdgeBlender::TOP, theBlendValues );
        } else {
            setGamma(EdgeBlender::TOP, _borderGamma[EdgeBlender::TOP]);

        }
    }

    if (_arguments.haveOption("--blend-bottom")) {
        //int blendBottom = 32;
        _blendRange[EdgeBlender::BOTTOM] = asl::as<int>(
                _arguments.getOptionArgument("--blend-bottom"));
        _edgeBlender.addBorder(video::EdgeBlender::BOTTOM, _blendRange[EdgeBlender::BOTTOM]  );
        if (blendVecSet) {
            _edgeBlender.setBlendValues(video::EdgeBlender::BOTTOM, theBlendValues );
        } else {
            setGamma(EdgeBlender::BOTTOM, _borderGamma[EdgeBlender::BOTTOM]);

        }
    }

    if (_arguments.haveOption("--blend-left")) {
        //int blendLeft = 32;
        _blendRange[EdgeBlender::LEFT] = asl::as<int>(
                _arguments.getOptionArgument("--blend-left"));
        _edgeBlender.addBorder(video::EdgeBlender::LEFT, _blendRange[EdgeBlender::LEFT] );
        if (blendVecSet) {
            _edgeBlender.setBlendValues(video::EdgeBlender::LEFT, theBlendValues );
        } else {
            setGamma(EdgeBlender::LEFT, _borderGamma[EdgeBlender::LEFT]);

        }
    }

    if (_arguments.haveOption("--blend-right")) {
        //int blendRight = 32;
        _blendRange[EdgeBlender::RIGHT] = asl::as<int>(
                _arguments.getOptionArgument("--blend-right"));
        _edgeBlender.addBorder(video::EdgeBlender::RIGHT, _blendRange[EdgeBlender::RIGHT] );
        if (blendVecSet) {
            _edgeBlender.setBlendValues(video::EdgeBlender::RIGHT, theBlendValues );
        } else {
            setGamma(EdgeBlender::RIGHT, _borderGamma[EdgeBlender::RIGHT]);

        }
    }

*/
}

void 
RenderApp::printUsage()  
{
    AC_DEBUG <<"usage: renderslave moviefilename1 [moviefilename2]"<<endl;
    _arguments.printUsage();
}

void debugGLUT  () {
    AC_DEBUG  << "debugGLUT ():" << endl
        << "\tcurrent window id: " << glutGetWindow ()
        << endl ;
    AC_DEBUG  << "debugGLUT ():" << endl
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
    
    /*
    if (_syncSlave) {
        SceneSyncMaster::SceneSyncPacket    mySyncPacket;

        // debug
        // AC_DEBUG << "display(): current packet # "<<  _currentSyncPacket._header._packetNumber << endl;
        
        if (_syncSlave->waitForSyncPacket(&mySyncPacket)) {
            _newSyncPacket = mySyncPacket;
            if (_newSyncPacket._moduleName[0]) {
                activateModule(string(_newSyncPacket._moduleName));
            } 
        } else {
            AC_ERROR ("display(): error in receiving sync packet");
        }
    };
    */
    //handleSceneCommand(_newSyncPacket._sceneCmd);
    
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

    // do the blending
    //_edgeBlender.display();

    if (_currentRenderModule) {
        _currentRenderModule->renderPostEdgeBlend();
    }
    
    glutSwapBuffers();
}

void
RenderApp::handleSceneCommand(int theSceneCommand) {

/*
    if (theSceneCommand == SceneSyncMaster::SceneSyncPacket::RESET_CMD) {
    
        AC_DEBUG << "RenderApp::handleSceneCommand RESET_CMD";
        
        if (_colorCorrectionFileName.length() > 0) {
            
            string theDocumentFile = asl::getWholeFile(_colorCorrectionFileName);
            if (theDocumentFile.length() == 0) {
                AC_WARNING("Received remote reset cmd: Unable to load color correction "
                        "config file '" << _colorCorrectionFileName << "'");
            }
            xml::Node colorConfig = xml::Document(theDocumentFile);
            if (!colorConfig) {
                AC_DEBUG << "RenderApp: could not parse xml file " << _colorCorrectionFileName << endl;
            }
            //AC_DEBUG << colorConfig << endl;   
        
            setupDisplayConfiguration(colorConfig);

        } else {

            AC_WARNING("Received remote reset cmd: Unable to load color correction config file");
            setupColorCorrection();
        }
        
        if (_currentRenderModule) {
            _currentRenderModule->handleResetCommand();
        }

    } else if (theSceneCommand == SceneSyncMaster::SceneSyncPacket::QUIT_CMD) {

        AC_DEBUG << "RenderApp::handleSceneCommand QUIT_CMD";
        
        if (_currentRenderModule) {
            _currentRenderModule->handleQuitCommand();
        }
        
        AC_DEBUG << "RenderApp: Received Quit Command - exiting" << endl;
        exit(0);
    } else if (theSceneCommand == SceneSyncMaster::SceneSyncPacket::NO_CMD) {
    } else {
        AC_WARNING("RenderApp::handleSceneCommand is UNKNOWN command");
    }
    */
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
    //_edgeBlender.reshape(w, h);
}


void 
RenderApp::handlePeriodic() {
    
    asl::Time  nowTime;
    double  runningTime = (double(nowTime) - double(_startTime)) ;

    bool needsRedraw = false;

    if (_currentRenderModule) {

        if ( 0 /*_syncSlave*/) {
            //_currentRenderModule->handlePeriodic(runningTime, &_newSyncPacket, needsRedraw);
        } else {
            _currentRenderModule->handlePeriodic(runningTime, 0, needsRedraw);
        }
        //_currentSyncPacket = _newSyncPacket;
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
#if 0
    switch (key) {
#if 1        
        case 'k':
            _changeBlendRange = EdgeBlender::TOP;
            cout <<" Activated Border: "<<BorderName[_changeBlendRange]<<endl;
            break;
        case 'j':
            _changeBlendRange = EdgeBlender::BOTTOM;
            cout <<" Activated Border: "<<BorderName[_changeBlendRange]<<endl;
            break;
        case 'h':
            _changeBlendRange = EdgeBlender::LEFT;
            cout <<" Activated Border: "<<BorderName[_changeBlendRange]<<endl;
            break;
        case 'l':
            _changeBlendRange = EdgeBlender::RIGHT;
            cout <<" Activated Border: "<<BorderName[_changeBlendRange]<<endl;
            break;
        case 'P':
            if (_blendRange[_changeBlendRange] >= 1 ) {
                _edgeBlender.setWidth(_changeBlendRange, 
                        _edgeBlender.getWidth(_changeBlendRange) + 1);
                if (_verbose) {
                    AC_DEBUG <<"key(): BlendRadius of Border: "<<BorderName[_changeBlendRange]<<" = "
                        << _edgeBlender.getWidth(_changeBlendRange) <<endl;
                }
            }
            setGamma(_changeBlendRange, _borderGamma[_changeBlendRange]);
            break; 
        case 'M':
            if (_blendRange[_changeBlendRange] >= 1 ) {
                _edgeBlender.setWidth(_changeBlendRange, 
                        _edgeBlender.getWidth(_changeBlendRange) - 1);

                if (_verbose) {
                    AC_DEBUG <<"key(): BlendRadius of Border: "<<BorderName[_changeBlendRange]<<" = "
                        << _edgeBlender.getWidth(_changeBlendRange) <<endl;
                }
            }
            setGamma(_changeBlendRange, _borderGamma[_changeBlendRange]);
            break;
        case 'I':
            {
                _borderGamma[_changeBlendRange] += 0.05;
                setGamma(_changeBlendRange, _borderGamma[_changeBlendRange] );
            }
            break; 
        case 'D':
            {
                _borderGamma[_changeBlendRange] -=0.05;
                setGamma(_changeBlendRange, _borderGamma[_changeBlendRange]);
            }
            break; 
#endif
        
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
        case 'm' :
            {
                RenderModuleMap::iterator it = _renderModules.begin();
                for (; it!=_renderModules.end(); ++it) {
                    if ((*it).second == _currentRenderModule) {
                        it++;
                        if (it!=_renderModules.end()) {
                            activateModule(((*it).second));
                        } else {
                            activateModule((*_renderModules.begin()).second); 
                        }
                        break;
                    }
                }
                AC_DEBUG << "RednerApp::handleKeyboard(): activate module: " << _currentRenderModule->getName());
            }
            break;
        
        COLOR_KEY('r', 'R', ((_correctionMode == ColorCorrection33::CORRECT_ADD) ? 1.f : -1.f) * 0.01f, _redValue)
        COLOR_KEY('g', 'G', ((_correctionMode == ColorCorrection33::CORRECT_ADD) ? 1.f : -1.f) * 0.01f, _greenValue)
        COLOR_KEY('b', 'B', ((_correctionMode == ColorCorrection33::CORRECT_ADD) ? 1.f : -1.f) * 0.01f, _blueValue)
//        COLOR_KEY('a', 'A', ((_correctionMode == ColorCorrection33::CORRECT_ADD) ? 1.f : 1.f) * 0.01f, _alphaValue)
//        COLOR_KEY('r', 'R', 0.01f, _redValue)
//        COLOR_KEY('g', 'G', 0.01f, _greenValue)
//        COLOR_KEY('b', 'B', 0.01f, _blueValue)
        COLOR_KEY('a', 'A', 0.01f, _alphaValue)
         
        case '+':
            AC_DEBUG << "ColorCorrection: correction mode is now 'add'" << endl;
            _correctionMode = ColorCorrection33::CORRECT_ADD;
            _colorCorrection.setMode(ColorCorrection33::CorrectionMode(_correctionMode));
            break;
        case '-':
            AC_DEBUG << "ColorCorrection: correction mode is now 'subtract'" << endl;
            _correctionMode = ColorCorrection33::CORRECT_SUBTRACT;
            _colorCorrection.setMode(ColorCorrection33::CorrectionMode(_correctionMode));
            break;
        case '_':
            AC_DEBUG << "ColorCorrection: correction mode is now 'reversesubtract'" << endl;
            _correctionMode = ColorCorrection33::CORRECT_REVERSE_SUBTRACT;
            _colorCorrection.setMode(ColorCorrection33::CorrectionMode(_correctionMode));
            break;
        case 'i':
            AC_DEBUG << "ColorCorrection: resetting vertex" << _selectedVertex << endl;
            _redValue = _greenValue = _blueValue = 1.f;
            _alphaValue = 0.f;
            _colorCorrection.setColor(_selectedVertex, _redValue, _greenValue,          
                                      _blueValue, _alphaValue);                        
            break;
        case 'w':
            

            writeDisplayConfiguration();

            break;
            
/*        
        case 'r':
            gRedValue -= 0.01;
            AC_DEBUG << "ColorCorrection: red value now " << gRedValue << endl;
            _colorCorrection.setColor(gSelectedVertex, gRedValue, gGreenValue, 
                                      gBlueValue, gAlphaValue);
            break; 
        case 'R':
            gRedValue += 0.01;
            AC_DEBUG << "ColorCorrection: red value now " << gRedValue << endl;
            _colorCorrection.setColor(gSelectedVertex, gRedValue, gGreenValue, 
                                      gBlueValue, gAlphaValue);
            break;
*/            
        default:
            if ((key >= '1') && (key <= '9')) {
                _selectedVertex = key - '1';
                AC_DEBUG << "ColorCorrection: selected vertex is now " << _selectedVertex << endl;
                _colorCorrection.getColor(_selectedVertex, _redValue, _greenValue, 
                                          _blueValue, _alphaValue);
            }
            return;
    }
#endif
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
        
        //_edgeBlender.reshape(_screenWidth, _screenHeight);

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
    

    // setup blending
    setupBlending();

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
        
        setupDisplayConfiguration(colorConfig);
        
    } else {
        setupColorCorrection();
    }

    if (_arguments.haveOption("--write-color-correction")) {
        _colorCorrectionFileName = asl::as<string>(_arguments.getOptionArgument("--write-color-correction"));        
        writeDisplayConfiguration();
    }

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // FRAME SYNC FACILITY 

    unsigned short udpPort = 1234; // XXX defaultUDPPort;
    if (_arguments.haveOption("--port")) {
        udpPort = asl::as<unsigned short>(_arguments.getOptionArgument("--port"));
    }
    if (_arguments.haveOption("--remote-control")) {
        /*
        _syncSlave = new SceneSyncSlave(udpPort);
        _syncSlave->enableVerboseMode(_verbose);
        _syncSlave->setFrequency(_fps);

        if (!_syncSlave->open()) {
            AC_DEBUG << "### ERROR Could not open remote-control UDP port!" << endl;
        }
        */
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


