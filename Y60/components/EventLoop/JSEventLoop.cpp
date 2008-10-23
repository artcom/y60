//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSEventLoop.h"

#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/input/EventDispatcher.h>
#include <y60/input/GenericEvent.h>
#include <y60/jsbase/JSNode.h>
#include <asl/base/Time.h>

namespace jslib {

    static JSBool Go( JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval) {
        DOC_BEGIN("");
        DOC_END;
        
        asl::Ptr<JSEventLoop> myNativePtr = getNativeAs<JSEventLoop>( cx, obj );

        JSObject * myObject = obj;
        if (argc == 1) {
            myObject = JSVAL_TO_OBJECT(argv[0]);
        }

        myNativePtr->go( cx, myObject );
        return JS_TRUE;

    }

    void JSEventLoop::go( JSContext *cx, JSObject *obj ) {

        AC_PRINT << "JSEventLoop::go!";
        
        _myJSContext = cx;
        _myEventListener = obj;
        asl::Time myStartTime;

        y60::EventDispatcher::get().addSink(this);

        while (true) {
            
            y60::EventDispatcher::get().dispatch();
            
            if (JSA_hasFunction( _myJSContext, _myEventListener, "onFrame" ) ) {
                jsval arg, rval;
                asl::Time myCurrentTime;
                arg = as_jsval( _myJSContext, 
                                (time_t)(myCurrentTime.millis() - myStartTime.millis())
                                / 1000 );
                JSA_CallFunctionName( _myJSContext, _myEventListener, 
                                      "onFrame", 1, &arg, &rval );
            }
        }

    }


    JSFunctionSpec * JSEventLoop::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {
            {0}
        };
        return myFunctions;
    }


    JSFunctionSpec * JSEventLoop::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"go",  Go,    0},
            {0}
        };
        return myFunctions;
    }


    void JSEventLoop::handle( y60::EventPtr theEvent ) {

        if (theEvent->type == y60::Event::GENERIC) {

            y60::GenericEvent & myEvent = dynamic_cast<y60::GenericEvent&>( *theEvent );
            dom::NodePtr myEventNode = myEvent.asNode();

            std::string myCallback( myEventNode->getAttribute("callback") ? 
                                    myEventNode->getAttributeValue<std::string>("callback"): 
                                    "onEvent" );
            if (_myEventListener) {
                if (JSA_hasFunction( _myJSContext, _myEventListener, myCallback.c_str()) ) {
                    jsval arg, rval;
                    arg = as_jsval( _myJSContext, myEvent.asNode() );
                    JSA_CallFunctionName( _myJSContext, _myEventListener, 
                                          myCallback.c_str(), 1, &arg, &rval );
                } else {
                    AC_WARNING << "Generic event callback '" << myCallback 
                               << "' is missing.";
                }
            }
        }
    }

    extern "C"
    EXPORT asl::PlugInBase* EventLoop_instantiatePlugIn( asl::DLHandle myDLHandle ) {
        return new JSEventLoop( myDLHandle );
    }

} // namespace jslib






