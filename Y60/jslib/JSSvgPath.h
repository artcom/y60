//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_JSSVGPATH_H_
#define _Y60_JSSVGPATH_H_

#include "JSWrapper.h"

#include <asl/SvgPath.h>
#include <dom/Value.h>

namespace jslib {

class JSSvgPath : public JSWrapper<asl::SvgPath, asl::Ptr<asl::SvgPath>, StaticAccessProtocol>
{
    JSSvgPath() {}

public:
    typedef asl::SvgPath NATIVE;
    typedef asl::Ptr<NATIVE> OWNERPTR;
    typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

    static const char * ClassName() {
        return "SvgPath";
    };

    enum PropertyNumbers {
        PROP_length = -100,
        PROP_numelements,
        PROP_segmentlength,
        PROP_numsegments,
    };

    static JSConstIntPropertySpec * ConstIntProperties();
    static JSFunctionSpec * StaticFunctions();
    static JSFunctionSpec * Functions();
    static JSPropertySpec * StaticProperties();
    static JSPropertySpec * Properties();

    virtual unsigned long length() const {
        return 1;
    }

    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
        return Base::Construct(cx, theOwner, theNative);
    }

    JSSvgPath(OWNERPTR theOwner, NATIVE * theNative) : Base(theOwner, theNative) {
    }

    static JSObject * initClass(JSContext *cx, JSObject * theGlobalObject);
};

template <>
struct JSClassTraits<asl::SvgPath>
    : public JSClassTraitsWrapper<asl::SvgPath, JSSvgPath> 
{
};

bool convertFrom(JSContext * cx, jsval theValue, asl::SvgPath *& theSvgPath);
bool convertFrom(JSContext * cx, jsval theValue, asl::SvgPath & theSvgPath);

jsval as_jsval(JSContext * cx, JSSvgPath::OWNERPTR theOwner);
jsval as_jsval(JSContext * cx, JSSvgPath::OWNERPTR theOwner, JSSvgPath::NATIVE * theSvgPath);

} // namespace

#endif
