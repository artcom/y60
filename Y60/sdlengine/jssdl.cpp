//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: jssdl.cpp,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2004/11/07 04:50:20 $
//
//
//=============================================================================

#include "JSRenderWindow.h"

using namespace jslib;

bool initSdlClasses(JSContext *cx, JSObject *theGlobalObject) {

    if (!JSRenderWindow::initClass(cx, theGlobalObject)) {
        return false;
    }

    return true;
}


