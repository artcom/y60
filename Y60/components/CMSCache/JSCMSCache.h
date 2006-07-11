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
//   $RCSfile: JSCMSCache.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/31 13:12:12 $
//
//
//=============================================================================

#ifndef _ac_y60_components_jsCMSCache_h_
#define _ac_y60_components_jsCMSCache_h_

#include "CMSCache.h"
#include <y60/JSWrapper.h>

namespace jslib {
    
class JSCMSCache : public jslib::JSWrapper<y60::CMSCache, asl::Ptr<y60::CMSCache> , jslib::StaticAccessProtocol> {
        JSCMSCache() {}
    public:
        typedef y60::CMSCache NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSCMSCache(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSCMSCache() {};
        
        static const char * ClassName() {
            return "CMSCache";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        enum PropertyNumbers {
            PROP_statusReport =  -100,
            PROP_verbose,
            PROP_END
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

        static JSCMSCache & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCMSCache &>(JSCMSCache::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSCMSCache::NATIVE> 
    : public JSClassTraitsWrapper<JSCMSCache::NATIVE, JSCMSCache> {};


bool convertFrom(JSContext *cx, jsval theValue, JSCMSCache::OWNERPTR & theCMSCache);

jsval as_jsval(JSContext *cx, JSCMSCache::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSCMSCache::OWNERPTR theOwner, JSCMSCache::NATIVE * theCMSCache);

} // namespace jslib
#endif


