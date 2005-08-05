//===========================================================================
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
//    $RCSfile: JSNetPlugIn.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.6 $
//
// Description:
//
//=============================================================================

#include "JSSocket.h"
#include "JSTCPServer.h"

#include <asl/PlugInBase.h>
#include <y60/IScriptablePlugin.h>

namespace y60 {
	class JSNetPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
    	public:
    		JSNetPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

    		virtual void initClasses(JSContext * theContext,
    			JSObject *theGlobalObject) {
    			JSSocket::initClass(theContext, theGlobalObject);
    			JSTCPServer::initClass(theContext, theGlobalObject);
    		}

    		const char * ClassName() {
    		    static const char * myClassName = "Socket";
    		    return myClassName;
    		}
	};
}

extern "C"
EXPORT asl::PlugInBase * jsnet_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSNetPlugIn(myDLHandle);
}
