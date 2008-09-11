//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_JSLIB_JSMODELLING_FUNCTIONS_INCLUDED_
#define _Y60_JSLIB_JSMODELLING_FUNCTIONS_INCLUDED_

#include <y60/jsbase/JSWrapper.h>

DEFINE_EXCEPTION(JSModellingFunctionException, asl::Exception);

namespace jslib {

typedef bool DummyT;
    
class JSModellingFunctions : public JSWrapper<DummyT,asl::Ptr<DummyT>, StaticAccessProtocol> {
public:
    typedef DummyT NATIVE;
    typedef asl::Ptr<DummyT> OWNERPTR;
    typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

    static const char * ClassName() {
        return "Modelling";
    }
    static JSFunctionSpec * Functions();
    static JSFunctionSpec * StaticFunctions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();

    virtual unsigned long length() const {
        return 0;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, asl::Ptr<DummyT> theDummy, DummyT * theNative) {
        return Base::Construct(cx, theDummy, theNative);
    }
    JSModellingFunctions(asl::Ptr<DummyT> theOwner, DummyT * theDummy)
        : Base(theOwner, theDummy)
    {AC_TRACE << "JSModellingFunctions()" << std::endl;}

    static void addClassProperties(JSContext * cx, JSObject * theClassProto);
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
private:
    JSModellingFunctions(); // hide default constructor
    JSModellingFunctions(const JSModellingFunctions &); // hide copy constructor
    JSModellingFunctions & operator=(const JSModellingFunctions &); // hide assignment operator
};

template <>
struct JSClassTraits<DummyT> : public JSClassTraitsWrapper<DummyT, JSModellingFunctions> {};

bool convertFrom(JSContext *cx, jsval theValue, DummyT *& theDummy);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<DummyT> & theDummy);

jsval as_jsval(JSContext *cx, JSModellingFunctions::OWNERPTR & theOwner);

}

#endif

