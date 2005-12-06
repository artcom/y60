//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSStringMover.cpp,v $
//   $Author: christian $
//   $Revision: 1.10 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSStringMover.h"
#include "JScppUtils.h"
#include "JSNode.h"
#include "JSVector.h"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
onFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Must be called once per frame");
    DOC_PARAM("theTime", "The current time", DOC_TYPE_FLOAT);
    DOC_END;
    return Method<JSStringMover::NATIVE>::call(&JSStringMover::NATIVE::onFrame,cx,obj,argc,argv,rval);
}

static JSBool
addCharacter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Adds a new character position. There must exist a corresponding shape (see CharacterMover.js)");
    DOC_PARAM("theFinalPosition", "Position where the character should move to after the explosion", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<JSStringMover::NATIVE>::call(&JSStringMover::NATIVE::addCharacter,cx,obj,argc,argv,rval);
}

static JSBool
resetPositions(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Reset all characters to new positions");
    DOC_PARAM("theStartPosition", "Startposition of the character explosion", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theTargetPositionOffset", "Offset between start and target position", DOC_TYPE_VECTOR3F);
    DOC_END;
    return Method<JSStringMover::NATIVE>::call(&JSStringMover::NATIVE::resetPositions,cx,obj,argc,argv,rval);
}

static JSBool
onMouseButton(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Triggers interaction depending on the current mouse position");
    DOC_PARAM("theMousePosition", "Mouse position in screen space", DOC_TYPE_VECTOR2F);
    DOC_END;
    return Method<JSStringMover::NATIVE>::call(&JSStringMover::NATIVE::onMouseButton,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSStringMover::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"onFrame",              onFrame,                 1},
        {"addCharacter",         addCharacter,            2},
        {"resetPositions",       resetPositions,          2},
        {"onMouseButton",        onMouseButton,           1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSStringMover::Properties() {
    static JSPropertySpec myProperties[] = {
        {"state", PROP_STATE, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSStringMover::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSStringMover::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSStringMover::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSStringMover::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_STATE:
            *vp = as_jsval(cx, int(getNative().getState()));
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSStringMover::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }

}

// setproperty handling
JSBool
JSStringMover::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_STATE:
            jsval dummy;
            return Method<NATIVE>::call(&NATIVE::setState, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSStringMover::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSStringMover::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new string mover. (An object to move around charachters in funny ways)");
    DOC_PARAM("theStartPosition", "The world position, where the charachters should explode from", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theShapeNode", "The shape, which contains all the character quads (see CharacterMover.js)", DOC_TYPE_NODE);
    DOC_PARAM("theBodyNode", "The body, connected to the character shape", DOC_TYPE_NODE);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSStringMover * myNewObject = 0;

    if (argc == 3) {
        asl::Vector3f myStartPos;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myStartPos)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #1 must be an vector");
            return JS_FALSE;
        }

        dom::NodePtr myShapeNode = dom::NodePtr(0);
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myShapeNode)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #2 must be an node");
            return JS_FALSE;
        }


        dom::NodePtr myBodyNode = dom::NodePtr(0);
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myBodyNode)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #3 must be an node");
            return JS_FALSE;
        }

        OWNERPTR myNewCharacterMover = OWNERPTR(new StringMover(myStartPos, asl::Vector3f(0.0f, 0.0f, 0.0f), myShapeNode, myBodyNode));
        myNewObject = new JSStringMover(myNewCharacterMover, &(*myNewCharacterMover));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 3 (startpos, shapenode) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSStringMover::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSStringMover::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), 0);
    DOC_CREATE(JSStringMover);
    return myClass;
}

}

