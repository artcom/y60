//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_jslib_JSOscSender_h_
#define _ac_jslib_JSOscSender_h_

#include <y60/jsbase/JSWrapper.h>

#include "OscSender.h"

class JSOscSender : public jslib::JSWrapper<y60::OscSender, asl::Ptr<y60::OscSender> , jslib::StaticAccessProtocol> {
        JSOscSender() {}
    public:
        typedef y60::OscSender NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSOscSender(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSOscSender();

        static const char * ClassName() {
            return "OscSender";
        }
        static JSFunctionSpec * Functions();

        static JSPropertySpec * Properties();

        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSOscSender & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSOscSender &>(JSOscSender::getJSWrapper(cx,obj));
        }
};

namespace jslib {
template <>
struct JSClassTraits<JSOscSender::NATIVE>
    : public JSClassTraitsWrapper<JSOscSender::NATIVE , JSOscSender> {};

} // namespace jslib

bool convertFrom(JSContext *cx, jsval theValue, JSOscSender::NATIVE & theNative);
bool convertFrom(JSContext *cx, jsval theValue, JSOscSender::OWNERPTR & theNativePtr);

jsval as_jsval(JSContext *cx, JSOscSender::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSOscSender::OWNERPTR theOwner, JSOscSender::NATIVE * theSerial);

#endif

