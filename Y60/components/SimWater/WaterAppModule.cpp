//=============================================================================
//
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#include "WaterAppModule.h"
#include "RenderApp.h"
#include "XmlTypes.h"

#include <asl/ThreadSemaphore.h>
#include <asl/file_functions.h>
#include <asl/Time.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
//#include <cmath>
#include <assert.h>
#include <list>
#include <vector>

#include <GL/glx.h>

#include <GL/glut.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>

//#include <Yr/math64.h>

#include "WaterRepresentation.h"
#include "WaterSimulation.h"

#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
#  include <strstream.h>
#else
#  include <sstream>
#endif


//using namespace ac;
using namespace video;


#define USE_AGP_MEMORY
#define COMPUTE_IN_SEPARATE_THREAD

#ifdef COMPUTE_IN_SEPARATE_THREAD
#include <asl/ThreadSemaphore.h>
//#include <asl/ThreadSemFactory.h>


using namespace std;  // automatically added!


#endif

#define TEST_WATER   

#ifdef USE_AGP_MEMORY

// taken from NVIDIAs glx header. needed for allocation of memory in fast AGP
// area

extern "C" {

extern void *glXAllocateMemoryNV(GLsizei size, GLfloat readfreq,
				 GLfloat writefreq, GLfloat priority);
extern void glXFreeMemoryNV(GLvoid *pointer);

}


#endif


