//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSParticleSystem.h"

#include <asl/os_functions.h>
#include <y60/JSBlock.h>
#include <dom/Nodes.h>
#include <y60/JSNode.h>
#include <y60/JSScene.h>
#include <y60/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns information about the ParticleSystem."); DOC_END;
    const ParticleSystem & myParticleSystem = JSParticleSystem::getJSWrapper(cx,obj).getNative();

    std::string myStatusString = "";
    myStatusString = std::string("\nStatus: is emitting..."); 
    
    *rval = as_jsval(cx, myStatusString);
    return JS_TRUE;
}

static JSBool
create(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Create a new Particle System Controller.");
    DOC_PARAM("theParentNode",       "a node from the scene dom to attach the particle system to", DOC_TYPE_NODE);
    DOC_PARAM("theParticleCount",    "The number of Particles that will be emitted [optional-default=200]", DOC_TYPE_INTEGER);
    DOC_PARAM("theTextureName",      "text[optional-default=1.png]", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theInitialDirection", "emitter base direcition", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theScattering",       "scattering angle around the initial direction", DOC_TYPE_FLOAT);
    DOC_PARAM("theSpeedRange",       "emitting speed range", DOC_TYPE_VECTOR2F);
    DOC_RVAL("true", DOC_TYPE_STRING);
    DOC_END;

    // generate default values
    dom::NodePtr  myParentNode;
    unsigned myParticleCount = 1000;
    string   myTextureName = "1.png";
    Vector3f myInitialDirection = Vector3f(0.0f, 1.0f, 0.0f);
    Vector2f myScattering = Vector2f(0.0f, 45.0f);
    Vector2f mySpeedRange = Vector2f(10.0, 20.0);
    
    try {
        if (argc > 6) {
            JS_ReportError(cx, "JSParticleSystem::create: needs between 0 and 5 arguments!");
            return JS_FALSE;
        }
        
        if (argc >= 1) {
            if (!convertFrom(cx, argv[0], myParentNode)) {
                JS_ReportError(cx, "JSParticleSystem::create: argument #1 must be an integer");
                return JS_FALSE;
            }
        }

        if (argc >= 2) {
            if (!convertFrom(cx, argv[1], myParticleCount)) {
                JS_ReportError(cx, "JSParticleSystem::create: argument #2 must be an integer");
                return JS_FALSE;
            }
        }
        
        if (argc >= 3) {
            if (!convertFrom(cx, argv[2], myTextureName)) {
                JS_ReportError(cx, "JSParticleSystem::create: argument #3 must be a string");
                return JS_FALSE;
            }
        }
        
        if (argc >= 4) {
            if (!convertFrom(cx, argv[3], myInitialDirection)) {
                JS_ReportError(cx, "JSParticleSystem::create: argument #4 must be a vector3f");
                return JS_FALSE;
            }
        }
        
        if (argc >= 5) {
            if (!convertFrom(cx, argv[4], myScattering)) {
                JS_ReportError(cx, "JSParticleSystem::create: argument #5 must be a vector2f");
                return JS_FALSE;
            }
        }
        
        if (argc >= 6) {
            if (!convertFrom(cx, argv[5], mySpeedRange)) {
                JS_ReportError(cx, "JSParticleSystem::create: argument #6 must be a vetor2f");
                return JS_FALSE;
            }
        }
            
        return Method<JSParticleSystem::NATIVE>::call(&JSParticleSystem::NATIVE::create, cx, obj, argc, argv, rval);
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
remove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("remove all data created by the particle system instance from the dom."); DOC_END;
    return Method<JSParticleSystem::NATIVE>::call(&JSParticleSystem::NATIVE::remove,cx,obj,argc,argv,rval);
}

JSParticleSystem::~JSParticleSystem() {
}

JSFunctionSpec *
JSParticleSystem::Functions() {
    IF_REG(cerr << "Registering class '" << ClassName() << "'" << endl);
    static JSFunctionSpec myFunctions[] = {
        // name      native       nargs
        {"toString", toString,    0},
        {"create",   create,      6},
        {"remove",   remove,      0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSParticleSystem::Properties() {
    static JSPropertySpec myProperties[] = {
        {"material",   PROP_material,   JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"body",       PROP_body,       JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"animation",  PROP_animation,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"image",      PROP_image,      JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSParticleSystem::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSParticleSystem::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name             native           nargs
        {0}
    };
    return myFunctions;
}

// getproperty handling
JSBool
JSParticleSystem::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_material:
            *vp = as_jsval(cx, getNative().getMaterialNode());
            return JS_TRUE;
        case PROP_body:
            *vp = as_jsval(cx, getNative().getBodyNode());
            return JS_TRUE;
        case PROP_animation:
            *vp = as_jsval(cx, getNative().getAnimationNode());
            return JS_TRUE; 
        case PROP_image:
            *vp = as_jsval(cx, getNative().getImageNode());
            return JS_TRUE; 
        default:
            JS_ReportError(cx,"JSParticleSystem::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSParticleSystem::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    
    switch (theID) {
        default:
            JS_ReportError(cx,"JSParticleSystem::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSParticleSystem::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a ParticleSystem Device. Call tuneChannel to tune for a certain channel");
    DOC_PARAM("theScene", "the scene Object", DOC_TYPE_NODE);
    DOC_END;
    
    try{
        ensureParamCount(argc, 1);
        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
            return JS_FALSE;
        }
        
        y60::ScenePtr myScene;
        if (!convertFrom(cx, argv[0], myScene)) {
            JS_ReportError(cx, "JSParticleSystem::Constructor: argument #1 must be the scene");
            return JS_FALSE;
        }
    
        OWNERPTR myParticleSystem = OWNERPTR(new ParticleSystem(myScene));
        JSParticleSystem * myNewObject = new JSParticleSystem(myParticleSystem, &(*myParticleSystem));
    
        if (myNewObject) {
            JS_SetPrivate(cx, obj, myNewObject);
            return JS_TRUE;
        } 
    
        JS_ReportError(cx,"JSParticleSystem::Constructor: bad parameters");

        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JSConstIntPropertySpec *
JSParticleSystem::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSObject *
JSParticleSystem::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), 
                                        Functions(), ConstIntProperties(), 0, StaticFunctions());
    DOC_MODULE_CREATE("Components", JSParticleSystem);
    return myClass;
}

jsval as_jsval(JSContext *cx, JSParticleSystem::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSParticleSystem::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSParticleSystem::OWNERPTR theOwner, JSParticleSystem::NATIVE * theSerial) {
    JSObject * myObject = JSParticleSystem::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
