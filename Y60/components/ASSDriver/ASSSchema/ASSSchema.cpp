//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include <y60/JSNode.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSWrapper.h>
#include <y60/AssEventSchema.h>

namespace y60 {

typedef bool DummyT;
class ASSSchemaPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
	public:
        typedef DummyT NATIVE;
        typedef asl::Ptr<DummyT> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR,jslib::StaticAccessProtocol> Base;
	
		ASSSchemaPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

        virtual JSFunctionSpec * StaticFunctions();
		const char * ClassName() {
		    static const char * myClassName = "ASSSchema";
		    return myClassName;
		}
		
};
    JS_STATIC_DLL_CALLBACK(JSBool)
    Schema(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            dom::NodePtr myEventSchema( new dom::Document( getASSSchema() ) );
            *rval = jslib::as_jsval(cx, myEventSchema);
            return JS_TRUE;    
        } HANDLE_CPP_EXCEPTION;
    }

    JSFunctionSpec * ASSSchemaPlugIn::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {
            // name                         native                       nargs    
            {"schema",             Schema,             0},
            {0}
        };
        return myFunctions;
    }    
} // end of namespace y60

extern "C"
EXPORT asl::PlugInBase * ASSSchema_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::ASSSchemaPlugIn(myDLHandle);
}