namespace water_module {
    

inline float minimum(float a, float b) {
	return a < b ? a : b;
}
inline float maximum(float a, float b) {
	return a > b ? a : b;
}
inline float clamp(float v, float min, float max) {
    if (v > max) {
        v = max;
    } else
    if (v < min) {
        v = min;
    }
    return v;
}

//===========================================================================================

WaterSimulation *       gWaterSimulation = 0;
WaterRepresentation *   gWaterRepresentation = 0;
#ifdef COMPUTE_IN_SEPARATE_THREAD
bool                    gUseSeparateThread = false;
asl::ThreadSemaphore *        gComputeLock;
#endif


// global flags

int     gNumIntegrationsPerFrame = 3;

bool            gDrawRefraction = false; // 'R'
bool            gDrawReflection= true;  // 'r'
bool            gDrawCaustics = true;   // 'c'
unsigned int    gLastFrame = 0; // for UDP sync tracking
unsigned long   gLastEvent = 0;
dom::Node       gConfig;

/*
typedef vector<SceneSyncMaster::Event>  EventList;
EventList      gEventList;

SceneSyncMaster::FileObject     gFileObject[SceneSyncMaster::NUM_FILE_OBJECTS];
SceneSyncMaster::SceneObject    gSceneObject[SceneSyncMaster::NUM_SCENE_OBJECTS];
*/

//=======================================

//const int gSimulationWidth = 600;
//const int gSimulationHeight = 300;

//const int gSimulationWidth = 480;
//const int gSimulationHeight = 240;


// gSimulationHeight must be a multiple of 
//  (DISPLAY_HEIGHT_FRACTION * [WaterRepresentation::]NUM_VAR_BUFFERS)
const int SIMULATION_WIDTH = 480;
const int SIMULATION_HEIGHT = 240;

int gSimulationWidth = SIMULATION_WIDTH+2;
int gSimulationHeight = SIMULATION_HEIGHT+2;
int gSimulationOffsetX = 1;
int gSimulationOffsetY = 1;

int gWindowWidth = 1024;
int gWindowHeight = 768;

// we've got 3 by 2 displays:
int gDisplayWidth = SIMULATION_WIDTH / 3;
int gDisplayHeight = SIMULATION_HEIGHT / 2;

int gDisplayOffsetX = 0;
int gDisplayOffsetY = 0;


float gTimeStep = 0.1;   // speed of simulation
const float c = 2;//wave space, meters/sec
const float h = 2*float(0.1*c*sqrt(2.f));//grid cell width (min for stability, assuming dt<=0.1)

float   gRunSimulation = true;
bool    gDrawWireframe = false;
bool    gTestMarker = false;
float   gWaterDamping = 0.9993f;
#ifdef TEST_WATER   
bool    gTestWater = false;
float   gTestWaterRunTime = 0.f;
float   gTestWaterSpeed = 0.005f;
int     gTestWaterNumDrops = 5;
float   gTestWaterDropSize = 1.f;
float   gTestWaterDropIntensity = 1.f;
float   gSpecialWaterDamping = 0.98f;
#endif
bool    gDrawCrackField = false;
float   gKeyIntensity = 1.f;

/* ///////////////////////// INTERACTION STUFF /////////////////////// */


static float scale = 1.0;
static float transx = 0.0, transy = 0.0, transz = 0.0;
static float rotx = 0.0, roty = 0.0, rotz = 0.0; 
static int ox = -1, oy = -1;
static int mot;

int     gMousePositionX = 0;
int     gMousePositionY = 0;
int     gScreenWidth = 0;
int     gScreenHeight= 0;

#define PAN     1
#define ROT     2
#define ZOOM    3


/* ////////////////////////////// CUBE MAP STUFF ////////////////////// */


string gDataDir = "data";

#if 0
//These are the textures for the skybox.
const char *const GroundTextureName = "ground.png";



//These are the textures for the skybox.
const char *const SkyboxTexturesNames[6] = {
	"cube_right.png",
	"cube_left.png",
	"cube_top.png",
	"cube_bottom.png",
	"cube_roof.png",
	"cube_ground.png"
};
#endif

const char * const cubeSides[6] = {"right","left","top","bottom","front","back"};


int     gCrackSegmentSize = 4;
int     gNumSegmentsPerCrack = 8;
float   gCrackVariation = 2.;
int     gNumCracksPerSplash = 4;
float   gCrackWidth = .5;
float   gInnerIceThickness = 2.;
float   gOuterIceThickness = 4.;
float   gSplashIntensityFactor = 1.;
float   gSplashRadiusFactor = 1.;
float   gCrackIntensityFactor = 1.;
float   gCrackSqrIntensityFactor = 0.;
float   gSurfaceOpacity = 0.95;
bool    gEnableSurfaceCracks = true;
bool    gFreezeWater = false;

/////////////////////////////////////////////////////////////////////////////////////


asl::Arguments::AllowedOption ourOptions[] = {
#ifdef COMPUTE_IN_SEPARATE_THREAD
    {"--multithread",         ""  },
#endif

    {"--simulation-width",    "%d"},
    {"--simulation-height",   "%d"},
    {"--simulation-offset-x", "%d"},
    {"--simulation-offset-y", "%d"},
    
    {"--display-width",       "%d"},
    {"--display-height",      "%d"},
    {"--display-offset-x",    "%d"},
    {"--display-offset-y",    "%d"},
    
    {"--data-dir",            "%s"},
    {"--water-xml-config",    "%s"},

    {"--num-integrations",    "%d"},
    {"--timestep",            "%d"},
    {"--wireframe",           ""},
    {"--test-marker",         ""},
    {"--test-water",          ""},
    {"--freeze-water",        ""},
    {"",                      ""  }
};

#ifndef SQR
#define SQR(x) ((x)*(x))
#endif

#if 0
void
handleEvents(int currentFrame, EventList & theEventList) {
    int i=0;
    while (i < theEventList.size()) {
        //if (theEventList[i]._frameNumber <= gLastFrame) {
        if (theEventList[i]._frameNumber <= currentFrame) {
            //if (theEventList[i]._frameNumber > gLastFrame) {

            int frameNumber = theEventList[i]._frameNumber;
            int xPos = theEventList[i]._xPos;
            int yPos = theEventList[i]._yPos;
            int splashMagnitude = (int) (theEventList[i]._intensity * gSplashIntensityFactor);
            int splashRadius = (int) (float(theEventList[i]._intensity) * gSplashRadiusFactor);

            float eventIntensity = min(max(0.f, theEventList[i]._intensity), 2.f);
            float crackIntensity = eventIntensity * gCrackIntensityFactor +
                                   SQR(eventIntensity) * gCrackSqrIntensityFactor;
            
            theEventList.erase(&theEventList[i]);

            if (frameNumber == currentFrame) {
                //AC_DEBUG << "Splash _on_time_ (now " << (theEventList.size()) 
                //    << " in saved list) :\n  ";
            } else {
                //AC_DEBUG << "< Late splash (now " << (theEventList.size()) 
                //    << " in saved list) :\n< ";
            }

            //AC_DEBUG << "    splash frame #" << frameNumber << "  intensity= "
            //    << theEventList[i]._intensity << "  splashRadius= " << splashRadius
            //    << "  x= " << xPos << "  y= " << yPos << endl;
           
            srand48(((frameNumber % 0xff) << 24) | 
                    ((frameNumber % 0xff) << 16) |
                    ((frameNumber % 0xff) << 8) |
                    (frameNumber % 0xff));
            
            if ((gWaterRepresentation) && (gWaterRepresentation->surfaceEnabled()) &&
                gEnableSurfaceCracks) 
            {
                
                for (int i=0; i < gNumCracksPerSplash; i++) {

                    float directionX = random(-100, 100);
                    float directionY = random(-100, 100);
                    
                    gWaterRepresentation->addCrack(crackIntensity,
                            Vec2(xPos, yPos), Vec2(directionX, directionY),
                            gNumSegmentsPerCrack, 
                            (1. + eventIntensity) * gCrackSegmentSize, 
                            gCrackVariation);

                    gWaterRepresentation->addCrack(crackIntensity,
                            Vec2(xPos, yPos), Vec2(-directionX, -directionY),
                            gNumSegmentsPerCrack, 
                            (1. + eventIntensity) * gCrackSegmentSize, 
                            gCrackVariation);
                }
            }

            gWaterSimulation->sinoidSplash(xPos, yPos, splashMagnitude, splashRadius);

            i = 0;
            continue;
        }
        i++;
    }
}

void
extractEvents(int currentFrame, const SceneSyncMaster::SceneSyncPacket & mySyncPacket, 
              EventList & theEventList) 
{

    //AC_DEBUG << "num splashes: " << mySyncPacket._numEvents << endl;

    for (int i = mySyncPacket._numEvents-1; i >= 0; i--) {

        if (mySyncPacket._event[i]._eventType != SceneSyncMaster::Event::Splash) {
            continue;
        }
        
        int frameNumber = mySyncPacket._event[i]._frameNumber;
        //int xPos = mySyncPacket._event[i]._xPos;
        //int yPos = mySyncPacket._event[i]._yPos;
        int xPos = (int) (float(mySyncPacket._event[i]._xPos) / 10000. * float(gSimulationWidth));
        int yPos = (int) (float(mySyncPacket._event[i]._yPos) / 10000. * float(gSimulationHeight));

        int magnitude = (int) (mySyncPacket._event[i]._intensity * 4.);
        int radius = (int) (mySyncPacket._event[i]._intensity * 4.);
/*                         
                if (mySyncPacket._event[i]._frameNumber > currentFrame) {
                    
                    theEventList.push_back(mySyncPacket._event[i]);
                    AC_DEBUG << "> Saving splash for later (now " << theEventList.size() 
                         << " in saved list):\n> ";
                    
                } else 
                if (theEventList[i]._frameNumber > gLastFrame) {
                    AC_DEBUG << "Splashing now:\n";
                    gWaterSimulation->sinoidSplash(xPos, yPos, 
                            magnitude, radius);
                }
*/              
        bool    isDuplicate = false;
        int j=0;
        while (j < theEventList.size()) {
            if (theEventList[i]._eventNumber == 
                    mySyncPacket._event[i]._eventNumber) 
            {
                isDuplicate = true;
                break;
            }
            j++;
        }
        if (isDuplicate) {
            // if we already have that one forget it!
            //AC_DEBUG << "DUPLICATE:";
        } else {
            theEventList.push_back(mySyncPacket._event[i]);

            SceneSyncMaster::Event * event = &theEventList.back();
            assert(event); 
            // res-scale the event coordinates from 10000/10000 to simulation width
        
            event->_xPos = xPos;
            event->_yPos = yPos;
        }
        //AC_DEBUG << i << "# : splash frame #" << frameNumber << " intensity= " 
        //    << magnitude << "  radius= " << radius
        //    << "  x= " << xPos << "  y= " << yPos << endl;
    }

}
#endif

void
createTestMarker() {
    /*
    if (gWaterRepresentation) {
        for (int i=0; i<10; i++) {
            gSceneObject[i]._classID = SceneSyncMaster::SceneObject::Marker;
            gSceneObject[i]._objectID = i;
            gSceneObject[i]._fileClassID = WaterRepresentation::puzzlemaps;
            gSceneObject[i]._fileObjectID = i % 4 + 1;
            gSceneObject[i]._xPos = int((double(i) * (10000. / 9.)));
            gSceneObject[i]._yPos = int((double(i) * (10000. / 9.)));
            gSceneObject[i]._zPos = 0; // directly at water surface
//            gSceneObject[i]._zPos = 220; // above ice/surface (above 210)
            gSceneObject[i]._xSize = (i+2) * 150;
            gSceneObject[i]._ySize = (i+2) * 300;
            gSceneObject[i]._zSize = 0;
            gSceneObject[i]._heading = short(random(0., 3600));
        }
        gWaterRepresentation->setSceneObjects(gSceneObject);
    }
    */
}

void
clearTestMarker() {
    /*
    if (gWaterRepresentation) {
        for (int i=0; i< 10; i++) {
            gSceneObject[i]._classID = SceneSyncMaster::SceneObject::NoObject;
        }
        gWaterRepresentation->setSceneObjects(gSceneObject);
    }
    */
}

#ifdef TEST_WATER   

void
createTestWater() {
}

void
clearTestWater() {
}

double random(double min, double max) {
    return drand48() * (max - min) + min;
}


float grandom(float min, float max, int numCalculations=3) {
    float result = 0;
    for (int i=0; i < numCalculations; i++) {
        result += water_module::random(min, max);
    }
    return result / float(numCalculations);
}

const int WATER_PFEILUNG = 40;
//const float WATER_Y_RANDOM1 = 100.f;
//const float WATER_Y_RANDOM2 = 30.f;

void
updateTestWater(float pos) {

    //AC_DEBUG << "updateTestWater( " << pos << " )" << endl;
    if (!gWaterSimulation) {
        return;
    }
  
    float   waterRandomY1 = 100.f;
    float   waterRandomY2 = float(gSimulationHeight) / float(gTestWaterNumDrops) * 0.5f;
    
    static int runCount = -1;
    
    for (int i=0; i < 5; i++) {
        int xPos = -WATER_PFEILUNG + int(((gSimulationWidth + WATER_PFEILUNG) * 
                (1.f - pos)) + (2 - i) * (2 - i) * 
                grandom(WATER_PFEILUNG/2, WATER_PFEILUNG) * 0.5f);
        int yPos = int(float(gSimulationHeight/ 2) + (2 - i) * grandom(0, waterRandomY1) + 
                grandom(-waterRandomY2, waterRandomY2));
//AC_DEBUG << i << "#" << endl;
//AC_DEBUG << "x= " << xPos << "  y= " << yPos << endl;
        gWaterSimulation->sinoidSplash(xPos, yPos, 
                int( gTestWaterDropIntensity * grandom(gSplashIntensityFactor*.5, gSplashIntensityFactor)), 
                int( gTestWaterDropSize * grandom(gSplashRadiusFactor*.5, gSplashRadiusFactor*1.5)));
    }
}

/*
void
updateTestWater(float pos) {

    AC_DEBUG << "updateTestWater( " << pos << " )" << endl;
    if (!gWaterSimulation) {
        return;
    }
   
    static int runCount = -1;
    
    for (int i=-1; i < 2; i++) {
//        int xPos = int(10240 * (1.f - pos));
//        int yPos = 5000 + i * 2500;
        int xPos = (int)(160 * (1.f - pos)) + i * i * 17;
        int yPos = 60 + i * 30 + runCount * i * 23 - runCount * i * 7;
        gWaterSimulation->sinoidSplash(xPos, yPos, gSplashIntensityFactor, 
                gSplashRadiusFactor);
    }
    runCount++;
    if (runCount > 1) {
        runCount = -1;
    }
}
*/

#if 0

const int WATER_OBJECT = 10;
const int WATER_TEXTURE_ID = 100;
void
createTestWater() {
    if (gWaterRepresentation) {
        gSceneObject[WATER_OBJECT]._classID = SceneSyncMaster::SceneObject::Marker;
        gSceneObject[WATER_OBJECT]._objectID = WATER_OBJECT;
        gSceneObject[WATER_OBJECT]._fileClassID = WaterRepresentation::puzzlemaps;
        gSceneObject[WATER_OBJECT]._fileObjectID = WATER_TEXTURE_ID;
        //gSceneObject[WATER_OBJECT]._xPos = 1024;
        //gSceneObject[WATER_OBJECT]._yPos = 10000;
        gSceneObject[WATER_OBJECT]._zPos = 100; //  0 is directly at water surface
        //            gSceneObject[i]._zPos = 220; // above ice/surface (above 210)
        gSceneObject[WATER_OBJECT]._xSize = 6000;
        gSceneObject[WATER_OBJECT]._ySize = 32767;
        gSceneObject[WATER_OBJECT]._zSize = 0;
        gSceneObject[WATER_OBJECT]._heading = 900; // short(random(0., 3600));

        gWaterRepresentation->setSceneObjects(gSceneObject);
    }
}

void
clearTestWater() {
    if (gWaterRepresentation) {
        gSceneObject[WATER_OBJECT]._classID = SceneSyncMaster::SceneObject::NoObject;
        gWaterRepresentation->setSceneObjects(gSceneObject);
    }
}

void
updateTestWater(float pos) {
    if (gWaterRepresentation) {
        
        gSceneObject[WATER_OBJECT]._xPos = int(10240 * (1.f - pos));
        gSceneObject[WATER_OBJECT]._yPos = 5000;
        gSceneObject[WATER_OBJECT]._pitch = int(pos * 10000.);
        
        gWaterRepresentation->setSceneObjects(gSceneObject);
    }    
}
#endif

#endif


/*
void
updateObjects(int currentFrame, const SceneSyncMaster::SceneSyncPacket & mySyncPacket) {
    for (int i=0; i< SceneSyncMaster::NUM_FILE_OBJECTS; i++) {
        
        gFileObject[i] = mySyncPacket._fileObject[i];
//AC_DEBUG << i << ":\n##################### C " << theNewSyncPacket->_fileObject[i]._fileClassID<<endl;
//AC_DEBUG << "##################### I " << theNewSyncPacket->_fileObject[i]._fileID << endl;
        if ((gFileObject[i]._fileClassID == SceneSyncMaster::FileObject::GroundTexture) && 
            (gWaterRepresentation)) 
        {
            //TODO: shall be activateTexture and not activateTextureIndex
            gWaterRepresentation->activateTexture(WaterRepresentation::floormaps,gFileObject[i]._fileID);
        }
        if ((gFileObject[i]._fileClassID == SceneSyncMaster::FileObject::CubeMap) && 
            (gWaterRepresentation)) 
        {
            //gWaterRepresentation->activateCubeTextureIndex(gFileObject[i]._fileID);
            //TODO: shall be activateTexture and not activateTextureIndex
            gWaterRepresentation->activateTexture(WaterRepresentation::cubemaps,gFileObject[i]._fileID);
        }
        if ((gFileObject[i]._fileClassID == SceneSyncMaster::FileObject::SurfaceTexture) && 
            (gWaterRepresentation)) 
        {
            //gWaterRepresentation->activateCubeTextureIndex(gFileObject[i]._fileID);
            //TODO: shall be activateTexture and not activateTextureIndex
            gWaterRepresentation->activateTexture(WaterRepresentation::surfacemaps,gFileObject[i]._fileID);
        }
    }
    for (int i=0; i< SceneSyncMaster::NUM_SCENE_OBJECTS; i++) {
        //AC_DEBUG << "scene object " << i << "\t posx: " << mySyncPacket._sceneObject[i]._xPos << "\t posy: " 
        //     << mySyncPacket._sceneObject[i]._yPos << endl;
        
        gSceneObject[i] = mySyncPacket._sceneObject[i];
    }

    if (gTestMarker) {
        createTestMarker();
    }
#ifdef TEST_WATER       
    if (gTestWater) {
        createTestWater();
    }
#endif    
    if (gWaterRepresentation) {
        gWaterRepresentation->setSceneObjects(gSceneObject);
    }
}
    */

void
handleResetCommand() {
    gWaterSimulation->reset();
    //water_module::gWaterRepresentation->clearCracks();

/*
    while (gEventList.size() > 0) {
        gEventList.erase(&gEventList[0]);
    }
    */
}


void 
periodic(double theRunningTime, 
         SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, 
         bool & theNeedsRedraw) 
{
    theNeedsRedraw = true;

    assert(gWaterSimulation);

    if (theNewSyncPacket) {
// PREVIOUS SYNC POS        
        int currentFrame = gLastFrame + 1;
        //AC_DEBUG << "current frame= " << currentFrame << "  ==========================\n";

/*
        if (theNewSyncPacket->_header._packetNumber > (gLastFrame + 1)) {
            AC_DEBUG << "MISSED FRAMES (UDP packets): " << (theNewSyncPacket->_header._packetNumber - 
                    (gLastFrame + 1)) << endl;
        }
        if (theNewSyncPacket->_sceneCmd == SceneSyncMaster::SceneSyncPacket::RESET_CMD) {
            gWaterSimulation->reset();
            gWaterRepresentation->clearCracks();
            
            while (gEventList.size() > 0) {
                gEventList.erase(&gEventList[0]);
            }
        }
        bool showSurface = (theNewSyncPacket->_moduleIntParam & 1);
        gEnableSurfaceCracks = (theNewSyncPacket->_moduleIntParam & 2);
        bool resetWater = (theNewSyncPacket->_moduleIntParam & 4);
        bool freezeWater = (theNewSyncPacket->_moduleIntParam & 8);
        bool freeWater = (theNewSyncPacket->_moduleIntParam & 16);
        bool testWater = (theNewSyncPacket->_moduleIntParam & 32);
        bool specialDampWater = (theNewSyncPacket->_moduleIntParam & 64);
        
*/        
        // XXX dunno yet
        bool showSurface = false; //(theNewSyncPacket->_moduleIntParam & 1);
        gEnableSurfaceCracks = false; //(theNewSyncPacket->_moduleIntParam & 2);
        bool resetWater = false;//(theNewSyncPacket->_moduleIntParam & 4);
        bool freezeWater = false;//(theNewSyncPacket->_moduleIntParam & 8);
        bool freeWater = false;//(theNewSyncPacket->_moduleIntParam & 16);
        bool testWater = false;//(theNewSyncPacket->_moduleIntParam & 32);
        bool specialDampWater = false;//(theNewSyncPacket->_moduleIntParam & 64);
        
        if (testWater != gTestWater) {
            gTestWaterRunTime = 0.f;
            AC_DEBUG << "INFO: WaterAppModule::periodic() \n TEST-WATER(" 
                      << (testWater ? " ON " : " OFF ") << ")";
            gTestWater = testWater;
        }

        /*
        if (theNewSyncPacket->_moduleFloatParam >= 0.f) {
            gSurfaceOpacity = theNewSyncPacket->_moduleFloatParam;
            AC_DB2("INFO: WaterAppModule::periodic() remote is setting ice opacity to " 
                    << gSurfaceOpacity);
        }
        */
        
        if (gWaterRepresentation) {
            gWaterRepresentation->enableSurface(showSurface);
            gWaterRepresentation->setSurfaceOpacity(gSurfaceOpacity);
            AC_DEBUG << "INFO: WaterAppModule::periodic()";
            AC_DEBUG << "   showSurface: " << showSurface;
            AC_DEBUG << "   ice opacity: " << gSurfaceOpacity;
            AC_DEBUG << "   gEnableSurfaceCracks: " << gEnableSurfaceCracks;

        }
        if (gWaterSimulation) { 
            if (resetWater) {
                AC_DEBUG << "INFO: WaterAppModule::periodic() \n resetWater";
                gWaterSimulation->reset();
            }
            if (freezeWater) {
                AC_DEBUG << "INFO: WaterAppModule::periodic() \n resetDamping(freezing it)";
                gWaterSimulation->resetDamping(true);
            } else if (freeWater) {
                AC_DEBUG << ("INFO: WaterAppModule::periodic() \n resetDamping(normal)");
                gWaterSimulation->resetDamping(false);
            } else if (specialDampWater) {
                AC_DEBUG << ("INFO: WaterAppModule::periodic() \n resetDamping(special)");
                gWaterSimulation->setDefaultDampingCoefficient(gSpecialWaterDamping);
                gWaterSimulation->resetDamping(false);
                gWaterSimulation->setDefaultDampingCoefficient(gWaterDamping);
            }
        }
        //extractEvents(currentFrame, *theNewSyncPacket, gEventList);
        gWaterSimulation->sinoidSplash(100, 100, 5, 50); // XXX
        //handleEvents(currentFrame, gEventList);
        //updateObjects(currentFrame, *theNewSyncPacket);

        //gLastFrame = theNewSyncPacket->_header._packetNumber;
        gLastFrame += 1; // XXX

    }

#ifdef COMPUTE_IN_SEPARATE_THREAD  
    if (gUseSeparateThread) {
        //AC_DEBUG << "gComputeLock.lock()" << endl;
//        gComputeLock->lock();
    }
#endif    


    if (gRunSimulation) {
#ifdef COMPUTE_IN_SEPARATE_THREAD            
//        AC_DEBUG << "calling simulationMultiStepInThread()" << endl;
        if (gUseSeparateThread) {
            static bool firstTime = true;
            if (firstTime) {
                firstTime = false;
            } else {
                gComputeLock->post();
            }

            assert(gWaterRepresentation);
            gWaterRepresentation->preRender();

            gWaterSimulation->simulationMultiStepInThread(*gComputeLock, 
                    gTimeStep, gNumIntegrationsPerFrame, 5);
        } else {
            
            if (gNumIntegrationsPerFrame == 1) {
                gWaterSimulation->simulationStep(gTimeStep);
            } else {
                gWaterSimulation->simulationMultiStep(gTimeStep, gNumIntegrationsPerFrame, 5);
            }
            
            assert(gWaterRepresentation);
            gWaterRepresentation->preRender();
        }
#else
        
        if (gNumIntegrationsPerFrame == 1) {
            gWaterSimulation->simulationStep(gTimeStep);
        } else {
            gWaterSimulation->simulationMultiStep(gTimeStep, gNumIntegrationsPerFrame, 5);
        }
        assert(gWaterRepresentation);
        gWaterRepresentation->preRender();
#endif
            
    }
#ifdef TEST_WATER    
    if (gTestWater) {
        if (gTestWaterRunTime > 1.f) {
            gTestWaterRunTime = 0.f;
        } else {
            gTestWaterRunTime += gTestWaterSpeed;
        }
        
        updateTestWater(gTestWaterRunTime);
    }
#endif    
}


void
generateSplash(float theIntensity, int splashX, int splashY) {

    if (gWaterRepresentation && gWaterRepresentation->surfaceEnabled() &&
        gEnableSurfaceCracks) 
    {
        //AC_DEBUG << "numSegments=       " << 30 / gCrackSegmentSize << endl;
        //AC_DEBUG << "gCrackSegmentSize= " << gCrackSegmentSize << endl;
        //AC_DEBUG << "gCrackVariation=   " << gCrackVariation << endl;

        if (gWaterRepresentation) {

            float randomFactor = 1.; // random(.6, 1.);
            float intensity = theIntensity * randomFactor * gCrackIntensityFactor +
                SQR(theIntensity * randomFactor) * gCrackSqrIntensityFactor;

            //AC_DEBUG << "theIntensity=   " << theIntensity << endl;
            //AC_DEBUG << "intensity   =   " << intensity << endl;

            for (int i=0; i < gNumCracksPerSplash; i++) {
                static int frameNumber = 0;
                srand48(((frameNumber % 0xff) << 24) | 
                        ((frameNumber % 0xff) << 16) |
                        ((frameNumber % 0xff) << 8) |
                        (frameNumber % 0xff));

                frameNumber++;
                float directionX = random(-10, +10);
                float directionY = random(-10, +10);

                /*
                gWaterRepresentation->addCrack(intensity, Vec2(splashX, splashY),
                        Vec2(directionX, directionY),
                        gNumSegmentsPerCrack, 
                        (1. + theIntensity) * gCrackSegmentSize, 
                        gCrackVariation);
                */
            }
        }
    } 
    //create a  splash
    int magnitude = int(theIntensity * gSplashIntensityFactor);
    int radius = int(theIntensity * gSplashRadiusFactor);
    assert(gWaterSimulation);
    gWaterSimulation->sinoidSplash(splashX, splashY, magnitude, radius);
}

/////////////////////////////////////////////////////////////////////////////////////

class SimpleTimer {
public:
    SimpleTimer(int numFramesPerMeasure=20) :
        _numFramesPerMeasure(numFramesPerMeasure)
    {
        reset();
    }
    
