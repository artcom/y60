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

#ifndef _Y60_ACGTKSHELL_JSEVENT_BUTTON_INCLUDED_
#define _Y60_ACGTKSHELL_JSEVENT_BUTTON_INCLUDED_

#include "y60_jsgtk_settings.h"

#include <y60/jsbase/JSWrapper.h>
#include <gdk/gdk.h>
#include <asl/base/string_functions.h>

namespace jslib {

class JSGdkEvent : public JSWrapper<GdkEvent, asl::Ptr<GdkEvent>, StaticAccessProtocol> {
        JSGdkEvent();  // hide default constructor
    public:
        virtual ~JSGdkEvent() {
        }
        typedef GdkEvent NATIVE;
        typedef asl::Ptr<GdkEvent> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "GdkEvent";
        };
        static JSFunctionSpec * Functions();

        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }
        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSGdkEvent(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
       //  static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        // getproperty handling
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSGdkEvent & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGdkEvent &>(JSGdkEvent::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSGdkEvent::NATIVE>
    : public JSClassTraitsWrapper<JSGdkEvent::NATIVE, JSGdkEvent> {};

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSGdkEvent::OWNERPTR theOwner, JSGdkEvent::NATIVE * theNative);

template<> Y60_JSGTK_DECL jsval as_jsval<GdkEvent>(JSContext *cx, GdkEvent* theNative);
template<> Y60_JSGTK_DECL jsval as_jsval<GdkEventKey>(JSContext *cx, GdkEventKey * theNative);
template<> Y60_JSGTK_DECL jsval as_jsval<GdkEventAny>(JSContext *cx, GdkEventAny * theNative);
template<> Y60_JSGTK_DECL jsval as_jsval<GdkEventFocus>(JSContext *cx, GdkEventFocus * theNative);
template<> Y60_JSGTK_DECL jsval as_jsval<GdkEventButton>(JSContext *cx, GdkEventButton * theNative);
template<> Y60_JSGTK_DECL jsval as_jsval<GdkEventCrossing>(JSContext *cx, GdkEventCrossing * theNative);
template<> Y60_JSGTK_DECL jsval as_jsval<GdkEventMotion>(JSContext *cx, GdkEventMotion * theNative);

} // namespace

#endif


