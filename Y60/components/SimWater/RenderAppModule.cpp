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


#include "RenderAppModule.h"
#include <GL/glx.h>
#include <GL/glut.h>

#include <asl/Time.h>


using namespace std;  // automatically added!
using namespace video;

namespace video {

BlankAppModule::BlankAppModule(RenderApp & theRenderApp) : 
        RenderAppModule(theRenderApp, string("blank"))
{
}

void
BlankAppModule::renderPostSync() {

    GLfloat r, g, b, a;

    r = static_cast<GLfloat>((_myColorABGR&0x000000ff))/255.f;
    g = static_cast<GLfloat>((_myColorABGR&0x0000ff00)>>8)/255.f;
    b = static_cast<GLfloat>((_myColorABGR&0x00ff0000)>>16)/255.f;
    a = static_cast<GLfloat>((_myColorABGR&0xff000000)>>24) /255.f;
    
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}


void
BlankAppModule::handlePeriodic(double theRunningTime, 
        SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, 
        bool & theNeedsRedraw)
{
    static double previousTime = 0;

    if ((((double)asl::Time()) - previousTime) > (1./30.)) {
        theNeedsRedraw = true;
        previousTime = asl::Time();
    }

    if (theNewSyncPacket) {
        //_myColorABGR = static_cast<unsigned int>(theNewSyncPacket->_moduleIntParam);

    } else {
        _myColorABGR = 0x00000000;
    }
      
}

}; // namespace video

