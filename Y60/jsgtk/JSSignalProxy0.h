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
//   $RCSfile: JSSignalProxy0.h,v $
//   $Author: david $
//   $Revision: 1.5 $
//   $Date: 2005/04/13 12:12:27 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSIGNAL_PROXY_0_INCLUDED_
#define _Y60_ACGTKSHELL_JSSIGNAL_PROXY_0_INCLUDED_

#include "JSSignal0.h"

#include "JSSignalProxyUtils.h"
// #include "GTKApp.h"
#include "JSSigConnection.h"

#include <y60/JSWrapper.h>
#include <libglademm/xml.h>
#include <sigc++/slot.h>

#include <asl/string_functions.h>

namespace jslib {

template <class R>
class JSSignalProxy0 : public JSWrapper<Glib::SignalProxy0<R>, asl::Ptr<Glib::SignalProxy0<R> >, StaticAccessProtocol>
{
        JSSignalProxy0() {}
    public:
        typedef Glib::SignalProxy0<R> NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            static std::string myName(composeTypeName<R>("SignalProxy0"));
            return myName.c_str();
        }

        static JSBool connect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
            try {
                ensureParamCount(argc, 2, 3);
                asl::Ptr<Glib::SignalProxy0<R> > myOwner;
                convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

                JSObject * myTarget(0);
                convertFrom(cx, argv[0], myTarget);

                Glib::ustring myMethodName;
                convertFrom(cx, argv[1], myMethodName);

                bool myAfterFlag;
                if (argc > 2) {
                    convertFrom(cx, argv[2], myAfterFlag);
                }

                SigC::Slot0<R> mySlot = sigc::bind<JSContext*, JSObject*, Glib::ustring>(
                    sigc::ptr_fun( & JSSignalAdapter0<R>::on_signal ), cx, myTarget, myMethodName);
                JSSigConnection::OWNERPTR myConnection = JSSigConnection::OWNERPTR( new sigc::connection);
                switch (argc) {
                    case 2 :
                        AC_TRACE << "JSSignalProxy0 connecting to '" << myMethodName << "'";
                        *myConnection = myOwner->connect(mySlot);
                        break;
                    case 3 :
                        AC_TRACE << "JSSignalProxy0 connecting to '" << myMethodName << "', after=" << myAfterFlag;
                        *myConnection = myOwner->connect(mySlot, myAfterFlag);
                        break;
                }
                // register our target object with the GCObserver
                GCObserver::FinalizeSignal myFinalizer = GCObserver::get().watchObject(myTarget);
                // now add our cleanup code to the finalize signal,
                // binding the connection as an extra argument
                myFinalizer.connect(sigc::bind<sigc::connection, std::string>(
                            sigc::ptr_fun( & JSSignalAdapterBase::on_target_finalized_dbg),
                            *myConnection, myMethodName.c_str()));

                // add return the connection to JS
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
            JSSignalProxy0<R> * myNewObject = 0;

            if (argc == 0) {
                myNewObject = new JSSignalProxy0<R>(OWNERPTR(newNative), newNative);
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
            return Base::Construct(cx, theOwner, &(*theOwner));
        }

        JSSignalProxy0(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
            JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
            if (myClassObject) {
                JSA_AddFunctions(cx, myClassObject, Functions());
            }
            return myClassObject;
        }

        static JSSignalProxy0<R> & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSignalProxy0<R> &>(JSSignalProxy0<R>::getJSWrapper(cx,obj));
        }

    private:
        static JSBool
        toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
            std::string myStringRep = std::string("SignalProxy0@") + asl::as_string(obj);
            JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
            *rval = STRING_TO_JSVAL(myString);
            return JS_TRUE;
        }
};

#define DEFINE_SIGNALPROXY0_CLASS_TRAIT(R) \
template <> \
struct JSClassTraits<JSSignalProxy0<R>::NATIVE> \
    : public JSClassTraitsWrapper<JSSignalProxy0<R>::NATIVE, JSSignalProxy0<R> > {};


#define INIT_SIGNALPROXY0( RVAL ) \
if (!JSSignalProxy0<RVAL>::initClass(cx, theGlobalObject)) { \
    return false; \
}

template <class R>
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<Glib::SignalProxy0<R> > & theOwner) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) ==
                        JSClassTraits<typename JSSignalProxy0<R>::NATIVE >::Class())
            {
                theOwner =
                    JSClassTraits<typename JSSignalProxy0<R>::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

template <class R>
jsval as_jsval(JSContext *cx, asl::Ptr<Glib::SignalProxy0<R> > & theValue) {
    JSObject * myObject = JSSignalProxy0<R>::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}

#endif