    void    reset() {
        _startTime = asl::Time();
        _endTime = asl::Time();
        _frameCounter = 0;
    }
    
    void    tickFrame() {
        if (_frameCounter >= _numFramesPerMeasure) {

            _endTime = asl::Time();

            double startTime = _startTime;
            double endTime = _endTime;
            AC_DEBUG << "FPS= " << (1. / ((endTime - startTime) / _frameCounter)) << endl;

            reset();
        } else {
            _frameCounter++;
        }
    }

    double getTime() const {
        double startTime = _startTime;
        double endTime = _endTime;
        return (endTime - startTime);
    }
    
private:  

    asl::Time  _startTime;
    asl::Time  _endTime;
    int     _frameCounter;
    int     _numFramesPerMeasure;
};

SimpleTimer gSimpleTimer(50);


void
setWaterProjection() {
    
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

/*
    float   left = -((float)(gDisplayWidth)) / 2.f;
    float   right = ((float)gDisplayWidth) / 2.f-1.f;
    float   top = -((float)gDisplayHeight) / 2.f;
    float   bottom = ((float)gDisplayHeight) / 2.f;
*/    
#if 1
/*
    float   left = (-((float)(gDisplayWidth)) / 2.f);
    float   right = (((float)gDisplayWidth) / 2.f-1.f);
    float   top = (-((float)gDisplayHeight) / 2.f);
    float   bottom = (((float)gDisplayHeight) / 2.f);
*/    
    float   left = (-((float)(gDisplayWidth)) / 2.f) + gDisplayOffsetX;
    float   right = (((float)gDisplayWidth) / 2.f-1.f) + gDisplayOffsetX;
    float   top = (-((float)gDisplayHeight) / 2.f) + gDisplayOffsetY;
    float   bottom = (((float)gDisplayHeight) / 2.f) + gDisplayOffsetY;

#else
    
    float   left = (-((float)(gSimulationWidth)) / 2.f) - gDisplayOffsetX;
    float   right = (((float)gSimulationWidth) / 2.f-1.f) - gDisplayOffsetX;
    float   top = (-((float)gSimulationHeight) / 2.f) - gDisplayOffsetY;
    float   bottom = (((float)gSimulationHeight) / 2.f) - gDisplayOffsetY;
#endif

    //AC_DEBUG << "left= " << left << endl;
    //AC_DEBUG << "right= " << right << endl;
    //AC_DEBUG << "top= " << top << endl;
    //AC_DEBUG << "bottom= " << bottom << endl;
	glOrtho( left, right, top, bottom, -100, 10000);
}

void
setEdgeProjection() {
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( -0.5, 0.5, -.5, 0.5, -101., 101);
}

void 
display() {

    glClearColor(0, 0, 0, 0);
    
    //clear frame buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setWaterProjection();

	glMatrixMode( GL_MODELVIEW );

    glLoadIdentity();

    // camera transformation
    glTranslatef(transx, transy, transz);
    glScalef(scale,scale,scale);
    glRotatef(rotx, 1.,0.,0.);
    glRotatef(roty, 0.,1.,0.);
    glRotatef(rotz, 0.,0.,1.);

    glColor4f(1, 1, 1, 1);

    //glutWireTeapot(1.);

    assert(gWaterRepresentation);
    
    //AC_DEBUG << "gWaterRepresentation->render()" << endl;
    gWaterRepresentation->render();

    setEdgeProjection();
  
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
}

void 
displayPostEdgeBlend() {

    glPopAttrib();

    glDisable(GL_BLEND);
    glDepthFunc( GL_LEQUAL );
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);

