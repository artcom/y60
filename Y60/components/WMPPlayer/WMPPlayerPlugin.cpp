//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: CTScanPlugIn.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================


#include "JSWMPPlayer.h"

#include <y60/IScriptablePlugin.h>
#include <asl/PlugInBase.h>

namespace y60 {
	class WMPPlayerPlugIn :
		public asl::PlugInBase,
        public jslib::IScriptablePlugin
	{
	    public:
		    WMPPlayerPlugIn (asl::DLHandle theDLHandle);
            ~WMPPlayerPlugIn();

   		    void initClasses(JSContext * theContext, JSObject *theGlobalObject);
            
            const char * ClassName() {
                static const char * myClassName = "WMPPlayerPlugIn";
                return myClassName;
            }
	};
}

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

WMPPlayerPlugIn::WMPPlayerPlugIn(DLHandle theDLHandle) :
			PlugInBase(theDLHandle)
{
    CoInitialize(0);
}

WMPPlayerPlugIn::~WMPPlayerPlugIn() {
    CoUninitialize();
}

void
WMPPlayerPlugIn::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
    jslib::JSWMPPlayer::initClass(theContext, theGlobalObject);
}



extern "C"
EXPORT PlugInBase* y60WMPPlayer_instantiatePlugIn(DLHandle myDLHandle) {
	return new y60::WMPPlayerPlugIn(myDLHandle);
}
