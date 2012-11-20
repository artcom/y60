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
        return !JS_IsExceptionPending(cx); // propagate js exception when js code was executed from the native function;

    }

    void JSEventLoop::go( JSContext *cx, JSObject *obj ) {

        AC_INFO << "JSEventLoop::go!";

        _myJSContext = cx;
        _myEventListener = obj;
        asl::Time myStartTime;

        y60::EventDispatcher::get().addSink(this);

        for(;;) {

            y60::EventDispatcher::get().dispatch();

            if (JSA_hasFunction( _myJSContext, _myEventListener, "onFrame" ) ) {
                jsval arg, rval;
                asl::Time myCurrentTime;
                arg = as_jsval( _myJSContext,
                                (time_t)(myCurrentTime.millis() - myStartTime.millis())
                                / 1000 );
                if (!JSA_CallFunctionName( _myJSContext, _myEventListener, "onFrame", 1, &arg, &rval )) {
                    return;
                }
            }

            asl::msleep(10);

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






