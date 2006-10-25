//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSEvent.h"
#include "JSNode.h"
#include "JScppUtils.h"
#include "JSWrapper.impl"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<GenericJSEvent, asl::Ptr<GenericJSEvent, dom::ThreadingModel>, 
         StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the type of the native event");
    DOC_RVAL("The string", DOC_TYPE_STRING);
    DOC_END;
    JSEvent::Base * myWrapperPtr = JSEvent::getJSWrapperPtr(cx,obj);
    if (myWrapperPtr) {
        std::string myStringRep = std::string("Event<")+myWrapperPtr->getNative().type()+">";
        *rval = as_jsval(cx, myStringRep);
        return JS_TRUE;
    } else {
        *rval = STRING_TO_JSVAL("");
        return JS_FALSE;
    }
}
static JSBool
initEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Initialize Event with id, bubble up flag and listener cancelable flag.");
    DOC_PARAM("theEventIdentifier", "", DOC_TYPE_STRING);
    DOC_PARAM("theCanBubbleFlag", "", DOC_TYPE_BOOLEAN);
    DOC_PARAM("theCancelableFlag", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    Method<dom::Event>::call(&dom::Event::initEvent, cx, obj, argc, argv, rval);
    *rval = as_jsval(cx, true);
    return JS_TRUE;
}
static JSBool
stopPropagation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Stop event propagation in dom.");
    DOC_END;
    Method<dom::Event>::call(&dom::Event::stopPropagation, cx, obj, argc, argv, rval);
    *rval = as_jsval(cx, true);
    return JS_TRUE;
}
static JSBool
preventDefault(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prevent engaging of default action");
    DOC_END;
    Method<dom::Event>::call(&dom::Event::preventDefault, cx, obj, argc, argv, rval);
    *rval = as_jsval(cx, true);
    return JS_TRUE;
}

