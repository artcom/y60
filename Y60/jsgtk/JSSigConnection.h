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
//   $RCSfile: JSSigConnection.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/04/13 12:12:27 $
//
//
//=============================================================================
#ifndef _Y60_ACGTKSHELL_JS_SIGNAL_CONNECTION_INCLUDED_
#define _Y60_ACGTKSHELL_JS_SIGNAL_CONNECTION_INCLUDED_

#include <y60/JSWrapper.h>
#include <sigc++/connection.h>

namespace jslib {

class JSSigConnection : public JSWrapper<SigC::Connection, asl::Ptr<SigC::Connection>, StaticAccessProtocol> {
        JSSigConnection();  // hide default constructor
    public:
        virtual ~JSSigConnection() {
        }
        typedef SigC::Connection NATIVE;
        typedef asl::Ptr<SigC::Connection> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "SigConnection";
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

jsval as_jsval(JSContext *cx, JSSigConnection::OWNERPTR theOwner, JSSigConnection::NATIVE * theNative);

bool convertFrom(JSContext *cx, jsval theValue, JSSigConnection::NATIVE * & theConnection);

} // namespace

#endif