    if (RenderApp::instance().getStatistic()) {
        gSimpleTimer.tickFrame();
    }
}



/////////////////////////////////////////////////////////////////////////////////////

class AGPBufferAllocator : public BufferAllocator {
public:
    AGPBufferAllocator() :
        _buffer(0),
        _isAGPMem(false)
    {
    }
    
    virtual ~AGPBufferAllocator() {
    }

    // old buffer size: BUFFER_SIZE*sizeof(GLfloat)
    virtual void *  allocateSingleBuffer(unsigned int numBytes) {
        assert(_buffer == 0);
        
        float megabytes = (numBytes/1000000.f);
        
#ifdef USE_AGP_MEMORY

        float priority = .5f;

        _buffer = (GLfloat *)glXAllocateMemoryNV(numBytes, 0, 0, priority);
        if(_buffer) {
            AC_DEBUG << "Allocated " << megabytes << " megabytes of fast AGP memory" << endl;
            _isAGPMem = true;
            memset(_buffer, 0, numBytes);
            return _buffer;
        }

        if(!_buffer) {
            _buffer = (GLfloat *)glXAllocateMemoryNV(numBytes, 0, 0, 1.f);
        }

        if(_buffer) {
            memset(_buffer, 0, numBytes);
            AC_DEBUG << "Allocated " << megabytes << " megabytes of fast AGP memory" << endl;
            _isAGPMem = true;
            return _buffer;
        }

        // fall back to normal system memory
        AC_DEBUG << "Failed to allocate " << megabytes << " megabytes of fast AGP memory" << endl;
#endif
        AC_DEBUG << "Allocated " << megabytes << " megabytes of normal heap mem" << endl;
        _buffer = (GLfloat *)malloc(numBytes);
        assert(_buffer);
        _isAGPMem = false;
        return _buffer;
    }
    
    virtual bool    freeSingleBuffer() {
        assert(_buffer);
        if (_buffer) {
            if (_isAGPMem) {
            } else {
                free(_buffer);
            }
            _buffer = 0;
        }
        return true;
    }

private:
    void *  _buffer;
    bool    _isAGPMem;
};

void
loadTexturesFromConfig(const dom::Node & theConfig, WaterRepresentation::TextureClass theClassID) {
    try {
        string myClassName = WaterRepresentation::TextureClassName(theClassID);
        int numFloorTextures = theConfig(myClassName).childNodesLength("file");
        for (int i = 0; i < numFloorTextures; ++i) {
            const dom::Node & myFileNode = theConfig(myClassName)("file",i); 
            string fileName = gDataDir + "/" + myFileNode["name"].nodeValue();
            short objectID = asl::as<short>(myFileNode["objectid"].nodeValue());
            bool loadOK = gWaterRepresentation->loadTexture(theClassID,objectID,fileName.c_str());
            assert(loadOK);
        }
    } catch (const asl::Exception & ex) {
        AC_DEBUG << "#ERROR: Water:: in loadTexturesFromConfig():" << endl;
        AC_DEBUG << ex.what() << " AT " << ex.where() << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }
    catch (...) {
        AC_DEBUG << "#ERROR: Water:: in loadTexturesFromConfig():" << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }
}

bool 
initScene(const dom::Node & theConfig) {
    return true;
}

