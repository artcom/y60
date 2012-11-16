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

#ifndef _Y60_ACXPSHELL_JSRENDERER_INCLUDED_
#define _Y60_ACXPSHELL_JSRENDERER_INCLUDED_

#include "y60_sdlengine_settings.h"

#include "SDLWindow.h"

#include <y60/jsbase/JSWrapper.h>
#include <y60/jslib/JSAbstractRenderWindow.h>

#include <asl/math/Vector234.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/intersection.h>
#include <asl/base/Exception.h>

#include <asl/dom/Value.h>

#include <y60/scene/Scene.h>

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

    static void addClassProperties(JSContext * cx, JSObject * theClassProto);
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
};

namespace jslib {


template <>
struct JSClassTraits<y60::SDLWindow> : public JSClassTraitsWrapper<y60::SDLWindow, JSRenderWindow> {
    static JSClass * Class() {
        return JSRenderWindow::Base::Class();
    }
};

Y60_SDLENGINE_DECL bool convertFrom(JSContext *cx, jsval theValue, y60::SDLWindow *& theRenderWindow);
Y60_SDLENGINE_DECL bool convertFrom(JSContext *cx, jsval theValue, jslib::AbstractRenderWindow *& theRenderWindow);

Y60_SDLENGINE_DECL jsval as_jsval(JSContext *cx, asl::Ptr<y60::SDLWindow> theOwner);

}

#endif
