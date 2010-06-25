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

#ifndef _Y60_ACGTKSHELL_JSGTKSINGLETON_INCLUDED_
#define _Y60_ACGTKSHELL_JSGTKSINGLETON_INCLUDED_

#include "y60_jsgtk_settings.h"

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
    #pragma warning(disable:4512 4413 4244 4251 4250)
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

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSGtkMain::OWNERPTR theOwner);
//jsval as_jsval(JSContext *cx, JSGtkMain::OWNERPTR theOwner, JSGtkMain::NATIVE * theSerial);

}

#endif



