//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_StlImporter_JSStlCodec_h_
#define _ac_StlImporter_JSStlCodec_h_

#include <y60/JSWrapper.h>
#include "StlCodec.h"

namespace jslib {
/**
 * 
 * @ingroup Y60componentsStlImporter
 */ 
class JSStlCodec : public jslib::JSWrapper<y60::StlCodec, asl::Ptr<y60::StlCodec>, jslib::StaticAccessProtocol> {
public:
public:
    typedef y60::StlCodec NATIVE;
    typedef asl::Ptr<NATIVE> OWNERPTR;
    typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

    JSStlCodec(OWNERPTR theOwner, NATIVE * theNative)
        : Base(theOwner, theNative)
    {}

    virtual ~JSStlCodec() {};

    static const char * ClassName() {
        return "StlCodec";
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

        static JSStlCodec & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSStlCodec &>(JSStlCodec::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSStlCodec::NATIVE> 
    : public JSClassTraitsWrapper<JSStlCodec::NATIVE, JSStlCodec> {};


bool convertFrom(JSContext *cx, jsval theValue, JSStlCodec::OWNERPTR & theStlCodec);

jsval as_jsval(JSContext *cx, JSStlCodec::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSStlCodec::OWNERPTR theOwner, JSStlCodec::NATIVE * theStlCodec);

} // jslib

#endif // _ac_StlImporter_JSStlCodec_h_
