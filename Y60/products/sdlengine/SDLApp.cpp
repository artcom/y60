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
//   $RCSfile: SDLApp.cpp,v $
//   $Author: martin $
//   $Revision: 1.12 $
//   $Date: 2004/11/07 04:50:20 $
//
//
//=============================================================================

#include "SDLApp.h"
#include "jssdl.h"

#define DB(x) // x

using namespace std;
using namespace asl;
using namespace jslib;

SDLApp::SDLApp() 
{
}

bool 
SDLApp::initClasses(JSContext * theContext, JSObject * theGlobalObject) {
    if (!JSApp::initClasses(theContext, theGlobalObject)) {
        return false;
    }
    if (!initSdlClasses(theContext, theGlobalObject))
        return false;
    return true;
}