JSFunctionSpec *
JSEvent::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"initEvent",            initEvent,               3},
        {"stopPropagation",      stopPropagation,         0},
        {"preventDefault",       preventDefault,          0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSEvent::Properties() {
    static JSPropertySpec myProperties[] = {
        {"type", PROP_type, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"target", PROP_target, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"currentTarget", PROP_currentTarget, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"eventPhase", PROP_eventPhase, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bubbles", PROP_bubbles, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"cancelable", PROP_cancelable, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"timeStamp", PROP_timeStamp, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"isDefaultPrevented", PROP_isDefaultPrevented, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSEvent::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        "CAPTURING_PHASE", PROP_CAPTURING_PHASE,  dom::Event::CAPTURING_PHASE,
        "AT_TARGET",       PROP_AT_TARGET,        dom::Event::AT_TARGET,
        "BUBBLING_PHASE",  PROP_BUBBLING_PHASE,   dom::Event::BUBBLING_PHASE,
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSEvent::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSEvent::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSEvent::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_type:
                *vp = as_jsval(cx, getNative().type());
                return JS_TRUE;
            case PROP_target:
                *vp = as_jsval(cx, getNative().target());
                return JS_TRUE;
            case PROP_currentTarget:
                *vp = as_jsval(cx, getNative().currentTarget());
                return JS_TRUE;
            case PROP_eventPhase:
                *vp = as_jsval(cx, getNative().eventPhase());
                return JS_TRUE;
            case PROP_bubbles:
                *vp = as_jsval(cx, getNative().bubbles());
                return JS_TRUE;
            case PROP_cancelable:
                *vp = as_jsval(cx, getNative().cancelable());
                return JS_TRUE;
            case PROP_timeStamp:
                *vp = as_jsval(cx, getNative().timeStamp());
                return JS_TRUE;
            case PROP_isDefaultPrevented:
                *vp = as_jsval(cx, getNative().isDefaultPrevented());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSEvent::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSEvent::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    //switch (theID) {
        //case PROP_isOpen:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
      //  default:
            JS_ReportError(cx,"JSEvent::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    //}
}

/* new Event(string theType,
             object thePayload = 0,
             bool canBubbleArg = true,
             bool cancelableArg = true,
             double theTimeStamp = DOMTimeStamp() )
*/
JSBool
JSEvent::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a dom event.");
    DOC_PARAM("theEventIdentifier", "", DOC_TYPE_STRING);
    DOC_PARAM("thePayload", "", DOC_TYPE_OBJECT);
    DOC_PARAM("theCanBubbleFlag", "", DOC_TYPE_BOOLEAN);
    DOC_PARAM("theCancelableFlag", "", DOC_TYPE_BOOLEAN);
    DOC_PARAM("theTimeStamp", "", DOC_TYPE_OBJECT);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSEvent * myNewObject = 0;
    if (argc == 0 ) {
        OWNERPTR myNewEvent = OWNERPTR(new GenericJSEvent("default"));
        myNewObject = new JSEvent(myNewEvent, &(*myNewEvent));
    } else if (argc >0 && argc < 6) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSEvent::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        std::string myType = "";
        if (!convertFrom(cx, argv[0], myType)) {
            JS_ReportError(cx, "JSEvent::Constructor: argument #1 must be an string (type)");
            return JS_FALSE;
        }
        asl::Ptr<JSObject*> myObject(new JSObject*(0));
        if (argc > 1) {
            *myObject = JSVAL_TO_OBJECT(argv[1]);
        }
        bool canBubbleArg = true;
        if (argc > 2) {
            if (!convertFrom(cx, argv[2], canBubbleArg)) {
                JS_ReportError(cx, "JSEvent::Constructor: argument #3 must be a bool (canBubbleArg)");
                return JS_FALSE;
            }
        }
        bool cancelableArg = true;
        if (argc > 3) {
            if (!convertFrom(cx, argv[3], cancelableArg)) {
                JS_ReportError(cx, "JSEvent::Constructor: argument #4 must be a bool (cancelableArg)");
                return JS_FALSE;
            }
        }
        dom::DOMTimeStamp myTimeStamp;
        double myTime;
        if (argc > 4) {
            if (!convertFrom(cx, argv[4], myTime)) {
                myTimeStamp = myTime;
                JS_ReportError(cx, "JSEvent::Constructor: argument #5 must be a double (theTimeStamp)");
                return JS_FALSE;
            }
        }
        if (argc > 5) {
            JS_ReportError(cx, "JSEvent::Constructor: excess arguments ignored, max arguments = 5");
        }
        OWNERPTR myNewEvent = OWNERPTR(
            new GenericJSEvent(myType,
                     canBubbleArg,
                     cancelableArg,
                     myTimeStamp,
                     myObject)
                     );
        myNewObject = new JSEvent(myNewEvent, &(*myNewEvent));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1-5 (theType [, thePayload, canBubbleArg, cancelableArg, theTimeStamp]) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSEvent::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSEvent::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
                Properties(), Functions(), ConstIntProperties());
        createClassDocumentation(ClassName(), Properties(), Functions(),
                ConstIntProperties(), 0, 0, 0);
        return myClass;
}

//bool convertFrom(JSContext *cx, jsval theValue, JSEvent::NATIVE & theEvent) {
bool convertFrom(JSContext *cx, jsval theValue, dom::EventPtr & theEvent) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSEvent::NATIVE >::Class()) {
                theEvent = JSClassTraits<JSEvent::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}


//jsval as_jsval(JSContext *cx, JSEvent::OWNERPTR theOwner) {
jsval as_jsval(JSContext *cx, dom::EventPtr theOwner) {
    // TODO, generalize to dom::Events. Right now it only works for JSGenericEvents
    JSEvent::OWNERPTR myGenericEvent = dynamic_cast_Ptr<JSEvent::NATIVE>(theOwner);
    JSObject * myReturnObject = JSEvent::Construct(cx, myGenericEvent, &(*myGenericEvent));
    return OBJECT_TO_JSVAL(myReturnObject);
}

//jsval as_jsval(JSContext *cx, JSEvent::OWNERPTR theOwner, JSEvent::NATIVE * theEvent) {
jsval as_jsval(JSContext *cx, dom::EventPtr theOwner, JSEvent::NATIVE * theEvent) {
    JSObject * myObject = JSEvent::Construct(cx, theOwner, theEvent);
    return OBJECT_TO_JSVAL(myObject);
}

jsval as_jsval(JSContext *cx, dom::EventTargetPtr theOwner) {
    dom::NodePtr myNode = dynamic_cast_Ptr<dom::Node>(theOwner);
    if (myNode) {
        return as_jsval(cx, myNode);
    }
    AC_ERROR << "as_jsval(SContext *, dom::EventTargetPtr): not a Node" << endl;
    return JSVAL_VOID;
}

}
