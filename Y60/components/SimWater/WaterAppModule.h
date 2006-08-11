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

#ifndef AC_Y60_WATER_APP_MODULE_INCLUDED
#define AC_Y60_WATER_APP_MODULE_INCLUDED

#include "RenderAppModule.h"


#include <string.h>

namespace video {

class WaterAppModule : public RenderAppModule {
public:

    WaterAppModule(RenderApp & theRenderApp);
    virtual ~WaterAppModule();

    virtual bool    shutdown(bool isFinalShutdown);

    virtual void    addAllowedOptions(asl::Arguments & theArguments);
    virtual bool    init(asl::Arguments & theArguments);

    virtual void    renderPostSync();
    virtual void    renderPostEdgeBlend();
    virtual void    handleReshape(int w, int h);
    virtual bool    handleKeyboard(unsigned char key, int x, int y);
    virtual bool    handleSpecialKeys(int key, int x, int y);
    virtual void    handleMouse(int button, int state, int x, int y);
    virtual void    handleMotion(int x, int y);
    virtual void    handlePeriodic(double theRunningTime, 
            SceneSyncMaster::SceneSyncPacket * theNewSyncPacket, bool & theNeedsRedraw);

    virtual void    handleResetCommand();

    // called during startup
    virtual void    initWindow();
    virtual void    updateWindow();
    virtual void    activateWindow();
    virtual void    deactivateWindow();
    //  called when activated
    virtual bool    activate();
    //  called when deactivated
    virtual bool    deactivate();

protected:
};



}; // namespace video

#endif // AC_Y60_WATER_APP_MODULE_INCLUDED
