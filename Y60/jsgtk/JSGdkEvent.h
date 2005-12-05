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
//   $RCSfile: JSGdkEvent.h,v $
//   $Author: david $
//   $Revision: 1.2 $
//   $Date: 2005/04/13 15:24:13 $
//
//
//=============================================================================
#ifndef _Y60_ACGTKSHELL_JSEVENT_BUTTON_INCLUDED_
#define _Y60_ACGTKSHELL_JSEVENT_BUTTON_INCLUDED_

#include <y60/JSWrapper.h>
#include <gdk/gdk.h>
#include <asl/string_functions.h>

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

jsval as_jsval(JSContext *cx, JSGdkEvent::OWNERPTR theOwner, JSGdkEvent::NATIVE * theNative);

jsval as_jsval(JSContext *cx, GdkEvent * theNative);
jsval as_jsval(JSContext *cx, GdkEventButton * theNative);
jsval as_jsval(JSContext *cx, GdkEventMotion * theNative);
jsval as_jsval(JSContext *cx, GdkEventKey * theNative);
jsval as_jsval(JSContext *cx, GdkEventFocus * theNative);

} // namespace

#endif


