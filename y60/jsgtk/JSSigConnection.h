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

#ifndef _Y60_ACGTKSHELL_JS_SIGNAL_CONNECTION_INCLUDED_
#define _Y60_ACGTKSHELL_JS_SIGNAL_CONNECTION_INCLUDED_

#include "y60_jsgtk_settings.h"

#include <y60/jsbase/JSWrapper.h>

#ifdef OSX
//namespace sigc { typedef ::sigc::connection Connection; }
#endif

#include <sigc++/connection.h>

namespace jslib {

class JSSigConnection : public JSWrapper<sigc::connection, asl::Ptr<sigc::connection>, StaticAccessProtocol> {
        JSSigConnection();  // hide default constructor
    public:
        virtual ~JSSigConnection() {
        }
        typedef sigc::connection NATIVE;
        typedef asl::Ptr<sigc::connection> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "sigconnection";
        };
        static JSFunctionSpec * Functions();

        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }
        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSSigConnection(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
       //  static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        // getproperty handling
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSSigConnection & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSigConnection &>(JSSigConnection::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSSigConnection::NATIVE>
    : public JSClassTraitsWrapper<JSSigConnection::NATIVE, JSSigConnection> {};

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSSigConnection::OWNERPTR theOwner, JSSigConnection::NATIVE * theNative);

Y60_JSGTK_DECL bool convertFrom(JSContext *cx, jsval theValue, JSSigConnection::NATIVE * & theConnection);

} // namespace

#endif