void
initWindow() {
    AC_DEBUG << ("water_module::initWindow");

    gWaterSimulation = new WaterSimulation(gSimulationWidth, gSimulationHeight, gWaterDamping);
    gWaterSimulation->init();
    gWaterSimulation->resetDamping(gFreezeWater);
    
    gWaterRepresentation = new WaterRepresentation();
    gWaterRepresentation->init(gWaterSimulation, gDisplayWidth, gDisplayHeight,
                               gSimulationOffsetX, gSimulationOffsetY,
                               gSimulationWidth, gSimulationHeight,
                               gWindowWidth, gWindowHeight, 
                               gDisplayOffsetX, gDisplayOffsetY,
                               new AGPBufferAllocator);

    AC_DEBUG << ("water_module::initWindow(): loading textures");
    
    try {
        const dom::Node & theConfig(gConfig("renderslave-config"));

        if (theConfig) {
            loadTexturesFromConfig(theConfig,WaterRepresentation::floormaps);
            loadTexturesFromConfig(theConfig,WaterRepresentation::surfacemaps);
            loadTexturesFromConfig(theConfig,WaterRepresentation::puzzlemaps);

            int numCubeTextures = theConfig("cubemaps").childNodesLength("fileset");
            for (int i = 0; i < numCubeTextures; ++i) {
                string fileNames[6];

                const dom::Node & myFileSetNode = theConfig("cubemaps")("fileset",i); 
                short objectID = asl::as<short>(myFileSetNode["objectid"].nodeValue());

                for (int s=0; s<6; s++) {
                    string fileName = myFileSetNode(cubeSides[s])["name"].nodeValue();
                    if (!fileName.size()) {
                        AC_DEBUG << "name for cubemap side " << cubeSides[s] << " not found" << endl;
                        assert(0);
                    }
                    fileNames[s] = gDataDir + "/" + fileName;
                }
                bool loadOK = gWaterRepresentation->loadCubeMapTexture(
                        WaterRepresentation::cubemaps,objectID,fileNames);
                assert(loadOK);
            }

            if (theConfig("cracksettings")) {
                ::fromNode(theConfig("cracksettings")("cracksegmentsize")("int"), gCrackSegmentSize);
                ::fromNode(theConfig("cracksettings")("numsegmentspercrack")("int"), gNumSegmentsPerCrack);
                ::fromNode(theConfig("cracksettings")("crackvariation")("float"), gCrackVariation);
                ::fromNode(theConfig("cracksettings")("numcrackspersplash")("int"), gNumCracksPerSplash);
                ::fromNode(theConfig("cracksettings")("crackwidth")("float"), gCrackWidth);
                ::fromNode(theConfig("cracksettings")("innericethickness")("float"), gInnerIceThickness);
                ::fromNode(theConfig("cracksettings")("outericethickness")("float"), gOuterIceThickness);
                ::fromNode(theConfig("cracksettings")("splashintensityfactor")("float"), gSplashIntensityFactor);
                ::fromNode(theConfig("cracksettings")("splashradiusfactor")("float"), gSplashRadiusFactor);
                ::fromNode(theConfig("cracksettings")("crackintensityfactor")("float"), gCrackIntensityFactor);
                ::fromNode(theConfig("cracksettings")("cracksqrintensityfactor")("float"), gCrackSqrIntensityFactor);
                int tmpDebugCracks = gDrawCrackField ? 1 : 0;
                ::fromNode(theConfig("cracksettings")("debugcracks")("int"), tmpDebugCracks);
                ::fromNode(theConfig("cracksettings")("opacity")("float"), gSurfaceOpacity);
                ::fromNode(theConfig("cracksettings")("waterdamping")("float"), gWaterDamping);

#ifdef TEST_WATER  
                ::fromNode(theConfig("cracksettings")("specialwaterdamping")("float"), gSpecialWaterDamping);
                ::fromNode(theConfig("cracksettings")("waterspeed")("float"), gTestWaterSpeed);
                ::fromNode(theConfig("cracksettings")("numwaterdrops")("int"), gTestWaterNumDrops);
                ::fromNode(theConfig("cracksettings")("waterdropsize")("float"), gTestWaterDropSize);
                ::fromNode(theConfig("cracksettings")("waterdropintensity")("float"), gTestWaterDropIntensity);
#endif                
                /*
                AC_DEBUG << "cracksegmentsize= " << gCrackSegmentSize << endl;
                AC_DEBUG << "numsegmentspercrack= " << gNumSegmentsPerCrack << endl;
                AC_DEBUG << "crackvariation= " << gCrackVariation << endl;
                AC_DEBUG << "numcrackspersplash= " << gNumCracksPerSplash << endl;
                AC_DEBUG << "crackwidth= " << gCrackWidth << endl;
                AC_DEBUG << "innericethickness= " << gInnerIceThickness << endl;
                AC_DEBUG << "outericethickness= " << gOuterIceThickness << endl;
                AC_DEBUG << "splashintensityfactor= " << gSplashIntensityFactor << endl;
                AC_DEBUG << "splashradiusfactor= " << gSplashRadiusFactor << endl;
                AC_DEBUG << "crackintensityfactor= " << gCrackIntensityFactor << endl;
                AC_DEBUG << "cracksqrintensityfactor= " << gCrackSqrIntensityFactor << endl;
                */

                gDrawCrackField = (tmpDebugCracks != 0);
                gWaterRepresentation->setCrackWidth(gCrackWidth);
                gWaterRepresentation->setInnerIceThickness(gInnerIceThickness);
                gWaterRepresentation->setOuterIceThickness(gOuterIceThickness);
                gWaterRepresentation->drawCrackField(gDrawCrackField);
                gWaterRepresentation->setSurfaceOpacity(gSurfaceOpacity);

                gWaterSimulation->setDefaultDampingCoefficient(gWaterDamping);
                
                //gWaterRepresentation->enableSurface(true);
            }
        }
    } catch (const asl::Exception & ex) {
        AC_DEBUG << "#ERROR: Water:: in initWindow():" << endl;
        AC_DEBUG << ex.what() << " AT " << ex.where() << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }
    catch (...) {
        AC_DEBUG << "#ERROR: Water:: in initWindow():" << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }
    gWaterRepresentation->setDrawWireFrame(gDrawWireframe);
	gWaterRepresentation->activateTextureIndex(WaterRepresentation::floormaps,0);
	gWaterRepresentation->activateTextureIndex(WaterRepresentation::cubemaps,0);
	gWaterRepresentation->activateTextureIndex(WaterRepresentation::surfacemaps,0);
   
    
    if (gTestMarker) {
        createTestMarker();
    }
#ifdef TEST_WATER   
    if (gTestWater) {
        createTestWater();
    }
#endif      
    AC_DEBUG << ("water_module::initWindow finished");
}

void
deactivateWindow() {
    AC_DEBUG << ("water_module::deactivateWindow");

    if (gWaterRepresentation) {
        gWaterRepresentation->setDefaultGLState();
    }
}


void
activate() {
    AC_DEBUG << ("water_module::activate");

    if (gWaterSimulation) {
        gWaterSimulation->reset();
        gWaterSimulation->resetDamping();
    }
    if (gWaterRepresentation) {
        //gWaterRepresentation->clearCracks();
        gWaterRepresentation->setDefaultGLState();
    }
}

void 
timeSimulation(int width, int height, int numSteps, int numMultiSteps) {
    
    WaterSimulation * simulation = new WaterSimulation(width, height);
    simulation->init();
   
    double startTime = asl::Time();
    
    AC_DEBUG << "simulation size :" << endl;
    AC_DEBUG << "width= " << width << "x" << height << endl;
    AC_DEBUG << "simulation steps:" << numSteps * numMultiSteps << endl;
   
    for (int i=0; i< numSteps; i++) {
        simulation->simulationMultiStep(gTimeStep, numMultiSteps, 5);
    }

    double endTime = asl::Time();
    
    double simulationTime = endTime - startTime;
    
    int numFloats = width*height;
    int numBytes = numFloats * sizeof(float) * 3;
    const int operationsPerFloat = 9;
    
    float floatsPerSecond = (numSteps * numFloats * numMultiSteps) / simulationTime;
    float bytesPerSecond = (numSteps * numBytes * numMultiSteps) / simulationTime;
    float mbytesPerSecond = bytesPerSecond / (1024. * 1024.);
    
    float flopsPerSecond = (numSteps * width * height * operationsPerFloat * numMultiSteps) / 
                           simulationTime;
    float mflopsPerSecond = flopsPerSecond / (1024. * 1024.);
    
    AC_DEBUG << simulationTime << " seconds\n";
    AC_DEBUG << (double) (numSteps * numMultiSteps) / simulationTime << " steps/second\n";
    AC_DEBUG << numFloats << " floats accessed\n";
    AC_DEBUG << numBytes << " bytes accessed\n";
    AC_DEBUG << mbytesPerSecond << " MB/s\n";
    AC_DEBUG << mflopsPerSecond << " MFLOPS/s\n";

    AC_DEBUG << endl;
}

void 
timeSimulation() {
    AC_DEBUG << "Timing water simulation:" << endl;

    const int STEPS_PER_ITER = 5;
    
    timeSimulation(480, 240, 20, STEPS_PER_ITER);
    timeSimulation(240, 480, 20, STEPS_PER_ITER);
    timeSimulation(120, 960, 20, STEPS_PER_ITER);
    timeSimulation(960, 120, 20, STEPS_PER_ITER);
     
    for (int i = 32; i <= 512; i += 32) {
        timeSimulation(100, i+2, 32, STEPS_PER_ITER);
    }
    for (int i = 32; i <= 512; i += 32) {
        timeSimulation(i+2, 100, 32, STEPS_PER_ITER);
    }
    for (int i = 32; i <= 512; i += 32) {
        timeSimulation(i+2, i+2, 32, STEPS_PER_ITER);
    }
}


