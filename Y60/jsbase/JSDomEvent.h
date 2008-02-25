//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSDomEvent_INCLUDED_
#define _Y60_ACXPSHELL_JSDomEvent_INCLUDED_

#include "JSWrapper.h"

#include <dom/Events.h>
#include <asl/string_functions.h>


struct JSObject;

namespace jslib {

typedef dom::GenericEvent<JSObject*> GenericJSDomEvent;

class JSDomEvent : public JSWrapper<GenericJSDomEvent, asl::Ptr<GenericJSDomEvent, dom::ThreadingModel>, StaticAccessProtocol>
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

bool convertFrom(JSContext *cx, jsval theValue, dom::EventPtr & theEvent);

jsval as_jsval(JSContext *cx, dom::EventPtr theOwner);
jsval as_jsval(JSContext *cx, dom::EventPtr, JSDomEvent::NATIVE * theEvent);


jsval as_jsval(JSContext *cx, dom::EventTargetPtr theOwner);


struct JSDomEventListener : public dom::EventListener {
    JSDomEventListener(JSContext * theContext, JSObject * theEventListener, const std::string & theMethodName = "handleEvent");
    ~JSDomEventListener();
    void handleEvent(dom::EventPtr theEvent);
    
    JSObject * getJSListener() {
        return _myEventListener;
    }
    JSContext * _myJSContext;
    JSObject * _myEventListener;
    std::string _myMethodName;
};

typedef asl::Ptr<JSDomEventListener,dom::ThreadingModel> JSDomEventListenerPtr;

#if 1
// creates a new wrapper object for the object passed in value; it does extract a wrapped value as usaual
//bool convertFrom(JSContext *cx, jsval theValue, JSDomEventListener & theEventListener) {
inline
bool convertFrom(JSContext *cx, jsval theValue, dom::EventListenerPtr & theEventListener) {
     JSObject * myObject;
     if (JS_ValueToObject(cx, theValue, &myObject)) {
        theEventListener = JSDomEventListenerPtr(new JSDomEventListener(cx, myObject));
        return true;
    }
    return false;
}
#endif
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

