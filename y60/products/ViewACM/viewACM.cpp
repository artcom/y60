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
// $RCSfile: viewCompressedTex.cpp,v $
// $Revision: 1.2 $
// $Date: 2002/09/06 18:17:15 $
// $Author: valentin $
//
// $Log $
//
// ----------------------------------------------------------------------------

#define DEBUG_LEVEL 1
#define SHOW_DATE 

#include <asl/base/Arguments.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/Time.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <string>
#include <vector>


#include <sys/types.h>
#include <sys/stat.h>

#include "CompressionType.h"
#include "FileBasedMovie.h"
#include "MovieScreen.h"
#include "MovieBase.h"


#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
#  include <strstream.h>
#else
#  include <sstream>
#endif


#define PAN     1
#define ROT     2
#define ZOOM    3

using namespace std;  // automatically added!
using namespace asl;
using namespace video;
using namespace TextureCompression;


// ----------------------------------------------------------------------------

static int globalWindowHeight = 96;
static int globalWindowWidth  = 128;

static float defaultPosX = 0;
static float defaultPosY = 0;
static float defaultScaleX = 1.0;
static float defaultScaleY = 1.0;
static float defaultTransX[] = {0.0, 0.0, 0.0, 0.0};
static float defaultTransY[] = {0.0, 0.0, 0.0, 0.0};

// ----------------------------------------------------------------------------

static float deltaScale = 0.0;
static float deltaTranslationX = 0.0, deltaTranslationY = 0.0;
static float deltaRotationX = 0, deltaRotationY = 0;

static int ox = -1, oy = -1;
static int mot = 0;

static int globalCursor = 1;
static int globalOriginalWindow = -1;

static double globalFps = 30.;
static bool globalVerbose = false;
static bool globalStatistic = false;

static unsigned long globalFrameCount = 0;

// ----------------------------------------------------------------------------

vector<MovieScreen*> globalMovieScreens;
vector<MovieBase*>   globalMovies;

Time* haltTime = 0;
Time* startTime = 0;
double runningTime = 0.0;
double lastRunningTime = 0.0;
int singleStepSize = 0;




void initWindow ();

void computeMovieScreensDefault () {

    if ( globalMovieScreens.size() > 1) {
        // compute scaling and positioning of moviescreens
        // in relation to number of screens
        defaultScaleX = 0.5;
        defaultScaleY = 0.5;

        defaultTransX[0] = -0.214; 
        defaultTransX[1] =  0.226;
        defaultTransX[2] = -0.214;
        defaultTransX[3] =  0.226;

        defaultTransY[0] =  0.226;
        defaultTransY[1] = -0.224;
        defaultTransY[2] = -0.224;
        defaultTransY[3] =  0.226;
    } else if ( globalMovieScreens.size() == 1) {
        defaultScaleX = 1.0;
        defaultScaleY = 1.0;

        defaultTransX [0] = 0.0;
        defaultTransY [0] = 0.0;
    } else if ( globalMovieScreens.size() == 0) {
        defaultScaleX = 1.0;
        defaultScaleY = 1.0;

        defaultTransX [0] = 0.0;
        defaultTransY [0] = 0.0;
    }
}

void setMovieScreensDefault () {
    for (int screen = 0; screen < globalMovieScreens.size() ; screen++) {
        globalMovieScreens[screen]->setPosition(defaultTransX[screen%4], defaultTransY[screen%4], 0.0f );
        globalMovieScreens[screen]->scaleScreen(defaultScaleX, defaultScaleY);
        globalMovieScreens[screen]->rotateScreen(0.0, 0.0, 0.0);
    }
}

