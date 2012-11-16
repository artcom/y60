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

#ifndef _Y60_ACGTKSHELL_JSSIGNAL_0_INCLUDED_
#define _Y60_ACGTKSHELL_JSSIGNAL_0_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSSignalProxyUtils.h"
#include "JSSigConnection.h"
#include "jsgtk.h"
#include <y60/acgtk/GCObserver.h>

#include <y60/jsbase/JSWrapper.h>
#include <libglademm/xml.h>
#include <sigc++/slot.h>

#include <asl/base/string_functions.h>

namespace jslib {

template <class R>
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<sigc::signal0<R> > & theOwner);

template <class R>
class JSSignalAdapter0 : public JSSignalAdapterBase {
    public:
        static R on_signal( JSContext * cx, JSObject * theJSObject, Glib::ustring theMethodName)
        {
            jsval rval;
            AC_TRACE << "JSSignalAdapter0 calling JS event handler '" << theMethodName << "'";
            JSBool ok = jslib::JSA_CallFunctionName(cx, theJSObject, theMethodName, 0, 0, &rval);
            (void)(ok); //XXX check if caller properly handles JS exception
            R myResult;
            convertFrom(cx, rval, myResult);
            return myResult;
        }
};

template <>
class JSSignalAdapter0<void> : public JSSignalAdapterBase {
    public:
        static void on_signal( JSContext * cx,  JSObject * theJSObject, Glib::ustring theMethodName)
        {
            AC_TRACE << "JSSignalAdapter0 calling JS event handler '" << theMethodName << "'";
            jsval rval;
            JSBool ok = jslib::JSA_CallFunctionName(cx, theJSObject, theMethodName, 0, 0, &rval);
            (void)(ok); //XXX check if caller properly handles JS exception
        }
};

template <class R>
std::string
composeTypeName(const char * theBasename) {
    return std::string(theBasename) + "_" + TypeNameTrait<R>::name() + "_";
}

template <class R>
class JSSignal0 : public JSWrapper<sigc::signal0<R>, asl::Ptr<sigc::signal0<R> >, StaticAccessProtocol>
{
        JSSignal0() {}
    public:
        typedef sigc::signal0<R> NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            static std::string myName(composeTypeName<R>("Signal0"));
            return myName.c_str();
        }

        static JSBool connect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
            try {
                ensureParamCount(argc, 2);
                OWNERPTR myOwner;
                convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

                JSObject * myTarget(0);
                convertFrom(cx, argv[0], myTarget);

                Glib::ustring myMethodName;
                convertFrom(cx, argv[1], myMethodName);

                sigc::slot0<R> mySlot = sigc::bind<JSContext*, JSObject*, Glib::ustring>(
                    sigc::ptr_fun( & JSSignalAdapter0<R>::on_signal ), cx, myTarget, myMethodName);
                JSSigConnection::OWNERPTR myConnection = JSSigConnection::OWNERPTR(new sigc::connection);
                AC_TRACE << "JSSignal0 connecting to '" << myMethodName << "'";

                *myConnection = myOwner->connect(mySlot);

                // register our target object with the GCObserver
                GCObserver::FinalizeSignal myFinalizer = GCObserver::get().watchObject(myTarget);
                // now add our cleanup code to the finalize signal,
                // binding the connection as an extra argument
                myFinalizer.connect(sigc::bind<sigc::connection>(
                            sigc::ptr_fun( & JSSignalAdapter0<R>::on_target_finalized),
                            *myConnection));

                *rval = as_jsval(cx, myConnection, & ( * myConnection));
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        }

        static JSFunctionSpec * Functions() {
            static JSFunctionSpec myFunctions[] = {
                {"connect", connect, 2},
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
            JSSignal0<R> * myNewObject = 0;

            if (argc == 0) {
                myNewObject = new JSSignal0<R>(OWNERPTR(newNative), newNative);
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

        JSSignal0(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
            JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
            if (myClassObject) {
                JSA_AddFunctions(cx, myClassObject, Functions());
            }
            return myClassObject;
        }

        static JSSignal0<R> & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSignal0<R> &>(JSSignal0<R>::getJSWrapper(cx,obj));
        }

    private:
        static JSBool
        toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("");
            DOC_END;
            std::string myStringRep = std::string("Signal0@") + asl::as_string(obj);
            *rval = as_jsval(cx, myStringRep);
            return JS_TRUE;
        }
};

#define DEFINE_SIGNAL0_CLASS_TRAIT(R) \
template <> \
struct JSClassTraits<JSSignal0<R>::NATIVE> \
    : public JSClassTraitsWrapper<JSSignal0<R>::NATIVE, JSSignal0<R> > {};

#define INIT_SIGNAL0( RVAL ) \
if (!JSSignal0<RVAL>::initClass(cx, theGlobalObject)) { \
    return false; \
}

template <class R>
jsval as_jsval(JSContext *cx, asl::Ptr<sigc::signal0<R> > & theValue) {
    JSObject * myObject = JSSignal0<R>::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}

#endif


