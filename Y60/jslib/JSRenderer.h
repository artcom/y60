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
//   $RCSfile: JSRenderer.h,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2005/02/10 19:59:31 $
//
//   Description: Base class wrapper for RenderWindow
//   TODO: this is not (yet) used by the SDL-based app
//
//=============================================================================

#ifndef _Y60_JSLIB_JSRENDERER_INCLUDED_
#define _Y60_JSLIB_JSRENDERER_INCLUDED_

#include "JSWrapper.h"
#include "JSLine.h"
#include "JSTriangle.h"
#include "JSSphere.h"
#include "JSVector.h"
#include "JSMatrix.h"
#include "JSNode.h"

#include <y60/Renderer.h>

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/intersection.h>
#include <asl/Exception.h>

#include <dom/Value.h>

#include <y60/Scene.h>

DEFINE_EXCEPTION(JSRendererException, asl::Exception);

namespace jslib {

class JSRenderer : public JSWrapper<y60::Renderer,asl::Ptr<y60::Renderer>, StaticAccessProtocol> {
public:
    typedef y60::Renderer NATIVE;
    typedef asl::Ptr<y60::Renderer> OWNERPTR;
    typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

    static const char * ClassName() {
        return "Renderer";
    }
    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();



    virtual unsigned long length() const {
        return 0;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, asl::Ptr<y60::Renderer> theRenderer, y60::Renderer * theNative) {
        return Base::Construct(cx, theRenderer, theNative);
    }
    JSRenderer(asl::Ptr<y60::Renderer> theOwner, y60::Renderer * theRenderer)
        : Base(theOwner, theRenderer)
    {}

    y60::Scene & getScene() {
        y60::ScenePtr myScene = getNative().getCurrentScene();
        return *myScene;
    }


    static void addClassProperties(JSContext * cx, JSObject * theClassProto);
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
private:
    JSRenderer(); // hide default constructor
    JSRenderer(const JSRenderer &); // hide copy constructor
    JSRenderer & operator=(const JSRenderer &); // hide assignment operator
};

template <>
struct JSClassTraits<y60::Renderer> : public JSClassTraitsWrapper<y60::Renderer, JSRenderer> {};

bool convertFrom(JSContext *cx, jsval theValue, y60::Renderer *& theRenderer);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<y60::Renderer> & theRenderer);

jsval as_jsval(JSContext *cx, JSRenderer::OWNERPTR & theOwner);

}

#endif

