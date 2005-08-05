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
//   $RCSfile: IScriptablePlugin.cpp,v $
//   $Author: david $
//   $Revision: 1.2 $
//   $Date: 2004/11/08 18:03:19 $
//
//
//=============================================================================

#include "IScriptablePlugin.h"
#include "JSScriptablePlugin.h"

namespace jslib {
    void
    IScriptablePlugin::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
	    JSScriptablePlugin::initClass(theContext, theGlobalObject, ClassName());
    }
}

