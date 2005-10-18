//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSCTScan.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/31 13:12:12 $
//
//
//=============================================================================

#ifndef _ac_y60_components_jsctscan_h_
#define _ac_y60_components_jsctscan_h_

#include "CTScan.h"
#include <y60/JSWrapper.h>

namespace jslib {
    
class JSCTScan : public jslib::JSWrapper<y60::CTScan, asl::Ptr<y60::CTScan> , jslib::StaticAccessProtocol> {
        JSCTScan() {}
    public:
        typedef y60::CTScan NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSCTScan(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSCTScan() {};
        
        static const char * ClassName() {
            return "CTScan";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        enum PropertyNumbers {
            PROP_default_window = -100,
            PROP_length,
            PROP_signal_progress,
            PROP_IDENTITY,
            PROP_ARBITRARY,
            PROP_Y2Z,
            PROP_X2Z
        };
        static JSPropertySpec * Properties();

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

        static JSCTScan & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCTScan &>(JSCTScan::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSCTScan::NATIVE> 
    : public JSClassTraitsWrapper<JSCTScan::NATIVE, JSCTScan> {};


bool convertFrom(JSContext *cx, jsval theValue, JSCTScan::OWNERPTR & theCTScan);

jsval as_jsval(JSContext *cx, JSCTScan::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSCTScan::OWNERPTR theOwner, JSCTScan::NATIVE * theCTScan);

} // namespace jslib
#endif