void
printParameters() {

    AC_DEBUG << "============================\n";
    AC_DEBUG << "gTimeStep = " << gTimeStep << endl;
    AC_DEBUG << "gDrawReflection is " << (int) gDrawReflection << endl;
    AC_DEBUG << "gDrawRefraction is " << (int) gDrawRefraction << endl;
    AC_DEBUG << "gDrawCaustics is " << (int)gDrawCaustics << endl;
    AC_DEBUG << "ReflectionAlphaBias = " << 
        gWaterRepresentation->getReflectionAlphaBias() << endl;
    AC_DEBUG << "ReflectionAlphaScale = " << 
        gWaterRepresentation->getReflectionAlphaScale() << endl;


    AC_DEBUG << "RefractionScale = " << 
        gWaterRepresentation->getRefractionScale() << endl;
    AC_DEBUG << "CausticBias= " << 
        gWaterRepresentation->getCausticBias() << endl;
    AC_DEBUG << "CausticScale= " << 
        gWaterRepresentation->getCausticScale() << endl;
    AC_DEBUG << "CausticSqrScale= " << 
        gWaterRepresentation->getCausticSqrScale() << endl;
    AC_DEBUG << "CausticNegativeScale= " << 
        gWaterRepresentation->getCausticNegativeScale() << endl;
    AC_DEBUG << "NumIntegrationsPerFrame= " << gNumIntegrationsPerFrame << endl;
    AC_DEBUG << "============================\n";

}

/////////////////////////////////////////////////////////////////////////////////////

bool
keyboard( unsigned char key, int x, int y ) {
    assert(gWaterSimulation);
    assert(gWaterRepresentation);
    
    switch( key )
    {
        case ' ':
            transx = transy = transz = 0;
            scale = scale = scale = 1.;
            rotx = roty = rotz = 0;;
            ox = oy = 0;
            glutPostRedisplay();
            break;
        case 't':
            timeSimulation();
            break;
        case 'w':
            gWaterRepresentation->setDrawWireFrame(!gWaterRepresentation->getDrawWireFrame());
            break;
        case '*':
            gTimeStep *=1.2;
            AC_DEBUG << "gTimeStep now " << gTimeStep << endl;
            break;
        case '+':
            gTimeStep /=1.2;
            AC_DEBUG << "gTimeStep now " << gTimeStep << endl;
            break;
        case '<': 
            {
                float   refractionScale = gWaterRepresentation->getRefractionScale();
                refractionScale /= 1.25f;
                if (refractionScale< 0.01f) {
                    refractionScale= 0.01f;
                }
                gWaterRepresentation->setRefractionScale(refractionScale);
                AC_DEBUG << "refractionScale now " << refractionScale << endl;
            }
            break;
        case '>': 
            {
                float   refractionScale = gWaterRepresentation->getRefractionScale();
                if (refractionScale< 0.01f) {
                    refractionScale= 0.01f;
                }
                refractionScale *= 1.25f;
                if (refractionScale> 10.f) {
                    refractionScale= 10.f;
                }
                gWaterRepresentation->setRefractionScale(refractionScale);
                AC_DEBUG << "refractionScale now " << refractionScale << endl;
            }
            break;
        case 'A':
            {
                float reflectionAlphaBias = gWaterRepresentation->getReflectionAlphaBias();

                if (reflectionAlphaBias < 0.01f) {
                    reflectionAlphaBias = 0.01f;
                }
                reflectionAlphaBias *= 1.25f;
                if (reflectionAlphaBias > 1.f) {
                    reflectionAlphaBias = 1.f;
                }
                gWaterRepresentation->setReflectionAlphaBias(reflectionAlphaBias);
                AC_DEBUG << "reflectionAlphaBias now " << reflectionAlphaBias << endl;
            }
            break;
        case 'a':
            {
                float reflectionAlphaBias = gWaterRepresentation->getReflectionAlphaBias();

                reflectionAlphaBias /= 1.25f;
                if (reflectionAlphaBias < 0.01f) {
                    reflectionAlphaBias = 0.f;
                }
                gWaterRepresentation->setReflectionAlphaBias(reflectionAlphaBias);
                AC_DEBUG << "reflectionAlphaBias now " << reflectionAlphaBias << endl;
            }
            break;
        case 'S':
            {
                float reflectionAlphaScale = gWaterRepresentation->getReflectionAlphaScale();
                if (reflectionAlphaScale < 0.01f) {
                    reflectionAlphaScale = 0.01f;
                }
                reflectionAlphaScale *= 1.25f;
                if (reflectionAlphaScale > 2.f) {
                    reflectionAlphaScale = 2.f;
                }
                gWaterRepresentation->setReflectionAlphaScale(reflectionAlphaScale);
                AC_DEBUG << "reflectionAlphaScale now " << reflectionAlphaScale << endl;
            }
            break;
        case 's':
            {
                float reflectionAlphaScale = gWaterRepresentation->getReflectionAlphaScale();
                reflectionAlphaScale /= 1.25f;
                if (reflectionAlphaScale < 0.01f) {
                    reflectionAlphaScale = 0.f;
                }
                gWaterRepresentation->setReflectionAlphaScale(reflectionAlphaScale);
                AC_DEBUG << "reflectionAlphaScale now " << reflectionAlphaScale << endl;
            }
            break;


        case 'm': 
            {
                float   causticBias = gWaterRepresentation->getCausticBias();
                causticBias /= 1.25f;
                if (causticBias< 0.01f) {
                    causticBias = 0.01f;
                }
                gWaterRepresentation->setCausticBias(causticBias);
                AC_DEBUG << "causticBias now " << causticBias<< endl;
            }
            break;
        case 'M': 
            {
                float   causticBias = gWaterRepresentation->getCausticBias();
                if (causticBias< 0.01f) {
                    causticBias= 0.01f;
                }
                causticBias*= 1.25f;
                if (causticBias> 5.f) {
                    causticBias= 5.f;
                }
                gWaterRepresentation->setCausticBias(causticBias);
                AC_DEBUG << "causticBias now " << causticBias << endl;
            }
            break;

        case ',': 
            {
                float   causticScale = gWaterRepresentation->getCausticScale();
                causticScale/= 1.25f;
                if (causticScale< 0.01f) {
                    causticScale= 0.01f;
                }
                gWaterRepresentation->setCausticScale(causticScale);
                AC_DEBUG << "causticScaleSqr now " << causticScale<< endl;
            }
            break;
        case ';': 
            {
                float   causticScale= gWaterRepresentation->getCausticScale();
                if (causticScale< 0.01f) {
                    causticScale= 0.01f;
                }
                causticScale*= 1.25f;
                if (causticScale> 10.f) {
                    causticScale= 10.f;
                }
                gWaterRepresentation->setCausticScale(causticScale);
                AC_DEBUG << "causticScale now " << causticScale<< endl;
            }
            break;

        case '.': 
            {
                float   causticSqrScale = gWaterRepresentation->getCausticSqrScale();
                causticSqrScale/= 1.25f;
                if (causticSqrScale< 0.01f) {
                    causticSqrScale= 0.01f;
                }
                gWaterRepresentation->setCausticSqrScale(causticSqrScale);
                AC_DEBUG << "causticSqrScale now " << causticSqrScale<< endl;
            }
            break;
        case ':': 
            {
                float   causticSqrScale= gWaterRepresentation->getCausticSqrScale();
                if (causticSqrScale< 0.01f) {
                    causticSqrScale= 0.01f;
                }
                causticSqrScale*= 1.25f;
                if (causticSqrScale> 10.f) {
                    causticSqrScale= 10.f;
                }
                gWaterRepresentation->setCausticSqrScale(causticSqrScale);
                AC_DEBUG << "causticSqrScale now " << causticSqrScale<< endl;
            }
            break;

        case '-': 
            {
                float   causticNegativeScale = gWaterRepresentation->getCausticNegativeScale();
                causticNegativeScale/= 1.25f;
                if (causticNegativeScale< 0.01f) {
                    causticNegativeScale= 0.01f;
                }
                gWaterRepresentation->setCausticNegativeScale(causticNegativeScale);
                AC_DEBUG << "causticNegativeScalenow " << causticNegativeScale<< endl;
            }
            break;
        case '_': 
            {
                float   causticNegativeScale= gWaterRepresentation->getCausticNegativeScale();
                if (causticNegativeScale< 0.01f) {
                    causticNegativeScale= 0.01f;
                }
                causticNegativeScale*= 1.25f;
                if (causticNegativeScale> 10.f) {
                    causticNegativeScale= 10.f;
                }
                gWaterRepresentation->setCausticNegativeScale(causticNegativeScale);
                AC_DEBUG << "causticNegativeScale now " << causticNegativeScale<< endl;
            }
            break;

        case '(':
            if (gNumIntegrationsPerFrame>1) {
                gNumIntegrationsPerFrame = gNumIntegrationsPerFrame - 1;
            }
            AC_DEBUG << "Num integrations per frame: " << gNumIntegrationsPerFrame << endl;
            break;
        case ')':
            if (gNumIntegrationsPerFrame<40) {
                gNumIntegrationsPerFrame = gNumIntegrationsPerFrame + 1;
            }
            AC_DEBUG << "Num integrations per frame: " << gNumIntegrationsPerFrame << endl;
            break;
        case 'i':
            gWaterSimulation->reset();
            gWaterSimulation->resetDamping();
            break;
        case 'f':
            gWaterSimulation->resetDamping(false);
            break;
        case 'F':
            gWaterSimulation->resetDamping(true);
            break;
        case 'I':
            AC_DEBUG << "Resetting simulation parameters!\n";
            gWaterRepresentation->resetParameters();
            break;
        case 'p':
            printParameters();
            break;
        case 'r':
            gDrawReflection = !gDrawReflection;
            gWaterRepresentation->setDrawReflections(gDrawReflection);
            AC_DEBUG << "gDrawReflection is " << (int) gDrawReflection << endl;
            break;
        case 'R':
            gDrawRefraction = !gDrawRefraction;
            gWaterRepresentation->setDrawRefractions(gDrawRefraction);
            AC_DEBUG << "gDrawRefraction is " << (int) gDrawRefraction << endl;
            break;
        case 'c':
            gDrawCaustics = !gDrawCaustics;
            gWaterRepresentation->setDrawCaustics(gDrawCaustics);
            AC_DEBUG << "gDrawCaustics is " << (int)gDrawCaustics << endl;
            break;
        case '#':
            gRunSimulation= !gRunSimulation;
            AC_DEBUG << "gRunSimulation is " << (int) gRunSimulation << endl;
            break;
        case 'o':
            gUseSeparateThread= !gUseSeparateThread;
            AC_DEBUG << "gUseSeparateThread is " << (int) gUseSeparateThread << endl;
            break;
        case 'C':
            gDrawCrackField = !gDrawCrackField;
            if (gWaterRepresentation) {
                gWaterRepresentation->drawCrackField(gDrawCrackField);
                gWaterRepresentation->enableSurface(true);
            }
            break;
        case '=':
            if (gWaterRepresentation) {
                //gWaterRepresentation->clearCracks();
                gWaterRepresentation->enableSurface(!gWaterRepresentation->surfaceEnabled());
            }
            break;
        default:
            if (key >= '0' && key <= '9') {
                //AC_DEBUG << "ox= " << ox << endl;
                //AC_DEBUG << "oy= " << oy << endl;
             
                gKeyIntensity = float(key - '0') / 9.;
                generateSplash(gKeyIntensity, gMousePositionX, gMousePositionY);
                
            } else {
                return false;
            }
            
    }
    return true;
}

