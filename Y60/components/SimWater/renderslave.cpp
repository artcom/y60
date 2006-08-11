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

#include "RenderAppModule.h"
#include "RenderApp.h"
#include "WaterAppModule.h"

using namespace std;  // automatically added!

int main( int argc, char *argv[] ) {

    try {
        asl::Arguments arguments;

        arguments.parse(argc, argv );

        video::RenderApp * renderApp = new video::RenderApp();
        
        video::IRenderAppModule * blankModule = new video::BlankAppModule(*renderApp);
        renderApp->registerModule(*blankModule);
        
        for (int i=0; i< arguments.getCount(); i++) {
            string arg = arguments.getArgument(i);

            if (arg == "water") {
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
        AC_PRINT << "#ERROR: renderslave caught exception" << endl;
        AC_PRINT << ex.what() << " AT " << ex.where() << endl;
        AC_PRINT << "Aborting" << endl;
        exit(-4);
    }
    catch (...) {
        AC_PRINT << "#ERROR: renderslave caught unknown exception" << endl;
        AC_PRINT << "Aborting" << endl;
        exit(-5);
    }
    
    return 0;
}

