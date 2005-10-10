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
//    $RCSfile: ProcFunctionsPlugIn.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.0 $
//
// Description:
//
//=============================================================================


#include <y60/IScriptablePlugin.h>
#include <asl/PlugInBase.h>
#include "JSproc_functions.h"

namespace y60 {
    class ProcFunctionsPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
        public:
            ProcFunctionsPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

            virtual void initClasses(JSContext * theContext,
                    JSObject *theGlobalObject) {
                JS_DefineFunctions(theContext, theGlobalObject, jslib::JSProcFunctions::Functions());
            }

            const char * ClassName() {
                static const char * myClassName = "ProcFunctions";
                return myClassName;
            }
    };
}

extern "C"
EXPORT asl::PlugInBase * ProcFunctions_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::ProcFunctionsPlugIn(myDLHandle);
}


