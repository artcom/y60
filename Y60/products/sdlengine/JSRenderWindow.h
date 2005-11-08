//=============================================================================
// Copyright (C) 2003-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSRENDERER_INCLUDED_
#define _Y60_ACXPSHELL_JSRENDERER_INCLUDED_

#include "SDLWindow.h"

#include <y60/JSWrapper.h>
#include <y60/JSAbstractRenderWindow.h>

#include <asl/Vector234.h>
#include <asl/numeric_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/intersection.h>
#include <asl/Exception.h>

#include <dom/Value.h>

#include <y60/Scene.h>

DEFINE_EXCEPTION(JSRendererWindowException, asl::Exception);

class JSRenderWindow : public jslib::JSWrapper<y60::SDLWindow,asl::Ptr<y60::SDLWindow>, jslib::StaticAccessProtocol> {
public:
    typedef y60::SDLWindow NATIVE;
    typedef asl::Ptr<y60::SDLWindow> OWNERPTR;
    typedef jslib::JSWrapper<NATIVE,OWNERPTR,jslib::StaticAccessProtocol> Base;
    typedef jslib::JSAbstractRenderWindow<NATIVE> JSBase;

    static const char * ClassName() {
        return "RenderWindow";
    }
    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();

    virtual unsigned long length() const {
        return 0;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, asl::Ptr<y60::SDLWindow> theOwner, y60::SDLWindow * theRenderer) {
        return Base::Construct(cx, theOwner, theRenderer);
    }
    JSRenderWindow(asl::Ptr<y60::SDLWindow> theOwner, y60::SDLWindow * theRenderer)
        : Base(theOwner, theRenderer)
    {}

    y60::Scene & getScene() {
        y60::ScenePtr myScene = Base::getNative().getCurrentScene();
        return *myScene;
    }

    static jslib::JSConstIntPropertySpec * ConstIntProperties();

    static void JSRenderWindow::addClassProperties(JSContext * cx, JSObject * theClassProto);
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};

namespace jslib {


template <>
struct JSClassTraits<y60::SDLWindow> : public JSClassTraitsWrapper<y60::SDLWindow, JSRenderWindow> {
    static JSClass * Class() {
        return JSRenderWindow::Base::Class();
    }
};

bool convertFrom(JSContext *cx, jsval theValue, y60::SDLWindow *& theRenderWindow);
bool convertFrom(JSContext *cx, jsval theValue, jslib::AbstractRenderWindow *& theRenderWindow);

jsval as_jsval(JSContext *cx, asl::Ptr<y60::SDLWindow> theOwner);

}

#endif
