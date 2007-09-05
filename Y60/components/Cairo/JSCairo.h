//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#ifndef _ac_jslib_JSCairo_h_
#define _ac_jslib_JSCairo_h_

#include <y60/JSWrapper.h>
#include "Cairo.h"

class JSCairo : public jslib::JSWrapper<Cairo, asl::Ptr<Cairo> , jslib::StaticAccessProtocol> {
    JSCairo() {}
    
public:
    typedef Cairo NATIVE;
    typedef asl::Ptr<NATIVE> OWNERPTR;
    typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;
    
    JSCairo(OWNERPTR theOwner, NATIVE * theNative)
        : Base(theOwner, theNative)
        {}
    
    virtual ~JSCairo();
    
    static const char * ClassName() {
        return "Cairo";
    }
    
    static JSFunctionSpec * Functions();
    
    enum PropertyNumbers {
    };
    
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
    
    static JSCairo & getObject(JSContext *cx, JSObject * obj) {
        return dynamic_cast<JSCairo &>(JSCairo::getJSWrapper(cx,obj));
    }
};

namespace jslib {
    template <>
    struct JSClassTraits<JSCairo::NATIVE>
        : public JSClassTraitsWrapper<JSCairo::NATIVE, JSCairo> {};
    
} // namespace jslib

jsval as_jsval(JSContext *cx, JSCairo::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSCairo::OWNERPTR theOwner, JSCairo::NATIVE * theSerial);

#endif