void resetUI() {

    setMovieScreensDefault ();

    if (startTime && !haltTime) {
        delete startTime;
        startTime = new Time();      
    }

    deltaScale = 0.0;
    deltaTranslationX = 0.0;
    deltaTranslationY = 0.0;
    deltaRotationX = 0;
    deltaRotationY = 0;
    ox = -1; 
    oy = -1;
    mot = 0;

    globalCursor = 1;
    glutSetCursor( globalCursor ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
}


asl::Arguments::AllowedOption ourOptions[] = {
    {"--fps",            "%d"},
    {"--verbose",        ""  },
    {"--statistic",      ""  },
    {"--help",           ""  },
    {"--window",         ""  },
    {"--fullscreen",     ""  },
    {"--pos-x",          "%d"},
    {"--pos-y",          "%d"},
    {"",                 ""  }
};

asl::Arguments ourArguments(ourOptions);


void printUsage()  
{
    cerr <<"usage: viewCompressedTex moviefilename1 [moviefilename2]"<<endl;
    ourArguments.printUsage();
}

void debugGLUT  () {
    cerr  << "debugGLUT ():" << endl
        << "\tcurrent window id: " << glutGetWindow ()
        << endl ;
    cerr  << "debugGLUT ():" << endl
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
pan(int x, int y)
{
    deltaTranslationX= (x - ox) / 500.;
    deltaTranslationY= (y - oy) / -500.;
    ox = x;
    oy = y;
}

void
zoom(int x, int y)
{
  deltaScale = (y - oy) / 500.;
  oy = y;
}


void
rotate(int x, int y)
{
    deltaRotationX = x - ox;
    deltaRotationY = y - oy;
    ox = x;
    oy = y;
}

void
motion(int x, int y)
{
  //  cerr << " motion(): x " << x << " y " << y << endl;
  if (mot == PAN)
    pan(x, y);
  else if (mot == ROT)
    rotate(x, y);
  else if (mot == ZOOM)
    zoom(x, y);
    glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{

    if (state == GLUT_DOWN) {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                mot = PAN;
                motion(ox = x, oy = y);
                break;
            case GLUT_MIDDLE_BUTTON:
                mot = ROT;
                motion(ox = x, oy = y);
                break;
            case GLUT_RIGHT_BUTTON:
                mot = ZOOM;
                motion(ox = x, oy = y);
                break;
        }
    } else if (state == GLUT_UP) {
        mot = 0;
    }
}





void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int movies = 0; movies < globalMovieScreens.size() ; movies++) {

        double myPos[3];
        globalMovieScreens[movies]->getPosition( myPos[0], myPos[1], myPos[2] );

        globalMovieScreens[movies]->setPosition(myPos[0] + deltaTranslationX,
                myPos[1] + deltaTranslationY, 
                myPos[2] );

        double myScale[2];
        globalMovieScreens[movies]->getScale (myScale[0], myScale[1]);
        globalMovieScreens[movies]->scaleScreen(myScale[0] + deltaScale, myScale[1] + deltaScale);

        double myRot[3];
        globalMovieScreens[movies]->getRotation( myRot[0], myRot[1], myRot[2] );
        globalMovieScreens[movies]->rotateScreen(myRot[0] + deltaRotationX, 
                myRot[1] + deltaRotationY, 
                myRot[2]);

        ((MovieScreen*)(globalMovieScreens[movies]))->draw();
    }


    // reset interactive delta values
    deltaTranslationX = 0.0;
    deltaTranslationY = 0.0;
    deltaScale  = 0.0;
    deltaRotationX        = 0.0;
    deltaRotationY        = 0.0;
    
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (globalVerbose)
        cerr << "reshape(): w " << w << " h " << h << endl;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}


void findBiggestAspect (MovieBase * theMovie ) {

    // XXX check aspect against actual screen/window size

    // find the biggest aspects
    if ((theMovie->getWidthAspect() > globalWindowWidth) || 
            (theMovie->getHeightAspect() >  globalWindowHeight)) {
        globalWindowHeight = theMovie->getHeightAspect();
        globalWindowWidth  = theMovie->getWidthAspect();
    }

}

void clearFilebasedMovies () {

    for (int movies = 0; movies < globalMovieScreens.size(); movies++) {
        assert(globalMovies[movies]);
        delete globalMovies[movies];
        
        assert(globalMovieScreens [movies]);
        delete globalMovieScreens [movies];
    }

    globalMovies.clear();
    globalMovieScreens.clear();
}


bool openFilebasedMovie (const char * myFileName) {

    MovieScreen * theMovieScreen = 0;
    MovieBase *   theMovie = 0;

    theMovie = new video::FileBasedMovie(myFileName, globalStatistic);
    
    if (!theMovie->init()) {
        return false;
    } else {
        globalMovies.push_back( theMovie );
        //if (globalFps) {
            theMovie->setFPS(globalFps);
        //}
        theMovieScreen = new MovieScreen(MovieScreen::QUAD);

        theMovieScreen->init( );
        theMovieScreen->setCurrentFrame( theMovie );
        theMovieScreen->setPosition(defaultTransX[globalMovieScreens.size() ],
                    defaultTransY[globalMovieScreens.size()], 
                    0.0f );
        theMovieScreen->scaleScreen(defaultScaleX, defaultScaleY);
        theMovieScreen->rotateScreen(0.0, 0.0, 0.0);
        globalMovieScreens.push_back( theMovieScreen );

        findBiggestAspect (theMovie);

        return true;
    }
}

