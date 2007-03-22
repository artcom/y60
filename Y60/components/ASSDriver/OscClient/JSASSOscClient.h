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
//   $RCSfile: JSASSOscClient.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/31 13:12:12 $
//
//
//=============================================================================

#ifndef _ac_y60_components_jsctscan_h_
#define _ac_y60_components_jsctscan_h_

#include "ASSOscClient.h"
#include <y60/JSWrapper.h>

namespace jslib {
    
class JSASSOscClient : public jslib::JSWrapper<y60::ASSOscClient,
        asl::Ptr<y60::ASSOscClient> , jslib::StaticAccessProtocol> 
{
        JSASSOscClient() {}
    public:
        typedef y60::ASSOscClient NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSASSOscClient(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSASSOscClient() {};
        
        static const char * ClassName() {
            return "ASSOscClient";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        /*
        enum PropertyNumbers {
        };
        */
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

        static JSASSOscClient & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSASSOscClient &>(JSASSOscClient::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSASSOscClient::NATIVE> 
    : public JSClassTraitsWrapper<JSASSOscClient::NATIVE, JSASSOscClient> {};


bool convertFrom(JSContext *cx, jsval theValue, JSASSOscClient::OWNERPTR & theASSOscClient);

jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner, JSASSOscClient::NATIVE * theASSOscClient);

} // namespace jslib
#endif


