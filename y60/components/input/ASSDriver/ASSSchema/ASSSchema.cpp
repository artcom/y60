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

#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/input/asseventxsd.h>

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
            dom::NodePtr myEventSchema( new dom::Document( y60::ourasseventxsd ) );
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
