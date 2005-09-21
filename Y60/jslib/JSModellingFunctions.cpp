//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSModellingFunctions.cpp,v $
//   $Author: christian $
//   $Revision: 1.10 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSModellingFunctions.h"
#include "JScppUtils.h"

#include "JSNode.h"
#include "JSVector.h"

#include "JSScene.h"
#include <y60/modelling_functions.h>

#include <iostream>

using namespace std;
using namespace asl;

using namespace y60;

namespace jslib {

JS_STATIC_DLL_CALLBACK(JSBool)
CreateTransform(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;
        ensureParamCount(argc, 1);

        dom::NodePtr  myParentNode(0);
        if (!convertFrom(cx, argv[0], myParentNode)) {
            JS_ReportError(cx,"CreateTransform: argument 1 is not a node");
            return JS_FALSE;
        }

        dom::NodePtr myResult = createTransform(myParentNode);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateCanvas(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;

        ensureParamCount(argc, 2);

        y60::ScenePtr mySceneNode(0);
        convertFrom(cx, argv[0], mySceneNode);
        string myCanvasName;
        convertFrom(cx, argv[1], myCanvasName);

        dom::NodePtr myResult = createCanvas(mySceneNode, myCanvasName);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateBody(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;

        ensureParamCount(argc, 2);

        dom::NodePtr  myParentNode(0);
        convertFrom(cx, argv[0], myParentNode);
        string myShapeId;
        convertFrom(cx, argv[1], myShapeId);

        dom::NodePtr myResult = createBody(myParentNode, myShapeId);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateQuad(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;

        ensureParamCount(argc, 4);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);
        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);
        Vector3f myTopLeftCorner;
        convertFrom(cx, argv[2], myTopLeftCorner);
        Vector3f myBottomRightCorner;
        convertFrom(cx, argv[3], myBottomRightCorner);

        dom::NodePtr myResult = createQuad(myScene, myMaterialId,
                                           myTopLeftCorner, myBottomRightCorner);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateCrosshair(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;

        ensureParamCount(argc, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        float myInnerRadius;
        convertFrom(cx, argv[2], myInnerRadius);

        float myHairLength;
        convertFrom(cx, argv[3], myHairLength);

        string myName;
        convertFrom(cx, argv[4], myName);

        dom::NodePtr myResult = createCrosshair(myScene, myMaterialId, myInnerRadius,
                                                myHairLength, myName);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateDistanceMarkup(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;
        ensureParamCount(argc, 3, 4);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        std::vector<asl::Vector3f> myPositions;
        convertFrom(cx, argv[2], myPositions);

        string myName;
        if (argc > 3) {
            convertFrom(cx, argv[3], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 3:
                myResult = createDistanceMarkup(myScene, myMaterialId, myPositions);
                break;
            case 4:
                myResult = createDistanceMarkup(myScene, myMaterialId,
                                                myPositions, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateAngleMarkup(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;
        ensureParamCount(argc, 6, 7);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        asl::Vector3f myApex;
        convertFrom(cx, argv[2], myApex);

        asl::Vector3f myPointA;
        convertFrom(cx, argv[3], myPointA);

        asl::Vector3f myPointB;
        convertFrom(cx, argv[4], myPointB);

        bool myOuterAngleFlag;
        convertFrom(cx, argv[5], myOuterAngleFlag);

        string myName;
        if (argc > 6) {
            convertFrom(cx, argv[6], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 6:
                myResult = createAngleMarkup(myScene, myMaterialId, myApex, myPointA,
                                             myPointB, myOuterAngleFlag);
                break;
            case 7:
                myResult = createAngleMarkup(myScene, myMaterialId, myApex, myPointA,
                                             myPointB, myOuterAngleFlag, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateTriangleMeshMarkup(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;
        ensureParamCount(argc, 4, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myLineMaterialId;
        convertFrom(cx, argv[1], myLineMaterialId);

        string myAreaMaterialId;
        convertFrom(cx, argv[2], myAreaMaterialId);

        std::vector<asl::Vector3f> myPositions;
        convertFrom(cx, argv[3], myPositions);

        string myName;
        if (argc > 4) {
            convertFrom(cx, argv[4], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 4:
                myResult = createTriangleMeshMarkup(myScene, myLineMaterialId, myAreaMaterialId,
                                             myPositions);
                break;
            case 5:
                myResult = createTriangleMeshMarkup(myScene, myLineMaterialId, myAreaMaterialId,
                                             myPositions, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateUnlitTexturedMaterial(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {

    try {
        DOC_BEGIN("");
        DOC_END;

        ensureParamCount(argc, 1, 7);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        dom::NodePtr myResult;

        string myTextureFilename;
        if (argc > 1) {
            convertFrom(cx, argv[1], myTextureFilename);
        }
        string myName;
        if (argc > 2) {
            convertFrom(cx, argv[2], myName);
        }
        bool myTransparencyFlag;
        if (argc > 3) {
            convertFrom(cx, argv[3], myTransparencyFlag);
        }
        bool mySpriteFlag;
        if (argc > 4) {
            convertFrom(cx, argv[4], mySpriteFlag);
        }
        unsigned myDepth;
        if (argc > 5) {
            convertFrom(cx, argv[5], myDepth);
        }
        Vector4f myColor;
        if (argc > 6) {
            convertFrom(cx, argv[6], myColor);
        }


        switch (argc) {
            case 1:
                myResult = createUnlitTexturedMaterial(myScene,"");
                break;
            case 2:
                myResult = createUnlitTexturedMaterial(myScene,myTextureFilename);
                break;
            case 3:
                myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,myName);
                break;
            case 4:
                myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                        myName,myTransparencyFlag);
                break;
            case 5:
                myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                        myName,myTransparencyFlag,mySpriteFlag);
                break;
            case 6:
                myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                        myName,myTransparencyFlag,mySpriteFlag,myDepth);
                break;
            case 7:
                myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                        myName,myTransparencyFlag,mySpriteFlag,myDepth, myColor);
                break;

        }

        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateQuadStack(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("");
        DOC_END;
        ensureParamCount(argc, 4, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        asl::Vector3i myDimensions;
        convertFrom(cx, argv[1], myDimensions);

        float mySize;
        convertFrom(cx, argv[2], mySize);

        string myMaterialId;
        convertFrom(cx, argv[3], myMaterialId);

        string myName;
        if (argc > 4) {
            convertFrom(cx, argv[4], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 4:
                myResult = createQuadStack(myScene, myDimensions, mySize,
                                             myMaterialId);
                break;
            case 5:
                myResult = createQuadStack(myScene, myDimensions, mySize,
                                             myMaterialId, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}


static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("Renderer@") + as_string(obj);
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

JSFunctionSpec *
JSModellingFunctions::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSModellingFunctions::StaticFunctions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                         native                       nargs
        {"createTransform",             CreateTransform,             1},
        {"createBody",                  CreateBody,                  2},
        {"createCanvas",                CreateCanvas,                2},
        {"createQuad",                  CreateQuad,                  2},
        {"createCrosshair",             CreateCrosshair,             5},
        {"createDistanceMarkup",        CreateDistanceMarkup,        5},
        {"createAngleMarkup",           CreateAngleMarkup,           6},
        {"createTriangleMeshMarkup",    CreateTriangleMeshMarkup,    5},
        {"createQuadStack",             CreateQuadStack,             5},
        {"createUnlitTexturedMaterial", CreateUnlitTexturedMaterial, 7},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSModellingFunctions::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSModellingFunctions::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSModellingFunctions::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

// getproperty handling
JSBool
JSModellingFunctions::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_FALSE;
}

// setproperty handling
JSBool
JSModellingFunctions::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_FALSE;
}

JSBool
JSModellingFunctions::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    AC_TRACE << "Constructor start " << endl;
    AC_TRACE << "Constructor argc =" << argc << endl;
    if (JSA_GetClass(cx,obj) != Base::Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    JSModellingFunctions *myNewObject = 0;
    OWNERPTR myNewNative = OWNERPTR(new bool);
    myNewObject = new JSModellingFunctions(myNewNative, &(*myNewNative));

     if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSAudio::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSModellingFunctions::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
}

JSObject *
JSModellingFunctions::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
        Properties(), Functions(), 0, 0, StaticFunctions());
    DOC_CREATE(JSModellingFunctions);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, DummyT *& theDummy) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSModellingFunctions::NATIVE >::Class()) {
                theDummy = &(*JSClassTraits<JSModellingFunctions::NATIVE>::getNativeOwner(cx,myArgument));
                return true;
            }
        }
    }
    return false;
}


bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<DummyT> & theDummy) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSModellingFunctions::NATIVE >::Class()) {
                theDummy = JSClassTraits<JSModellingFunctions::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSModellingFunctions::OWNERPTR & theOwner) {
    JSObject * myReturnObject = JSModellingFunctions::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

}



