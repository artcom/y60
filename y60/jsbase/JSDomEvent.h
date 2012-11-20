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

#ifndef _Y60_ACXPSHELL_JSDomEvent_INCLUDED_
#define _Y60_ACXPSHELL_JSDomEvent_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSWrapper.h"

#include <asl/dom/Events.h>
#include <asl/base/string_functions.h>


struct JSObject;

namespace jslib {

typedef dom::GenericEvent<JSObject*> GenericJSDomEvent;

class Y60_JSBASE_DECL JSDomEvent : public JSWrapper<GenericJSDomEvent, asl::Ptr<GenericJSDomEvent, dom::ThreadingModel>, StaticAccessProtocol>
{
        JSDomEvent() {}
    public:
        typedef GenericJSDomEvent NATIVE;
        typedef asl::Ptr<NATIVE, dom::ThreadingModel> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "DomEvent";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_CAPTURING_PHASE = -100,
            PROP_AT_TARGET,
            PROP_BUBBLING_PHASE,
            PROP_type,
            PROP_target,
            PROP_currentTarget,
            PROP_targetOnly,
            PROP_eventPhase,
            PROP_bubbles,
            PROP_cancelable,
            PROP_timeStamp,
            PROP_isDefaultPrevented,
            PROP_data
        };
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();


        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }
        static
        JSObject * Construct(JSContext *cx, dom::EventPtr theOwner, NATIVE * theNative) {
            OWNERPTR myEvent = dynamic_cast_Ptr<GenericJSDomEvent>(theOwner);
            return Base::Construct(cx, myEvent, theNative);
        }

        JSDomEvent(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSDomEvent & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSDomEvent &>(JSDomEvent::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<GenericJSDomEvent> : public JSClassTraitsWrapper<GenericJSDomEvent, JSDomEvent> {};

template <>
struct JSClassTraits<dom::Event> : public JSClassTraitsWrapper<GenericJSDomEvent, JSDomEvent> {};

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, dom::EventPtr & theEvent);

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::EventPtr theOwner);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::EventPtr, JSDomEvent::NATIVE * theEvent);

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::EventTargetPtr theOwner);

struct JSDomEventListener : public dom::EventListener {
    JSDomEventListener(JSContext * theContext, JSObject * theEventListener, const std::string & theMethodName = "handleEvent");
    ~JSDomEventListener();
    void handleEvent(dom::EventPtr theEvent);

    JSObject * getJSListener() {
        return _myEventListener;
    }
    void setMethodName(const std::string & theMethodName) {
        _myMethodName = theMethodName;
    }
    JSContext * _myJSContext;
    JSObject * _myEventListener;
    std::string _myMethodName;
};

typedef asl::Ptr<JSDomEventListener,dom::ThreadingModel> JSDomEventListenerPtr;

inline
bool convertFrom(JSContext *cx, jsval theValue, dom::EventListenerPtr & theEventListener) {
     JSObject * myObject;
     if (JS_ValueToObject(cx, theValue, &myObject)) {
        theEventListener = JSDomEventListenerPtr(new JSDomEventListener(cx, myObject));
        return true;
    }
    return false;
}

inline
jsval as_jsval(JSContext *cx, asl::Ptr<JSDomEventListener> theOwner) {
    return OBJECT_TO_JSVAL(theOwner->getJSListener());
}

inline
jsval as_jsval(JSContext *cx, asl::Ptr<JSDomEventListener>, JSDomEventListener * theEvent) {
    return OBJECT_TO_JSVAL(theEvent->getJSListener());
}

}

#endif

