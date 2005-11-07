//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: SDLApp.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/02/25 14:58:30 $
//
//
//=============================================================================

#ifndef __Y60_SDLAPP_INCLUDED__
#define __Y60_SDLAPP_INCLUDED__

#include <y60/JSApp.h>

class SDLApp : public jslib::JSApp {
    public:
        SDLApp();
        static bool _myGenerateTutorialMode;
    protected:
        virtual bool initClasses(JSContext * theContext, JSObject * theGlobalObject);
};
#endif