bool
handleSpecialKeys( int key, int x, int y ) {
    assert(gWaterSimulation);
    assert(gWaterRepresentation);
    
    switch (key) {
        case GLUT_KEY_F1:
            gWaterRepresentation->activateOtherTexture(WaterRepresentation::floormaps,-1);
            break;
        case GLUT_KEY_F2:
            gWaterRepresentation->activateOtherTexture(WaterRepresentation::floormaps,+1);
            break;
            break;
        case GLUT_KEY_F3:
            gWaterRepresentation->activateOtherTexture(WaterRepresentation::cubemaps,-1);
            break;
        case GLUT_KEY_F4:
            gWaterRepresentation->activateOtherTexture(WaterRepresentation::cubemaps,+1);
            break;
    
        case GLUT_KEY_F5:
            gCrackSegmentSize --;
            gWaterRepresentation->enableSurface(true);
            break;
        case GLUT_KEY_F6:
            gCrackSegmentSize ++;
            gWaterRepresentation->enableSurface(true);
            break;
        case GLUT_KEY_F7:
            gCrackVariation -= 0.2;
            gWaterRepresentation->enableSurface(true);
            break;
        case GLUT_KEY_F8:
            gWaterRepresentation->enableSurface(true);
            gCrackVariation += 0.2;
            break;

        case GLUT_KEY_F9:
            gTestMarker = !gTestMarker;
            if (gTestMarker) {
                createTestMarker();
            } else {
                clearTestMarker();
            }
            break;
#ifdef TEST_WATER   
        case GLUT_KEY_F10:
            gTestWater = !gTestWater;
            if (gTestWater) {
                gTestWaterRunTime = 0.f;
                createTestWater();
            } else {
                clearTestWater();
            }
            break;
#endif
        default:
            return false;
    }
    return true; // handled the key
}


/////////////////////////////////////////////////////////////////////////////////////

void 
reshape( int w, int h ) {
    int   left = (-((gDisplayWidth)) / 2) + gDisplayOffsetX;
    int   right = ((gDisplayWidth) / 2-1) + gDisplayOffsetX;
    int   top = (-(gDisplayHeight) / 2)   + gDisplayOffsetY;
    int   bottom = ((gDisplayHeight) / 2) + gDisplayOffsetY;
    
    gScreenWidth = w;
    gScreenHeight = h;

	glViewport( 0, 0, w, h );
	//glViewport( left, top, bottom, right );

    setWaterProjection();
}

void
pan(int x, int y) {
  transx += (x - ox) / 500.;
  transy -= (y - oy) / 500.; 
  ox = x;
  oy = y;
  glutPostRedisplay();
}

void
zoom(int x, int y) {
  scale += (y - oy) / 500.;
   oy = y;
  glutPostRedisplay();
}


void
rotate(int x, int y) {
    rotx += x - ox;
    if (rotx > 360.)
        rotx -= 360.;
    else if (rotx < -360.)
        rotx += 360.;
    roty += y - oy;
    if (roty > 360.)
        roty -= 360.;
    else if (roty < -360.)
        roty += 360.;
    ox = x;
    oy = y;
    glutPostRedisplay();
}

void
motion(int x, int y) {
    if (mot == PAN) {
        pan(x, y);
    } else if (mot == ROT) {
        rotate(x, y);
    } else if (mot == ZOOM) {
        zoom(x, y);
    } else {
        ox = x;
        oy = y;
    }
}

void
mouse(int button, int state, int x, int y) {

    AC_DEBUG << "mouse button=" << button << "  state=" << state 
         << "  x=" << x << "  y=" << y << endl;
    if (state == GLUT_DOWN) {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                //mot = PAN;
                //motion(ox = x, oy = y);
                gMousePositionX = max(0, min(gDisplayWidth-1, 
                            (x * gDisplayWidth / gScreenWidth)- gDisplayOffsetX));
                gMousePositionY = max (0, min(gDisplayHeight-1,
                            ((gScreenHeight - y)  * gDisplayHeight / gScreenHeight)- gDisplayOffsetY));
                
                generateSplash(gKeyIntensity, gMousePositionX, gMousePositionY);
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
    } else {
        mot = 0;
    }
}



/////////////////////////////////////////////////////////////////////////////////////////


/*

UL 0/0
UM 1/0
UR 2/0

OL 0/1
OM 1/1
OR 2/1

UL:
--display-offset-x -120 --display-offset-y -60 --fullscreen --simulation-offset-x 0 --simulation-offset-y 0

UM:
--display-offset-x 0 --display-offset-y -60 --fullscreen --simulation-offset-x 160 --simulation-offset-y 0

UR:
--display-offset-x 120 --display-offset-y -60 --fullscreen --simulation-offset-x 320 --simulation-offset-y 0

OL:
--display-offset-x -120 --display-offset-y +60 --fullscreen --simulation-offset-x 0 --simulation-offset-y 120

OM:
--display-offset-x 0 --display-offset-y +60 --fullscreen --simulation-offset-x 160 --simulation-offset-y 120

OR:
--display-offset-x 120 --display-offset-y +60 --fullscreen --simulation-offset-x 320 --simulation-offset-y 120


*/

/////////////////////////////////////////////////////////////////////////////////////////


