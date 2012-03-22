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
*/

#include "JSCSVImporter.h"

#include <y60/jsbase/JSNode.h>

using namespace y60;
using namespace std;
using namespace dom;

#define ASSERT(x) if (!(x)) {cerr << "Assertion failed: " << #x << " " << __FILE__ << " " <<__LINE__ << endl; exit(1);}


namespace jslib {

    static JSBool
    Csv2array(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        std::string myFileName;
        convertFrom(cx, argv[0], myFileName );
        if (argc > 1) {
            std::string myDelimiter;
            convertFrom(cx, argv[1], myDelimiter);
            *rval = as_jsval(cx, csv::CSVImporter::csv2array(myFileName, myDelimiter[0]));
        } else {
            *rval = as_jsval(cx, csv::CSVImporter::csv2array(myFileName));
        }
        return JS_TRUE;
    }

    JSObject *
    csv::JSImporter::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(),  ConstIntProperties(), StaticProperties(), StaticFunctions());
        DOC_MODULE_CREATE("CSVImporter", JSImporter);
        return myClass;
    }

    JSBool
    csv::JSImporter::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        try {
            if (JSA_GetClass(cx,obj) != Class()) {
                JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
                return JS_FALSE;
            }

            OWNERPTR myNewNative;
            myNewNative = OWNERPTR(new csv::CSVImporter());

            csv::JSImporter * myNewObject = new csv::JSImporter(myNewNative, myNewNative.get());
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSImporter::Constructor: bad parameters");
                return JS_FALSE;
            }
        } HANDLE_CPP_EXCEPTION;
    }

    JSConstIntPropertySpec *
    csv::JSImporter::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    csv::JSImporter::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    csv::JSImporter::Properties() {
        static JSPropertySpec myProperties[] = {
            {0}
        };
        return myProperties;
    }
    
    JSBool
    csv::JSImporter::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSImporter::getProperty: index %d out of range", theID);
        return JS_FALSE;
    }

    JSBool
    csv::JSImporter::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSImporter::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    }

    JSFunctionSpec *
    csv::JSImporter::Functions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    JSFunctionSpec *
    csv::JSImporter::StaticFunctions() {
        AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
        static JSFunctionSpec myFunctions[] = {
            // name                  native                   nargs
            {"csv2array",            Csv2array,               2},
            {0}
        };
        return myFunctions;
    }


    bool convertFrom(JSContext *cx, jsval theValue, csv::JSImporter::OWNERPTR & theCSVImporter) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<csv::JSImporter::NATIVE>::Class()) {
                    theCSVImporter = JSClassTraits<csv::JSImporter::NATIVE>::getNativeOwner(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }

    jsval as_jsval(JSContext *cx, csv::JSImporter::OWNERPTR theOwner) {
        JSObject * myReturnObject = csv::JSImporter::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, csv::JSImporter::OWNERPTR theOwner, csv::JSImporter::NATIVE * theNative) {
        JSObject * myObject = csv::JSImporter::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }


} // namespace jslib

