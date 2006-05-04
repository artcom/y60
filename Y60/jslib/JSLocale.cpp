//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSLocale.h"
#include "Documentation.h"
#include <locale>
#include <iostream>

using namespace asl;
using namespace std;

namespace jslib {
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
    return JS_TRUE;
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
    return JS_TRUE;
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
            JS_ReportError(cx,"JSLocale::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
    return JS_TRUE;
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
        myNewObject = new JSLocale(myNewOwner, &(*myNewOwner));
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

        OWNERPTR myNewOwner = OWNERPTR(new NATIVE(myLocaleString.c_str()));        
        myNewObject = new JSLocale(myNewOwner, &(*myNewOwner));
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



