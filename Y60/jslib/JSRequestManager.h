//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSREQUEST_MANAGER_INCLUDED_
#define _Y60_ACXPSHELL_JSREQUEST_MANAGER_INCLUDED_

#include <y60/JSWrapper.h>
#include <y60/RequestManager.h>
#include <asl/Ptr.h>
#include <asl/string_functions.h>

namespace jslib {

class JSRequestManager : public JSWrapper<inet::RequestManager, asl::Ptr<inet::RequestManager>, StaticAccessProtocol > {
        JSRequestManager() {}
    public:
        typedef inet::RequestManager NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "RequestManager";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_ACTIVE_COUNT = -100,
        };
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
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

        JSRequestManager(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSRequestManager & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSRequestManager &>(JSRequestManager::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<inet::RequestManager> : public JSClassTraitsWrapper<inet::RequestManager, JSRequestManager> {};

bool convertFrom(JSContext *cx, jsval theValue, JSRequestManager::OWNERPTR & theRequest);
bool convertFrom(JSContext *cx, jsval theValue, JSRequestManager::NATIVE & theRequest);
bool convertFrom(JSContext *cx, jsval theValue, JSRequestManager::NATIVE *& theRequest);

jsval as_jsval(JSContext *cx, JSRequestManager::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSRequestManager::OWNERPTR theOwner, JSRequestManager::NATIVE * theManager);

}

#endif

