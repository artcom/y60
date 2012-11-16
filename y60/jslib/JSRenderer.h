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

#ifndef _Y60_JSLIB_JSRENDERER_INCLUDED_
#define _Y60_JSLIB_JSRENDERER_INCLUDED_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSTriangle.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSNode.h>

#include <y60/glrender/Renderer.h>

#include <asl/math/Vector234.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/intersection.h>
#include <asl/base/Exception.h>

#include <asl/dom/Value.h>

#include <y60/scene/Scene.h>

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

Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, y60::Renderer *& theRenderer);
Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<y60::Renderer> & theRenderer);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, JSRenderer::OWNERPTR & theOwner);

}

#endif

