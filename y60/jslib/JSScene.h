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

#ifndef _Y60_JSLIB_JSSCENE_INCLUDED_
#define _Y60_JSLIB_JSSCENE_INCLUDED_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JSWrapper.h>

#include <y60/scene/Scene.h>


namespace jslib {

    class JSScene : public JSWrapper<y60::Scene, asl::Ptr<y60::Scene, dom::ThreadingModel>, StaticAccessProtocol> {
    JSScene() {}
public:
    virtual ~JSScene();
    typedef y60::Scene NATIVE;
    typedef asl::Ptr<y60::Scene, dom::ThreadingModel> OWNERPTR;
    typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;
    static const char * ClassName() {
        return "Scene";
    }
    static JSFunctionSpec * Functions();
    static JSFunctionSpec * StaticFunctions();

    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * Properties();
    static JSPropertySpec * StaticProperties();

    virtual unsigned long length() const {
        return 1;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
        JSObject * Construct(JSContext *cx, asl::Ptr<y60::Scene,dom::ThreadingModel> theScene, y60::Scene * theNative) {
        return Base::Construct(cx, theScene, theNative);
    }
    JSScene(asl::Ptr<y60::Scene, dom::ThreadingModel> theScene, y60::Scene * theNative)
        : Base(theScene, theNative)
    {}

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

    static JSBool
    save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    saveWithCatalog(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    /*
    static JSObject *
    ConstructWithArgs(JSContext * cx, OWNERPTR theOwner, NATIVE * theNative,
            uintN argc, jsval * argv);
    */


private:
    JSBool getStatistics(JSContext *cx, jsval *vp);

};

template <>
struct JSClassTraits<y60::Scene>
    : public JSClassTraitsWrapper<y60::Scene, JSScene> {};

Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<y60::Scene, dom::ThreadingModel> & theScene);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, JSScene::OWNERPTR theScene);

}

#endif
