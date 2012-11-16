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
#include "JSGLResourceManager.h"

#include "JSApp.h"
#include <y60/jsbase/JSWrapper.impl>
#include <y60/glrender/GLResourceManager.h>
#include <asl/base/os_functions.h>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

template class JSWrapper<GLResourceManager, asl::Ptr<GLResourceManager>,
         StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the object");
    DOC_RVAL("The string", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = std::string("GLResourceManager@") + as_string(obj);
*rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
loadShaderLibrary(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Loads a given shader library.");
    DOC_PARAM("theFileName", "Filename of the shaderlibrary to load", DOC_TYPE_STRING);
    DOC_PARAM("theVertexProfileName", "(optional) name of Cg vertex shader profile", DOC_TYPE_STRING);
    DOC_PARAM("theFragmentProfileName", "(optional) name of Cg fragment shader profile", DOC_TYPE_STRING);
    DOC_END;
    try {
        std::string myShaderLibFile;
        std::string myVertexProfileName;
        std::string myFragmentProfileName;

        switch(argc) {
            case 3:
                convertFrom(cx,argv[2],myFragmentProfileName);
                // fall-through
            case 2:
                convertFrom(cx,argv[1],myVertexProfileName);
                // fall-through
            case 1:
                convertFrom(cx,argv[0],myShaderLibFile);
                break;
        }

        if (argc == 0) {
            GLResourceManager::get().loadShaderLibrary();
        } else {
            GLResourceManager::get().loadShaderLibrary(
                        asl::expandEnvironment(myShaderLibFile), myVertexProfileName, myFragmentProfileName);
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
prepareShaderLibrary(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets name and optional parameters for a given shader library; loading will take place automatically after window creation.");
    DOC_PARAM("theFileName", "Filename of the shaderlibrary to load", DOC_TYPE_STRING);
    DOC_PARAM("theVertexProfileName", "(optional) name of Cg vertex shader profile", DOC_TYPE_STRING);
    DOC_PARAM("theFragmentProfileName", "(optional) name of Cg fragment shader profile", DOC_TYPE_STRING);
    DOC_END;
    try {
        std::string myShaderLibFile;
        std::string myVertexProfileName;
        std::string myFragmentProfileName;

        switch(argc) {
            case 3:
                convertFrom(cx,argv[1],myVertexProfileName);
                convertFrom(cx,argv[2],myFragmentProfileName);
            case 1:
                convertFrom(cx,argv[0],myShaderLibFile);
            break;
            default:
                JS_ReportError(cx,"JSGLResourceManager::loadShaderLibrary: number of arguments is %d, must be 1 oder 3", argc);
                return JS_FALSE;
        }

        GLResourceManager::get().prepareShaderLibrary(
                    asl::expandEnvironment(myShaderLibFile), myVertexProfileName, myFragmentProfileName);
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
        // name                   native               nargs
        {"loadShaderLibrary",     loadShaderLibrary,    3},
        {"prepareShaderLibrary",  prepareShaderLibrary, 3},
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
    DOC_BEGIN("Creates a new resource manager");
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
    OWNERPTR myNewPtr = OWNERPTR();
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
                theGLResourceManager = JSClassTraits<JSGLResourceManager::NATIVE>::getNativeOwner(cx,myArgument).get();
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
    JSObject * myReturnObject = JSGLResourceManager::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}
*/
}