void periodic() {
    bool hasChanged = false;

    Time nowTime;
    runningTime = (double(nowTime) - double(*startTime)) ;

    if (haltTime) {

        // MODE: INTERACTIVE SINGLE FRAME CONTROL

        for (int movies = 0; movies < globalMovieScreens.size() ; movies++) {
            unsigned long nextFrame = globalMovies[movies]->getCurrentFrame() + singleStepSize;
            if (nextFrame == ULONG_MAX) 
                nextFrame = globalMovies[movies]->getFrameCount() - 1;
            if (nextFrame == globalMovies[movies]->getFrameCount())
                nextFrame = 0;


            if ( globalMovies[movies]->updateToFrame (nextFrame)) {
                hasChanged = true;
                if (globalVerbose)
                    cerr << "periodic(): stepped to frame # " << nextFrame << endl;
            }
        }
        if (hasChanged) {
            for (int moviescreens = 0; moviescreens < globalMovieScreens.size() ; moviescreens++) {
                if ( globalMovieScreens[moviescreens]->update()) {
                }
            }
        }

        singleStepSize = 0;

    } else { 

        // MODE: CONTINUOUS PLAYING MODE

        for (int movies = 0; movies < globalMovieScreens.size() ; movies++) {
            if ( globalMovies[movies]->updateToTime(runningTime)) {
                hasChanged = true;
            }
        }
        if (hasChanged) {
            for (int moviescreens = 0; moviescreens < globalMovieScreens.size() ; moviescreens++) {
                if ( globalMovieScreens[moviescreens]->updateToTime(runningTime)) {
                    //hasChanged = true;
                }
            }
        }
    }

    if (hasChanged) { 
        glutPostRedisplay();
    }
    double myDiffTime = (runningTime- lastRunningTime )*1000;
    double myDiffToFrameTime = ((1.0/globalMovies[0]->getFPS()) * 1000) - myDiffTime;
    if (myDiffToFrameTime > 0) {
        msleep(myDiffToFrameTime);
    }
    lastRunningTime = runningTime;
}


void
shutdown() {
    clearFilebasedMovies ();
}

void
key(unsigned char key, int x, int y) {
    switch (key) {
    case 'r':
        resetUI();
        break;
    case 's':
        for (int screen = 0; screen < globalMovieScreens.size(); screen++) {
            globalMovieScreens[screen]->setStatistic(!globalMovieScreens[screen]->getStatistic());  
        }
        break;
    case 'c':
        globalCursor = !globalCursor;
        glutSetCursor( globalCursor ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
        break;
    case 'd':
        debugGLUT();
        break;

    case '\033':  // Escape quits. 
        shutdown();
        exit(0);
        break;

    case ' ':
        // single frame control only if external sync control is disabled

        if (!haltTime) {
            haltTime = new Time();
            singleStepSize = 0;
            if (globalVerbose) cerr << "key(): " 
                << "single step mode - current frame# " << globalMovies[0]->getCurrentFrame() << endl;
        } else {
            Time nowTime;
            *startTime = *startTime + nowTime - *haltTime;
            delete haltTime;
            haltTime = 0;
            if (globalVerbose) cerr << "key(): " << "continuous playing mode" << endl;
        }
        break;

    case '/':
        singleStepSize = -1;
        break;

    case '*':
        singleStepSize = +1;
        break;

    default:
        cerr << "*** WARNING key(): unknown key '" << key << "'" << endl;
        return;
    }
    glutPostRedisplay();
}


void initWindow() {
    if (globalVerbose)
        cerr << "initWindow():" << endl
            << "\tcurrent window id " << glutGetWindow () << endl
            << endl;

    // create window if its not there
    if (globalOriginalWindow == -1) {
        glutInitWindowSize(globalWindowWidth, globalWindowHeight);
        globalOriginalWindow = glutCreateWindow(ourArguments.getProgramName().c_str());
        if (globalVerbose)
            cerr << "initWindow(): created window with id " << globalOriginalWindow << endl;
    }

    glutPostWindowRedisplay (globalOriginalWindow);
	// initialize glew
	unsigned int myGlewError = glewInit();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glClearColor (0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, 0.5, -0.5, 0.5, -1.0, 1.0);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutSetCursor( globalCursor ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
};



int main( int argc, char *argv[] )
{

    // ARGUMENT HANDLING

    ourArguments.parse( argc, argv );
    if (ourArguments.haveOption("--help")) {
        printUsage();
        return 0;
    }

    if (ourArguments.haveOption("--window")) {
        globalCursor = 1;
    }
    if (ourArguments.haveOption("--verbose")) {
        globalVerbose = 1;
    }
    if (ourArguments.haveOption("--statistic")) {
        globalStatistic = true;
    }
    if (ourArguments.haveOption("--fps")) {
        globalFps = asl::as<double>(ourArguments.getOptionArgument("--fps"));
    }


    // INIT GLUT
    glutInit(&argc, argv);       
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // CREATE movies and theaters nearby you
    clearFilebasedMovies ();
    // CREATE filebased movies

    for (int i = 0 ; i <  ourArguments.getCount(); i++) {
        if (! openFilebasedMovie (ourArguments.getArgument(i).c_str())) {
            cerr <<"### ERROR Could not open movie '" 
                << ourArguments.getArgument(i).c_str()
                <<"' -> not showing " << endl;
        }
    }

    // COMPUTE SCREEN DEFAULTS 

    if (ourArguments.haveOption("--pos-x")) {
        defaultPosX = asl::as<int>( ourArguments.getOptionArgument("--pos-x"));
    }
    if (ourArguments.haveOption("--pos-y")) {
        defaultPosY = asl::as<int>( ourArguments.getOptionArgument("--pos-y"));
    }
    computeMovieScreensDefault ();
    setMovieScreensDefault ();




    // CREATE screen


    initWindow ();

    glutIdleFunc(periodic);

    if (globalVerbose)
        cerr << "main(): Entering main loop" << endl;

    startTime = new Time();
    glutMainLoop();

    return 0;
}

