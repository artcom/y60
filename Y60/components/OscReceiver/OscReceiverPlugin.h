//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef OSC_RECEIVER_PLUGIN_INCLUDED
#define OSC_RECEIVER_PLUGIN_INCLUDED

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include "JSOscReceiver.h"

namespace y60 {
	class OscReceiverPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
    	public:
    		OscReceiverPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

    		virtual void initClasses(JSContext * theContext,
    			JSObject *theGlobalObject) 
            {
                jslib::JSOscReceiver::initClass(theContext, theGlobalObject);
    		}

    		const char * ClassName() {
    		    static const char * myClassName = "OscReceiver";
    		    return myClassName;
    		}
	};
}

#endif
