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
// $RCSfile: renderslave.cpp,v $
// $Revision: 1.2 $
// $Date: 2002/09/06 18:17:30 $
// $Author: valentin $
//
// $Log $
//
// ----------------------------------------------------------------------------

#define DEBUG_LEVEL 1
#define DEBUG_TIME 

#include "RenderAppModule.h"
#include "RenderApp.h"
//#include "VideoAppModule.h"
#include "WaterAppModule.h"

//#include <asl/Debug.h>


using namespace std;  // automatically added!



int main( int argc, char *argv[] ) {

    try {
        asl::Arguments arguments;

        arguments.parse(argc, argv /*, asl::Arguments::IgnoreErrors*/);

        video::RenderApp * renderApp = new video::RenderApp();
        
        video::IRenderAppModule * blankModule = new video::BlankAppModule(*renderApp);
        renderApp->registerModule(*blankModule);
        
        for (int i=0; i< arguments.getCount(); i++) {
            string arg = arguments.getArgument(i);

            if (arg == "video") {
                //video::IRenderAppModule * videoModule = new video::VideoAppModule(*renderApp);
                //renderApp->registerModule(*videoModule);
            } else if (arg == "water") {
                video::IRenderAppModule * waterModule = new video::WaterAppModule(*renderApp);
                renderApp->registerModule(*waterModule);
            }
        }
        
        if (!renderApp->preInit()) {
            AC_DEBUG << "#ERROR : RenderApp::preInit() failed. Aborting";
            exit(-2);
        }

        if (!renderApp->handleArguments(argc, argv)) {
            AC_DEBUG << "#ERROR : RenderApp::handleArguments() failed. Aborting";
            delete renderApp;
            renderApp = 0;
            return -1;
        }

        if (!renderApp->init()) {
            AC_DEBUG << "#ERROR : RenderApp::init() failed. Aborting";
            exit(-2);
        }

        try {
            renderApp->startMainLoop();
        }
        catch (...) {
            AC_DEBUG << "#ERROR : Unexpected expection thrown in startMainLoop()" << endl;
            exit(-3);
        }

        // will never return here...
    } catch (const asl::Exception & ex) {
        AC_DEBUG << "#ERROR: renderslave caught exception" << endl;
        AC_DEBUG << ex.what() << " AT " << ex.where() << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(-4);
    }
    catch (...) {
        AC_DEBUG << "#ERROR: renderslave caught unknown exception" << endl;
        AC_DEBUG << "Aborting" << endl;
        exit(-5);
    }
    
    return 0;
}

