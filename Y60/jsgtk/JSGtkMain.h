//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSGTKSINGLETON_INCLUDED_
#define _Y60_ACGTKSHELL_JSGTKSINGLETON_INCLUDED_

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4512 4413 4244 4251)
#endif //defined(_MSC_VER)
#include <gtkmm/main.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/ipc/MessageAcceptor.h>
#include <asl/ipc/LocalPolicy.h>
#include <asl/base/string_functions.h>

namespace jslib {

class JSGtkMain : public JSWrapper<int, asl::Ptr<int>, StaticAccessProtocol>
{
        JSGtkMain() {}
    public:
        typedef int NATIVE;
        typedef asl::Ptr<int> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "GtkMain";
        }

        virtual unsigned long length() const {
            return 1;
        }

        // virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        //virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSGtkMain(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSFunctionSpec * StaticFunctions();
        static JSPropertySpec * StaticProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSGtkMain & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGtkMain &>(JSGtkMain::getJSWrapper(cx,obj));
        }

        static bool on_timeout( JSContext * cx, JSObject * theJSObject, std::string theMethodName);
        static int on_idle( JSContext * cx, JSObject * theJSObject, std::string theMethodName);

        // non-gtk extensions (single application instance support)
        static asl::Ptr<asl::MessageAcceptor<asl::LocalPolicy> > ourAppAcceptor;
        static sigc::connection ourAcceptorTimeout;
        typedef sigc::signal<void, const std::string &> OtherInstanceSignal;
        static OtherInstanceSignal ourOtherInstanceSignal; 
        static bool onAcceptorTimeout();
        static bool sendToPrevInstance(const std::string & theSessionPipeId, const std::string & theInitialProjectfilename);
};


template <>
struct JSClassTraits<JSGtkMain::NATIVE> 
    : public JSClassTraitsWrapper<JSGtkMain::NATIVE, JSGtkMain> {};

jsval as_jsval(JSContext *cx, JSGtkMain::OWNERPTR theOwner);
//jsval as_jsval(JSContext *cx, JSGtkMain::OWNERPTR theOwner, JSGtkMain::NATIVE * theSerial);

}

#endif



