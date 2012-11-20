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

#ifndef _ac_jslib_JSOffscreenRenderArea_h_
#define _ac_jslib_JSOffscreenRenderArea_h_

#include "y60_jslib_settings.h"

#include <y60/scene/Scene.h>

#include "OffscreenRenderArea.h"
#include "JSAbstractRenderWindow.h"

/**
 *
 * @ingroup Y60jslib
 */
namespace jslib {

class JSOffscreenRenderArea : public jslib::JSWrapper<jslib::OffscreenRenderArea, asl::Ptr<OffscreenRenderArea>, StaticAccessProtocol> {
    public:
        typedef jslib::OffscreenRenderArea NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;
        typedef JSAbstractRenderWindow<jslib::OffscreenRenderArea> JSBASE;

        static const char * ClassName() {
            return "OffscreenRenderArea";
        }
        static JSFunctionSpec * Functions();
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        virtual unsigned long length() const {
            return 1;
        }

        static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                                        JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx,
                                         JSObject *obj, jsval id, jsval *vp);
        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID,
                                        JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj,
                                         jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSOffscreenRenderArea(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSOffscreenRenderArea & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSOffscreenRenderArea &>(JSOffscreenRenderArea::getJSWrapper(cx,obj));
        }

    private:
};

bool convertFrom(JSContext *cx, jsval theValue, OffscreenRenderArea *& theRenderArea);

} //namespace

#endif // _ac_jslib_JSOffscreenRenderArea_h_
