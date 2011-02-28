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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "JSDomEvent.h"

#include "JSNode.h"
#include "JScppUtils.h"
#include "JSWrapper.impl"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<GenericJSDomEvent, asl::Ptr<GenericJSDomEvent, dom::ThreadingModel>,
         StaticAccessProtocol>;

JSDomEventListener::JSDomEventListener(JSContext * theContext, JSObject * theEventListener, const std::string & theMethodName)
    : _myJSContext(theContext), _myEventListener(theEventListener), _myMethodName(theMethodName)
{
    AC_TRACE << "JSDomEventListener()";
    if(_myEventListener) {
        if (!JS_AddRoot(_myJSContext, &_myEventListener)) {
            // We might at one day remove this warning when we use non-js event listeners
            AC_WARNING << "JSDomEventListener(): could not root event listener objects";
        };
    }
}

JSDomEventListener::~JSDomEventListener() {
    AC_TRACE<< " ~JSDomEventListener()";
    if(_myEventListener) {
        JS_RemoveRoot(_myJSContext, &_myEventListener);
    }
}
void
JSDomEventListener::handleEvent(dom::EventPtr theEvent) {
    asl::Ptr<GenericJSDomEvent, dom::ThreadingModel> myGenericEvent = dynamic_cast_Ptr<GenericJSDomEvent>(theEvent);
    if (myGenericEvent) {
        jsval argv[1], rval;
        argv[0] = as_jsval(_myJSContext, theEvent);
        /*JSBool ok =*/ JSA_CallFunctionName(_myJSContext, _myEventListener, _myMethodName.c_str(), 1, argv, &rval);
    } else {
        AC_ERROR << "JSDomEventListener::handleEvent: not a GenericJSDomEvent" << std::endl;
    }
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the type of the native event");
    DOC_RVAL("The string", DOC_TYPE_STRING);
    DOC_END;
    JSDomEvent::Base * myWrapperPtr = JSDomEvent::getJSWrapperPtr(cx,obj);
    if (myWrapperPtr) {
        std::string myStringRep = std::string("Event<")+myWrapperPtr->getNative().type()+">";
#if 0
        GenericJSDomEvent myEvent = myWrapperPtr->getNative();
        std::string myStringRep = std::string("{")+
                                    "type: '" + myEvent.type() + "', " +
                                    "eventPhase: '" + myEvent.eventPhase() +
                                    "bubbles: '" + myEvent.bubbles() +
                                    "cancelable: '" + myEvent.cancelable() +
                                    "isDefaultPrevented: '" + myEvent.isDefaultPrevented() +
                                    "payload: " + myEvent.getPayload() +
                                    "}"
#endif
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
    DOC_PARAM("theTargetOnlyFlag", "", DOC_TYPE_BOOLEAN);
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
JSDomEvent::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"initEvent",            initEvent,               4},
        {"stopPropagation",      stopPropagation,         0},
        {"preventDefault",       preventDefault,          0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSDomEvent::Properties() {
    static JSPropertySpec myProperties[] = {
        {"type", PROP_type, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"target", PROP_target, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"currentTarget", PROP_currentTarget, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"eventPhase", PROP_eventPhase, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bubbles", PROP_bubbles, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"targetOnly", PROP_targetOnly, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"cancelable", PROP_cancelable, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"timeStamp", PROP_timeStamp, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"isDefaultPrevented", PROP_isDefaultPrevented, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"data", PROP_data, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSDomEvent::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {"CAPTURING_PHASE", PROP_CAPTURING_PHASE,  dom::Event::CAPTURING_PHASE},
        {"AT_TARGET",       PROP_AT_TARGET,        dom::Event::AT_TARGET},
        {"BUBBLING_PHASE",  PROP_BUBBLING_PHASE,   dom::Event::BUBBLING_PHASE},
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSDomEvent::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSDomEvent::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSDomEvent::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
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
            case PROP_targetOnly:
                *vp = as_jsval(cx, getNative().targetOnly());
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
            case PROP_data:
                *vp = OBJECT_TO_JSVAL(*getNative().getPayload());
                return JS_TRUE;
             default:
                JS_ReportError(cx,"JSDomEvent::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSDomEvent::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    //switch (theID) {
        //case PROP_isOpen:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
      //  default:
            //JS_ReportError(cx,"JSDomEvent::setPropertySwitch: index %d out of range", theID);
            //return JS_FALSE;
    //}
}

/* new Event(string theType,
             object thePayload = 0,
             bool canBubbleArg = true,
             bool cancelableArg = true,
             double theTimeStamp = DOMTimeStamp() )
*/
JSBool
JSDomEvent::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a dom event.");
    DOC_PARAM("theEventIdentifier", "", DOC_TYPE_STRING);
    DOC_PARAM_OPT("thePayload", "", DOC_TYPE_OBJECT, "undefined");
    DOC_PARAM_OPT("theCanBubbleFlag", "", DOC_TYPE_BOOLEAN, "false");
    DOC_PARAM_OPT("theCancelableFlag", "", DOC_TYPE_BOOLEAN, "false");
    DOC_PARAM_OPT("theTargetOnlyFlag", "", DOC_TYPE_BOOLEAN, "true");
    DOC_PARAM_OPT("theTimeStamp", "", DOC_TYPE_OBJECT, "<current time>");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSDomEvent * myNewObject = 0;
    if (argc == 0 ) {
        OWNERPTR myNewEvent = OWNERPTR(new GenericJSDomEvent("default"));
        myNewObject = new JSDomEvent(myNewEvent, myNewEvent.get());
    } else if (argc >0 && argc < 7) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSDomEvent::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        std::string myType = "";
        if (!convertFrom(cx, argv[0], myType)) {
            JS_ReportError(cx, "JSDomEvent::Constructor: argument #1 must be an string (type)");
            return JS_FALSE;
        }
        asl::Ptr<JSObject*> myObject(new JSObject*(0));
        if (argc > 1) {
            *myObject = JSVAL_TO_OBJECT(argv[1]);
        }
        bool canBubbleArg = true;
        if (argc > 2) {
            if (!convertFrom(cx, argv[2], canBubbleArg)) {
                JS_ReportError(cx, "JSDomEvent::Constructor: argument #3 must be a bool (canBubbleArg)");
                return JS_FALSE;
            }
        }
        bool cancelableArg = true;
        if (argc > 3) {
            if (!convertFrom(cx, argv[3], cancelableArg)) {
                JS_ReportError(cx, "JSDomEvent::Constructor: argument #4 must be a bool (cancelableArg)");
                return JS_FALSE;
            }
        }
        bool targetOnly = false;
        if (argc > 4) {
            if (!convertFrom(cx, argv[4], targetOnly)) {
                JS_ReportError(cx, "JSDomEvent::Constructor: argument #5 must be a bool (targetOnly)");
                return JS_FALSE;
            }
        }
        dom::DOMTimeStamp myTimeStamp;
        double myTime;
        if (argc > 5) {
            if (!convertFrom(cx, argv[5], myTime)) {
                myTimeStamp = myTime;
                JS_ReportError(cx, "JSDomEvent::Constructor: argument #6 must be a double (theTimeStamp)");
                return JS_FALSE;
            }
        }
        if (argc > 5) {
            JS_ReportError(cx, "JSDomEvent::Constructor: excess arguments ignored, max arguments = 5");
        }
        OWNERPTR myNewEvent = OWNERPTR(
            new GenericJSDomEvent(myType,
                     canBubbleArg,
                     cancelableArg,
                     targetOnly,
                     myTimeStamp,
                     myObject)
                     );
        myNewObject = new JSDomEvent(myNewEvent, myNewEvent.get());
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1-6 args (theType [, thePayload, canBubbleArg, cancelableArg, targetOnly, theTimeStamp]) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSDomEvent::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSDomEvent::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
                Properties(), Functions(), ConstIntProperties());
        createClassDocumentation(ClassName(), Properties(), Functions(),
                ConstIntProperties(), 0, 0, 0);
        return myClass;
}

//bool convertFrom(JSContext *cx, jsval theValue, JSDomEvent::NATIVE & theEvent) {
bool convertFrom(JSContext *cx, jsval theValue, dom::EventPtr & theEvent) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSDomEvent::NATIVE >::Class()) {
                theEvent = JSClassTraits<JSDomEvent::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}


//jsval as_jsval(JSContext *cx, JSDomEvent::OWNERPTR theOwner) {
jsval as_jsval(JSContext *cx, dom::EventPtr theOwner) {
    // TODO, generalize to dom::Events. Right now it only works for JSGenericEvents
    JSDomEvent::OWNERPTR myGenericEvent = dynamic_cast_Ptr<JSDomEvent::NATIVE>(theOwner);
    JSObject * myReturnObject = JSDomEvent::Construct(cx, myGenericEvent, myGenericEvent.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

//jsval as_jsval(JSContext *cx, JSDomEvent::OWNERPTR theOwner, JSDomEvent::NATIVE * theEvent) {
jsval as_jsval(JSContext *cx, dom::EventPtr theOwner, JSDomEvent::NATIVE * theEvent) {
    JSObject * myObject = JSDomEvent::Construct(cx, theOwner, theEvent);
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
