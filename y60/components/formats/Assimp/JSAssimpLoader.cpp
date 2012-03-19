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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "JSAssimpLoader.h"

#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSNode.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;


const unsigned READ_BUFFER_SIZE = 20000;

#define DB(x) // x

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("toString"); DOC_END;
    // const AssimpLoader & myLoader = JSAssimpLoader::getJSWrapper(cx, obj).getNative();
    std::string myStatusString = "[AssimpLoader]"; 

    *rval = as_jsval(cx, myStatusString);
    return JS_TRUE;
}

static JSBool
readFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("read scene from a given filename"); DOC_END;
    
    if (argc == 0) {
        JS_ReportError(cx, "AssimpLoader::readFile: 1 or 2 arguments expected: filename, [postprocessing flags]. 0 given");    
        return JS_FALSE;
    }

    std::string myFilename;
    if (!convertFrom(cx, argv[0], myFilename)) {
        JS_ReportError(cx, "AssimpLoader::readFile: first argument is not a string/filename");
        return JS_FALSE;
    }
   
    unsigned int myPostProcessingFlags = 0;
    if (argc > 1 ) {
        if (!convertFrom(cx, argv[1], myPostProcessingFlags)) {
            JS_ReportError(cx, "AssimpLoader::readFile: first argument is not a Number");
            return JS_FALSE;
        }
                
    } else {
        AC_INFO << "no post processing flags specified. Using the following as default: Assimp.PRESET_TARGET_REALTIME_QUALITY";  
        myPostProcessingFlags = aiProcessPreset_TargetRealtime_Quality; 
    
    }

    AssimpLoader &myLoader = JSAssimpLoader::getJSWrapper(cx, obj).openNative();
    myLoader.readFile(myFilename, myPostProcessingFlags);

    return JS_TRUE;
}

static JSBool
setBlacklist(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("set a list of strings, containing nodes that will be ignored while generating y60 scene structure."); 
    DOC_PARAM("theBlacklist", "array of blacklisted node names", DOC_TYPE_NODE);
    DOC_END;

    ensureParamCount(argc, 1);

    std::map<std::string, bool> blacklist;
    if (!JSVAL_IS_OBJECT(argv[0])) {
        JS_ReportError(cx, "could not convert argument 1 to Javascript Object.");
        return JS_FALSE;
    }

    JSObject * myObject = JSVAL_TO_OBJECT(argv[0]);
    
    if (!JS_IsArrayObject(cx, myObject)) {
        JS_ReportError(cx, "argument 1 is no array.");
        return JS_FALSE;
    }

    jsuint arrayLength = 0;
    if (!JS_GetArrayLength(cx, myObject, &arrayLength)) {
        JS_ReportError(cx, "error getting array length.");
        return JS_FALSE;
    }
    
    for (unsigned int i=0; i<arrayLength; i++) {
        jsval myArrayElement;
        if (!JS_GetElement(cx, myObject, i, &myArrayElement)) {
            return JS_FALSE;
        }

        std::string myString;
        if (!convertFrom(cx, myArrayElement, myString)) {
            JS_ReportError(cx, "element #%d is not a string", i);
            return JS_FALSE;
        }
    
        blacklist[myString] = true;
    }

    AssimpLoader &myLoader = JSAssimpLoader::getJSWrapper(cx, obj).openNative();
    myLoader.setBlacklist(blacklist);
    
    return JS_TRUE; 
}

static JSBool
forceMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("set a material that is used to replace all materials in the imported node."); 
    DOC_PARAM("theMaterialNode", "the forced material or null if to reenable material generation", DOC_TYPE_NODE);
    DOC_END;
   
    dom::NodePtr myMaterial;
    if (argc == 0) {

        myMaterial = dom::NodePtr(0);
    } else {
    
        if (!convertFrom(cx, argv[0], myMaterial)) {
            JS_ReportError(cx, "AssimpLoader::forceMaterial: first argument is not a material node");
            return JS_FALSE;
        }
    }

    AssimpLoader &myLoader = JSAssimpLoader::getJSWrapper(cx, obj).openNative();
    myLoader.forceMaterial(myMaterial);
    
    return JS_TRUE; 
}

static JSBool
createScene(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("create y60 scene representation from currently loaded scene and append to the given scene node."); 
    DOC_PARAM("theSceneNode", "the current scene", DOC_TYPE_NODE);
    DOC_PARAM("theTransformNode", "the transform node to attach the model to.", DOC_TYPE_NODE);
    DOC_END;

    ensureParamCount(argc, 1);

    dom::NodePtr myTransform;
    if (!convertFrom(cx, argv[0], myTransform)) {
        JS_ReportError(cx, "AssimpLoader::createScene: first argument must be a transform node.");
        return JS_FALSE;
    }

    AssimpLoader &myLoader = JSAssimpLoader::getJSWrapper(cx, obj).openNative();
    myLoader.createScene(myTransform);
    
    return JS_TRUE; 
}

JSFunctionSpec *
JSAssimpLoader::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString", toString, 0},
        {"readFile", readFile, 1},
        {"setBlacklist", setBlacklist, 1},
        {"forceMaterial", forceMaterial, 1},
        {"createScene", createScene, 1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSAssimpLoader::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSAssimpLoader::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSAssimpLoader::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSConstIntPropertySpec *
JSAssimpLoader::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSAssimpLoader::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

// setproperty handling
JSBool
JSAssimpLoader::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

JSBool
JSAssimpLoader::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates Assimp Loader");
    DOC_PARAM("filename", "the filename to be loaded.", DOC_TYPE_NODE);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    ensureParamCount(argc, 1);

    y60::ScenePtr myScene;
    if (!convertFrom(cx, argv[0], myScene)) {
        JS_ReportError(cx, "AssimpLoader, first argument must be the scene.");
        return JS_FALSE;
    }


    JSAssimpLoader * myNewObject = 0;
    OWNERPTR myAssimp = OWNERPTR(new AssimpLoader(myScene));
    myNewObject = new JSAssimpLoader(myAssimp, myAssimp.get());

    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSAssimpLoader::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("AssimpLoader", JSAssimpLoader);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSAssimpLoader::OWNERPTR & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSAssimpLoader::NATIVE>::Class()) {
                theNativePtr = JSClassTraits<JSAssimpLoader::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSAssimpLoader::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSAssimpLoader::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}
