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
//   $RCSfile: JSSignalProxy1.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/04/13 12:12:27 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSIGNAL_3_INCLUDED_
#define _Y60_ACGTKSHELL_JSSIGNAL_3_INCLUDED_

#include "JSSignalProxyUtils.h"
#include "JSSigConnection.h"
#include "jsgtk.h"
#include <acgtk/GCObserver.h>
#include <y60/JSWrapper.h>
#include <libglademm/xml.h>
#include <sigc++/slot.h>
#include <sigc++/connection.h>

#include <asl/string_functions.h>

namespace jslib {

template <class R, class P0, class P1, class P2>
class JSSignalAdapter3 : public JSSignalAdapterBase {
    public:
        static R on_signal(P0 theParam0, P1 theParam1, P2 theParam2,
                           JSContext * cx, JSObject * theJSObject, Glib::ustring theMethodName)
        {
            // call the function
            jsval argv[3], rval;
            argv[0] = as_jsval(cx, theParam0);
            argv[1] = as_jsval(cx, theParam1);
            argv[2] = as_jsval(cx, theParam2);
            AC_TRACE << "calling signal3 " << theMethodName;
            JSBool ok = jslib::JSA_CallFunctionName(cx, theJSObject, theMethodName, 3, argv, &rval);

            R myResult;
            convertFrom(cx, rval, myResult);
            return myResult;
        }
};

template <class P0, class P1, class P2>
class JSSignalAdapter3<void, P0, P1, P2> : public JSSignalAdapterBase {
    public:
        static void on_signal(P0 theParam0, P1 theParam1, P2 theParam2,
                              JSContext * cx,  JSObject * theJSObject, Glib::ustring theMethodName)
        {
            // call the function
            jsval argv[3], rval;
            argv[0] = as_jsval(cx, theParam0);
            argv[1] = as_jsval(cx, theParam1);
            argv[2] = as_jsval(cx, theParam2);
            JSBool ok = jslib::JSA_CallFunctionName(cx, theJSObject, theMethodName, 3, argv, &rval);
        }
};

template <class R, class P0, class P1, class P2>
std::string
composeTypeName(const char * theBasename) {
    return std::string(theBasename) + "_" + TypeNameTrait<R>::name() + "_" +
                                            TypeNameTrait<P0>::name() + "_" +
                                            TypeNameTrait<P1>::name() + "_" +
                                            TypeNameTrait<P2>::name() + "_";
}

template <class R, class P0, class P1, class P2>
class JSSignal3 : public JSWrapper<sigc::signal3<R, P0, P1, P2>, asl::Ptr<sigc::signal3<R, P0, P1, P2> >, StaticAccessProtocol>
{
        JSSignal3() {}
    public:
        typedef sigc::signal3<R, P0, P1, P2> NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            static std::string myName(composeTypeName<R,P0,P1,P2>("Signal3"));
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

                SigC::Slot3<R, P0, P1, P2> mySlot = sigc::bind<JSContext*, JSObject*, Glib::ustring>(
                    sigc::ptr_fun( & JSSignalAdapter3<R, P0, P1, P2>::on_signal ), cx, myTarget, myMethodName);
                JSSigConnection::OWNERPTR myConnection = JSSigConnection::OWNERPTR(new SigC::Connection);

                *myConnection = myOwner->connect(mySlot);
                
                // register our target object with the GCObserver
                GCObserver::FinalizeSignal myFinalizer = GCObserver::get().watchObject(myTarget);
                // now add our cleanup code to the finalize signal,
                // binding the connection as an extra argument
                myFinalizer.connect(sigc::bind<sigc::connection>(
                            sigc::ptr_fun( & JSSignalAdapter3<R, P0, P1, P2>::on_target_finalized),
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
            JSSignal3<R, P0, P1, P2> * myNewObject = 0;

            if (argc == 0) {
                myNewObject = new JSSignal3<R, P0, P1, P2>(OWNERPTR(newNative), newNative);
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

        JSSignal3(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
            JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
            if (myClassObject) {
                JSA_AddFunctions(cx, myClassObject, Functions());
            }
            return myClassObject;
        }

        static JSSignal3<R, P0, P1, P2> & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSignal3<R, P0, P1, P2> &>(JSSignal3<R, P0, P1, P2>::getJSWrapper(cx,obj));
        }

    private:
        static JSBool
        toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
            std::string myStringRep = std::string("Signal3@") + asl::as_string(obj);
            JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
            *rval = STRING_TO_JSVAL(myString);
            return JS_TRUE;
        }
};

#define DEFINE_SIGNAL3_CLASS_TRAIT(R, P0, P1, P2) \
template <> \
struct JSClassTraits<JSSignal3<R, P0, P1, P2>::NATIVE> \
    : public JSClassTraitsWrapper<JSSignal3<R, P0, P1, P2>::NATIVE, JSSignal3<R, P0, P1, P2> > {};

#define INIT_SIGNAL3( RVAL, PARAM0, PARAM1, PARAM2 ) \
if (!JSSignal3<RVAL, PARAM0, PARAM1, PARAM2>::initClass(cx, theGlobalObject)) { \
    return false; \
}

template <class R, class P0, class P1, class P2>
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<sigc::signal3<R, P0, P1, P2> > & theOwner) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) ==
                        JSClassTraits<typename JSSignal3<R, P0, P1, P2>::NATIVE >::Class())
            {
                theOwner =
                    JSClassTraits<typename JSSignal3<R, P0, P1, P2>::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}


template <class R, class P0, class P1, class P2>
jsval as_jsval(JSContext *cx, asl::Ptr<sigc::signal3<R, P0, P1, P2> > & theValue) {
    JSObject * myObject = JSSignal3<R, P0, P1, P2>::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

}

#endif


