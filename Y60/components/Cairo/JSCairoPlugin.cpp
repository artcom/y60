//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <asl/PlugInBase.h>
#include <y60/IScriptablePlugin.h>

#include "JSCairoContext.h"
#include "JSCairoPattern.h"
#include "JSCairoSurface.h"

#include "JSCairoFontOptions.h"

namespace y60 {
    using namespace jslib;

    class JSCairoPlugin : public asl::PlugInBase, public jslib::IScriptablePlugin {
        public:
            JSCairoPlugin(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

            virtual void initClasses(JSContext * theContext, JSObject * theGlobalObject) {
                JSCairoContext::initClass(theContext, theGlobalObject);
                JSCairoPattern::initClass(theContext, theGlobalObject);
                JSCairoSurface::initClass(theContext, theGlobalObject);

                JSCairoFontOptions::initClass(theContext, theGlobalObject);
            }

            const char * ClassName() {
                static const char * myClassName = "Cairo";
                return myClassName;
            }
    };
}

extern "C"
EXPORT asl::PlugInBase * Cairo_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSCairoPlugin(myDLHandle);
}
