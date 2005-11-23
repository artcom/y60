//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSGLResourceManager.h"
#include "JSApp.h"
#include <y60/GLResourceManager.h>
#include <asl/os_functions.h>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("GLResourceManager@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool 
loadShaderLibrary(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        std::string theShaderLibFile;
        convertFrom(cx,argv[0],theShaderLibFile);

        GLResourceManager::get().loadShaderLibrary(asl::expandEnvironment(theShaderLibFile));
        JSApp::getPackageManager()->add(asl::getDirectoryPart(theShaderLibFile));    
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION; 
}

JSFunctionSpec *
JSGLResourceManager::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSGLResourceManager::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name                    native          nargs
        {"loadShaderLibrary",     loadShaderLibrary,     1},
        {0}
    };
    return myFunctions;
}
 
JSPropertySpec *
JSGLResourceManager::Properties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSConstIntPropertySpec *
JSGLResourceManager::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}
JSPropertySpec *
JSGLResourceManager::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

// getproperty handling
JSBool
JSGLResourceManager::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSGLResourceManager::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSGLResourceManager::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSGLResourceManager::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSGLResourceManager::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Base::Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    // we never create a renderer with new, we only access existing renderers by
    // accessing the RenderWindow's renderer property.
    // so only create a dummy object here - it will be reassigned
    // in the JSWrapper::Construct method anyway
    // TODO: is there a better way to do this?
    OWNERPTR myNewPtr = OWNERPTR(0);
    JSGLResourceManager * myNewObject=new JSGLResourceManager(myNewPtr, 0);
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGLResourceManager::Constructor: new JSGLResourceManager failed");
    return JS_FALSE;
}

JSObject *
JSGLResourceManager::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, Functions(), 0, 0, StaticFunctions());
    DOC_CREATE(JSGLResourceManager);
    return myClass;
}
/*
bool convertFrom(JSContext *cx, jsval theValue, GLResourceManager *& theGLResourceManager) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSGLResourceManager::NATIVE >::Class()) {
                theGLResourceManager = &(*JSClassTraits<JSGLResourceManager::NATIVE>::getNativeOwner(cx,myArgument));
                return true;
            }
        }
    }
    return false;
}


bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<GLResourceManager> & theGLResourceManager) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSGLResourceManager::NATIVE >::Class()) {
                theGLResourceManager = JSClassTraits<JSGLResourceManager::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSGLResourceManager::OWNERPTR & theOwner) {
    JSObject * myReturnObject = JSGLResourceManager::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}
*/
}




