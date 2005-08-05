//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSEvent.h,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSEvent_INCLUDED_
#define _Y60_ACXPSHELL_JSEvent_INCLUDED_

#include "JSWrapper.h"

#include <dom/Events.h>
#include <asl/string_functions.h>


struct JSObject;

//struct GenericJSEvent : public dom::GenericEvent<JSObject*> {
//};

namespace jslib {

typedef dom::GenericEvent<JSObject*> GenericJSEvent;

class JSEvent : public JSWrapper<GenericJSEvent, asl::Ptr<GenericJSEvent, dom::ThreadingModel>, StaticAccessProtocol>
{
        JSEvent() {}
    public:
        typedef GenericJSEvent NATIVE;
        typedef asl::Ptr<NATIVE, dom::ThreadingModel> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "JSEvent";
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
            PROP_isDefaultPrevented
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
            OWNERPTR myEvent = dynamic_cast_Ptr<GenericJSEvent>(theOwner);
            return Base::Construct(cx, myEvent, theNative);
        }

        JSEvent(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSEvent & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSEvent &>(JSEvent::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<GenericJSEvent> : public JSClassTraitsWrapper<GenericJSEvent, JSEvent> {};

template <>
struct JSClassTraits<dom::Event> : public JSClassTraitsWrapper<GenericJSEvent, JSEvent> {};

bool convertFrom(JSContext *cx, jsval theValue, dom::EventPtr & theEvent);

jsval as_jsval(JSContext *cx, dom::EventPtr theOwner);
jsval as_jsval(JSContext *cx, dom::EventPtr, JSEvent::NATIVE * theEvent);


jsval as_jsval(JSContext *cx, dom::EventTargetPtr theOwner);


struct JSEventListener : public dom::EventListener {
    JSEventListener(JSContext * theContext, JSObject * theEventListener, const std::string & theMethodName = "handleEvent")
        : _myJSContext(theContext), _myEventListener(theEventListener), _myMethodName(theMethodName)
    {}
    void handleEvent(dom::EventPtr theEvent) {
        asl::Ptr<GenericJSEvent, dom::ThreadingModel> myGenericEvent = dynamic_cast_Ptr<GenericJSEvent>(theEvent);
        if (myGenericEvent) {
            jsval argv[1], rval;
            argv[0] = myGenericEvent->getPayload();
            JSBool ok = JSA_CallFunctionName(_myJSContext, _myEventListener, _myMethodName.c_str(), 0, argv, &rval);
        } else {
            AC_ERROR << "JSEventListener::handleEvent: not a js listener" << std::endl;
        }
    }
    JSObject * getJSListener() {
        return _myEventListener;
    }
    JSContext * _myJSContext;
    JSObject * _myEventListener;
    std::string _myMethodName;
};

typedef asl::Ptr<JSEventListener,dom::ThreadingModel> JSEventListenerPtr;

// creates a new wrapper object for the object passed in value; it does extract a wrapped value as usaual
//bool convertFrom(JSContext *cx, jsval theValue, JSEventListener & theEventListener) {
inline
bool convertFrom(JSContext *cx, jsval theValue, dom::EventListenerPtr & theEventListener) {
     JSObject * myObject;
     if (JS_ValueToObject(cx, theValue, &myObject)) {
        theEventListener = JSEventListenerPtr(new JSEventListener(cx, myObject));
        return true;
    }
    return false;
}

inline
jsval as_jsval(JSContext *cx, asl::Ptr<JSEventListener> theOwner) {
    return OBJECT_TO_JSVAL(theOwner->getJSListener());
}

inline
jsval as_jsval(JSContext *cx, asl::Ptr<JSEventListener>, JSEventListener * theEvent) {
    return OBJECT_TO_JSVAL(theEvent->getJSListener());
}

}

#endif

