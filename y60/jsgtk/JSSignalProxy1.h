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
//
//   $RCSfile: JSSignalProxy1.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/04/13 12:12:27 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSIGNAL_PROXY_1_INCLUDED_
#define _Y60_ACGTKSHELL_JSSIGNAL_PROXY_1_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSSignal1.h"
// #include "GTKApp.h"
#include "JSSigConnection.h"

#include <y60/jsbase/JSWrapper.h>
#include <libglademm/xml.h>
#include <sigc++/slot.h>

#include <asl/base/string_functions.h>

namespace jslib {

template <class R, class P0>
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<Glib::SignalProxy1<R, P0> > & theOwner);

template <class R, class P0>
class JSSignalProxy1 : public JSWrapper<Glib::SignalProxy1<R, P0>, asl::Ptr<Glib::SignalProxy1<R, P0> >, StaticAccessProtocol>
{
        JSSignalProxy1() {}
    public:
        typedef Glib::SignalProxy1<R, P0> NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            static std::string myName(composeTypeName<R,P0>("SignalProxy1"));
            return myName.c_str();
        }

        static JSBool connect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
            try {
                ensureParamCount(argc, 2, 3);
                asl::Ptr<Glib::SignalProxy1<R, P0> > myOwner;
                convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

                JSObject * myTarget(0);
                convertFrom(cx, argv[0], myTarget);

                Glib::ustring myMethodName;
                convertFrom(cx, argv[1], myMethodName);

                bool myAfterFlag = false;
                if (argc > 2) {
                    convertFrom(cx, argv[2], myAfterFlag);
                }

                sigc::slot1<R, P0> mySlot = sigc::bind<JSContext*, JSObject*, Glib::ustring>(
                    sigc::ptr_fun( & JSSignalAdapter1<R, P0>::on_signal ), cx, myTarget, myMethodName);
                JSSigConnection::OWNERPTR myConnection = JSSigConnection::OWNERPTR(new sigc::connection);
                switch (argc) {
                    case 2 :
                        *myConnection = myOwner->connect(mySlot);
                        break;
                    case 3 :
                        *myConnection = myOwner->connect(mySlot, myAfterFlag);
                        break;
                }
                // register our target object with the GCObserver
                GCObserver::FinalizeSignal myFinalizer = GCObserver::get().watchObject(myTarget);
                // now add our cleanup code to the finalize signal,
                // binding the connection as an extra argument
                myFinalizer.connect(sigc::bind<sigc::connection, std::string>(
                            sigc::ptr_fun( & JSSignalAdapter1<R, P0>::on_target_finalized_dbg),
                            *myConnection, myMethodName));

                *rval = as_jsval(cx, myConnection, & ( * myConnection));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }

        static JSFunctionSpec * Functions() {
            static JSFunctionSpec myFunctions[] = {
                {"connect", connect, 3},
                {0}
            };
            return myFunctions;
        }

        virtual unsigned long length() const {
            return 1;
        }

        // virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        //virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
            if (JSA_GetClass(cx,obj) != Base::Class()) {
                JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
                return JS_FALSE;
            }

            NATIVE * newNative = 0;
            JSSignalProxy1<R, P0> * myNewObject = 0;

            if (argc == 0) {
                myNewObject = new JSSignalProxy1<R, P0>(OWNERPTR(newNative), newNative);
            } else {
                JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
                return JS_FALSE;
            }

            if (myNewObject) {
                JS_SetPrivate(cx,obj,myNewObject);
                return JS_TRUE;
            }
            JS_ReportError(cx,"JSWindow::Constructor: bad parameters");
            return JS_FALSE;
        }


        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner) {
            return Base::Construct(cx, theOwner, theOwner.get());
        }

        JSSignalProxy1(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
            JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
            if (myClassObject) {
                JSA_AddFunctions(cx, myClassObject, Functions());
            }
            return myClassObject;
        }

        static JSSignalProxy1<R, P0> & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSignalProxy1<R, P0> &>(JSSignalProxy1<R, P0>::getJSWrapper(cx,obj));
        }

    private:
        static JSBool
        toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            std::string myStringRep = std::string("SignalProxy1@") + asl::as_string(obj);
            *rval = as_jsval(cx, myStringRep);
            return JS_TRUE;
        }
};

#define DEFINE_SIGNALPROXY1_CLASS_TRAIT(R, P0) \
template <> \
struct JSClassTraits<JSSignalProxy1<R, P0>::NATIVE> \
    : public JSClassTraitsWrapper<JSSignalProxy1<R, P0>::NATIVE, JSSignalProxy1<R, P0> > {};

#define INIT_SIGNALPROXY1( RVAL, PARAM0 ) \
if (!JSSignalProxy1<RVAL, PARAM0>::initClass(cx, theGlobalObject)) { \
    return false; \
}

template <class R, class P0>
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<Glib::SignalProxy1<R, P0> > & theOwner) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) ==
                        JSClassTraits<typename JSSignalProxy1<R, P0>::NATIVE >::Class())
            {
                theOwner =
                    JSClassTraits<typename JSSignalProxy1<R, P0>::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}


template <class R, class P0>
jsval as_jsval(JSContext *cx, asl::Ptr<Glib::SignalProxy1<R, P0> > & theValue) {
    JSObject * myObject = JSSignalProxy1<R, P0>::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}

#endif