int 
main(asl::Arguments & ourArguments) {

#ifdef COMPUTE_IN_SEPARATE_THREAD
    //assert(asl::ThreadSemFactory::instance());
    gComputeLock = new asl::ThreadSemaphore(); //asl::ThreadSemFactory::instance()->newLock();
    assert(gComputeLock);
#endif

    try {
#ifdef COMPUTE_IN_SEPARATE_THREAD
        gUseSeparateThread = ourArguments.haveOption("--multithread");
#endif        

        AC_DEBUG << "\nWater Keyboard Control:" << endl;
        AC_DEBUG << " ESC = exit" << endl;
        AC_DEBUG << " t   = time the simulation algorithm performance" << endl;
        AC_DEBUG << " u   = toggle sync over UDP" << endl;
        AC_DEBUG << "\nSimulation Parameters:\n" << endl;
        AC_DEBUG << " #   = toggle simulation" << endl;
        AC_DEBUG << " i   = reset water simulation" << endl;
        AC_DEBUG << "\nGraphics Parameters:\n" << endl;
        AC_DEBUG << " w   = toggle wireframe" << endl;
        AC_DEBUG << " SPC = reset view\n" << endl;
        AC_DEBUG << " p   = print parameters" << endl;
        AC_DEBUG << " r   = toggle reflections" << endl;
        AC_DEBUG << " R   = toggle refractions" << endl;
        AC_DEBUG << " c   = toggle caustics" << endl;
        AC_DEBUG << " */+ = increment/decrement timestep" << endl;
        AC_DEBUG << " </> = decrement/decrement refraction scale" << endl;
        AC_DEBUG << " A/s = increment/decrement alpha bias" << endl;
        AC_DEBUG << " S/s = increment/decrement alpha scale" << endl;
        AC_DEBUG << " M/m = increment/decrement caustic bias" << endl;
        AC_DEBUG << " ,/; = increment/decrement caustic scale" << endl;
        AC_DEBUG << " :/. = increment/decrement caustic square scale" << endl;
        AC_DEBUG << " _/- = increment/decrement caustic negative scale" << endl;
        AC_DEBUG << " (/) = decrement/increment num iterations per frame" << endl;
        AC_DEBUG << " I   = reset water representation" << endl;
        AC_DEBUG << "\n\n";


        if (ourArguments.haveOption("--simulation-width")) {
            gSimulationWidth = asl::as<int>(ourArguments.getOptionArgument("--simulation-width"));
        }
        if (ourArguments.haveOption("--simulation-height")) {
            gSimulationHeight = asl::as<int>(ourArguments.getOptionArgument("--simulation-height"));
        }
        if (ourArguments.haveOption("--simulation-offset-x")) {
            gSimulationOffsetX = asl::as<int>(ourArguments.getOptionArgument("--simulation-offset-x"));
        }
        if (ourArguments.haveOption("--simulation-offset-y")) {
            gSimulationOffsetY = asl::as<int>(ourArguments.getOptionArgument("--simulation-offset-y"));
        }

        if (ourArguments.haveOption("--display-width")) {
            gDisplayWidth = asl::as<int>(ourArguments.getOptionArgument("--display-width"));
        }
        if (ourArguments.haveOption("--display-height")) {
            gDisplayHeight = asl::as<int>(ourArguments.getOptionArgument("--display-height"));
        }

        //gDisplayOffsetX = gDisplayWidth * (gSimulationOffsetX / gSimulationWidth) 
        
        if (ourArguments.haveOption("--display-offset-x")) {
            gDisplayOffsetX = asl::as<int>(ourArguments.getOptionArgument("--display-offset-x"));
        }
        if (ourArguments.haveOption("--display-offset-y")) {
            gDisplayOffsetY = asl::as<int>(ourArguments.getOptionArgument("--display-offset-y"));
        }
        if (ourArguments.haveOption("--num-integrations")) {
            gNumIntegrationsPerFrame = asl::as<int>(ourArguments.getOptionArgument("--num-integrations"));
        }
        if (ourArguments.haveOption("--timestep")) {
            gTimeStep = asl::as<double>(ourArguments.getOptionArgument("--timestep"));
        }
        if (ourArguments.haveOption("--data-dir")) {
            gDataDir = asl::as<string>(ourArguments.getOptionArgument("--data-dir"));
        }
        if (ourArguments.haveOption("--water-xml-config")) {
            string configFile = asl::as<string>(ourArguments.getOptionArgument("--water-xml-config"));
            string theDocumentFile = asl::readFile(configFile);
            if (theDocumentFile.length() == 0) {
                AC_DEBUG << "WaterAppModule: could not find xml file " << configFile << endl;
            }
            gConfig = dom::Document(theDocumentFile);
            cerr << gConfig;
            if (!gConfig) {
                AC_DEBUG << "WaterAppModule: could not parse xml file" << configFile  << endl;
            }
//AC_DEBUG << gConfig << endl;       
        } else {
            AC_DEBUG << "##WARNING: WaterAppModule: no xml configuration file sprecified" << endl;

        }
        gDrawWireframe = ourArguments.haveOption("--wireframe");
        gTestMarker = ourArguments.haveOption("--test-marker");
#ifdef TEST_WATER   
        gTestWater = ourArguments.haveOption("--test-water");
#endif
        gFreezeWater = ourArguments.haveOption("--freeze-water");
    }
    catch (const asl::Exception & ex) {
        AC_DEBUG << "Water::Internal error in parameter handling:" << endl;
        //AC_DEBUG << ex.what() << " AT " << ex.where << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }

    AC_DEBUG << "\nSimulation configuration:\n";
    AC_DEBUG << "==========================\n";
    AC_DEBUG << "simulation-width    (W+2) = " << gSimulationWidth << endl;
    AC_DEBUG << "simulation-height   (H+2) = " << gSimulationHeight << endl;
    AC_DEBUG << "simulation-offset-x ( >0) = " << gSimulationOffsetX << endl;
    if (gSimulationOffsetX <= 0) {
        gSimulationOffsetX = 1;
        AC_DEBUG << "simulation-offset-x CORRECTED TO = " << gSimulationOffsetX << endl;
    }
    
    AC_DEBUG << "simulation-offset-y ( >0) = " << gSimulationOffsetY << endl;
    if (gSimulationOffsetY <= 0) {
        gSimulationOffsetY = 1;
        AC_DEBUG << "simulation-offset-y CORRECTED TO = " << gSimulationOffsetY << endl;
    }
    AC_DEBUG << "display-width       = " << gDisplayWidth << endl;
    if ((gDisplayWidth <= 0) || (gDisplayWidth > gSimulationWidth)) {
        gDisplayWidth = gSimulationWidth-gSimulationOffsetX-1;
        AC_DEBUG << "display-width CORRECTED TO = " << gDisplayWidth << endl;
    }
    AC_DEBUG << "display-height      = " << gDisplayHeight << endl;
    if ((gDisplayHeight <= 0) || (gDisplayHeight > gSimulationHeight)) {
        gDisplayHeight = gSimulationHeight-gSimulationOffsetY-1;
        AC_DEBUG << "display-height CORRECTED TO = " << gDisplayHeight << endl;
    }

    assert(gSimulationWidth - (gSimulationOffsetX + gDisplayWidth) >= 1);
    assert(gSimulationHeight - (gSimulationOffsetY + gDisplayHeight) >= 1);
    
    AC_DEBUG << "display-offset-x    = " << gDisplayOffsetX << endl;
    AC_DEBUG << "display-offset-y    = " << gDisplayOffsetY << endl;
    AC_DEBUG << endl;
    
    try {
        //initialize the scene and objects
        if (gConfig) {
            if (!initScene(gConfig("renderslave-config"))) {
                AC_DEBUG << "Internal error in initScene(gConfig)" << endl;
                return -1;
            }
        } else if (! initScene(gConfig) ) {
            AC_DEBUG << "Internal error in initScene(NULL):" << endl;
            return -1;
        }
        return 0;
    }
    catch (const asl::Exception & ex) {
        AC_DEBUG << "Water:: Internal error in initScene():" << endl;
        AC_DEBUG << ex.what() << " AT " << ex.where() << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }
    catch (...) {
        AC_DEBUG << "Water:: unidentified internal error in initScene():" << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(1);
    }
    return -1;
}


}; // namespace water_module



namespace video {
    
WaterAppModule::WaterAppModule(RenderApp & theRenderApp) : 
    RenderAppModule(theRenderApp, "water")
{
    AC_DEBUG << ("WaterAppModule::WaterAppModule()");
}

WaterAppModule::~WaterAppModule() {
    AC_DEBUG << ("WaterAppModule::~WaterAppModule()");
}

bool
WaterAppModule::shutdown(bool isFinalShutdown) {
    return true;
} 


void
WaterAppModule::addAllowedOptions(asl::Arguments & theArguments) {
    AC_DEBUG << ("WaterAppModule::addAllowedOptions()");
//AC_DEBUG << "WaterAppModule::addAllowedOptions" << endl;    
    theArguments.addAllowedOptions(&water_module::ourOptions[0]);
//AC_DEBUG << "WaterAppModule::addAllowedOptions" << endl;    
}

bool
WaterAppModule::init(asl::Arguments & theArguments) { 
    AC_DEBUG << ("WaterAppModule::init()");
    return (water_module::main(theArguments) == 0);
}

void
WaterAppModule::renderPostSync() {
    water_module::display();
}
void
WaterAppModule::renderPostEdgeBlend() {
    water_module::displayPostEdgeBlend();
}

void
WaterAppModule::handleReshape(int w, int h) {
    water_module::reshape(w, h);
}

bool
WaterAppModule::handleKeyboard(unsigned char key, int x, int y) {
    return water_module::keyboard(key, x, y);
}

bool
WaterAppModule::handleSpecialKeys(int key, int x, int y) {
    return water_module::handleSpecialKeys(key, x, y);
}
void
WaterAppModule::handleMouse(int button, int state, int x, int y) {
    water_module::mouse(button, state, x, y);
}

void
WaterAppModule::handleMotion(int x, int y) {
    water_module::motion(x, y);
}

void
WaterAppModule::handlePeriodic(double theRunningTime, 
    SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, bool & theNeedsRedraw)
{
    water_module::periodic(theRunningTime, theNewSyncPacket, theNeedsRedraw);
}

void
WaterAppModule::handleResetCommand() {
    water_module::handleResetCommand();
}


// called during startup
void
WaterAppModule::initWindow() {
    AC_DEBUG << ("WaterAppModule::initWindow()");
    water_module::gScreenWidth = _renderApp.getScreenWidth();
    water_module::gScreenHeight = _renderApp.getScreenHeight();
    water_module::initWindow();
}

void
WaterAppModule::updateWindow() {
    AC_DEBUG << ("WaterAppModule::updateWindow()");
    //water_module::initWindow();
}

void
WaterAppModule::activateWindow() {
    AC_DEBUG << ("WaterAppModule::activateWindow()");
}

void
WaterAppModule::deactivateWindow() {
    AC_DEBUG << ("WaterAppModule::deactivateWindow()");
    water_module::deactivateWindow();
}


//  called when activated
bool
WaterAppModule::activate() {
    AC_DEBUG << ("WaterAppModule::activate()");
    water_module::activate();
    return true; 
}

//  called when deactivated
bool
WaterAppModule::deactivate() { 
    AC_DEBUG << ("WaterAppModule::deactivate()");
    return true; 
}




}; // namespace video

