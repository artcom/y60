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

// own header
#include "JSLocale.h"

#include <y60/jsbase/Documentation.h>
#include <y60/jsbase/JSWrapper.impl>
#include <locale>
#include <iostream>

using namespace asl;
using namespace std;

namespace jslib {

template class JSWrapper<std::locale, asl::Ptr<std::locale>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("current locale in string representation.");
    DOC_END;
    std::string myStringRep = JSLocale::getJSWrapper(cx,obj).getNative().name();
*rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
parseFloat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("string-to-float conversion");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        const std::locale & myLocale = JSLocale::getJSWrapper(cx,obj).getNative();
        ensureParamCount(argc, 1);
        string myString;
        convertFrom(cx, argv[0], myString);
        istringstream myStream;
        myStream.imbue(myLocale);
        myStream.str(myString);
        float myFloat;
        myStream >> myFloat;
        if (myStream.fail()) {
            *rval = JS_GetNaNValue(cx);
        } else {
            *rval = as_jsval(cx, myFloat);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
formatFloat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("string-to-float conversion");
    DOC_PARAM("theMessage", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        const std::locale & myLocale = JSLocale::getJSWrapper(cx,obj).getNative();
        ensureParamCount(argc, 1);
        float myFloat;
        convertFrom(cx, argv[0], myFloat);
        ostringstream myStream;
        myStream.imbue(myLocale);
        myStream << myFloat;
        *rval = as_jsval(cx, myStream.str());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSLocale::StaticFunctions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                    native          nargs
        {0}
    };
    return myFunctions;
}
JSFunctionSpec *
JSLocale::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"toString",               toString,       0},
        {"parseFloat",             parseFloat,     1},
        {"formatFloat",            formatFloat,    1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSLocale::Properties() {
    static JSPropertySpec myProperties[] = {
       {0}
    };
    return myProperties;
}

enum PropertyNumbers {PROP_verbosity};

JSConstIntPropertySpec *
JSLocale::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

#ifdef WITH_UNUSED_FUNCTIONS
static JSBool
getStaticProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    int myID = JSVAL_TO_INT(id);
    switch (myID) {
    case 0:
    default:
        JS_ReportError(cx,"JSLocale::getStaticProperty: index %d out of range", myID);
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
setStaticProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    int myID = JSVAL_TO_INT(id);
    switch (myID) {
    case 0:
    default:
        JS_ReportError(cx,"JSLocale::getStaticProperty: index %d out of range", myID);
        return JS_FALSE;
    }
    return JS_TRUE;
}
#endif

JSPropertySpec *
JSLocale::StaticProperties() {
    static JSPropertySpec myProperties[] = {
    //     {"verbosity", PROP_verbosity, JSPROP_ENUMERATE|JSPROP_PERMANENT, getStaticProperty, setStaticProperty},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSLocale::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            ;
    }
    JS_ReportError(cx,"JSLocale::getProperty: index %d out of range", theID);
    return JS_FALSE;
};

// setproperty handling
JSBool
JSLocale::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSLocale::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
};

JSBool
JSLocale::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new Locale object");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    JSLocale * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewOwner = OWNERPTR(new NATIVE());
        myNewObject = new JSLocale(myNewOwner, myNewOwner.get());
    } else if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSLocale::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        string myLocaleString = "";
        if (!convertFrom(cx, argv[0], myLocaleString)) {
            JS_ReportError(cx, "JSLocale::Constructor: argument #1 must be a string (locale)");
            return JS_FALSE;
        }

        try {
            OWNERPTR myNewOwner = OWNERPTR(new NATIVE(myLocaleString.c_str()));
            myNewObject = new JSLocale(myNewOwner, myNewOwner.get());
        } catch (const exception & ex) {
            AC_ERROR << "Error while creating locale for `" << myLocaleString << "`. Using `C` as default. Exception thrown: " << ex.what();
            OWNERPTR myNewOwner = OWNERPTR(new NATIVE());
            myNewObject = new JSLocale(myNewOwner, myNewOwner.get());
        }

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (locale) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSLocale::Constructor: bad parameters");
    return JS_FALSE;
}


JSObject *
JSLocale::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
        Properties(), Functions(), ConstIntProperties(), StaticProperties(), StaticFunctions());
    DOC_CREATE(JSLocale);
    return myClass;
}
} // namespace



