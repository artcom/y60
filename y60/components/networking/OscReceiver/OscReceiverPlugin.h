/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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
