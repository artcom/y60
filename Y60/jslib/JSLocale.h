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

#ifndef _ac_jslib_JSLocale_h_
#define _ac_jslib_JSLocale_h_

#include "JSWrapper.h"
#include <locale>

namespace jslib {

    /**
     * @ingroup y60jslib
     *
     * Javascript interface to STL's locale and facets.
     */
    class JSLocale : public JSWrapper<std::locale, asl::Ptr<std::locale>, StaticAccessProtocol> {
        JSLocale() {};
    public:
        virtual ~JSLocale() {};
        typedef std::locale NATIVE;
        typedef asl::Ptr<std::locale> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;
        static const char * ClassName() {
            return "Locale";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();
    
        static JSPropertySpec * Properties();
        static JSPropertySpec * StaticProperties();
        static JSConstIntPropertySpec * ConstIntProperties();
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
        JSObject * Construct(JSContext *cx, asl::Ptr<NATIVE> theLocale, NATIVE * theNative) {
            return Base::Construct(cx, theLocale, theNative);
        }
        JSLocale(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}
    
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    };

template <>
struct JSClassTraits<std::locale> : public JSClassTraitsWrapper<std::locale, JSLocale> {};
bool convertFrom(JSContext *cx, jsval theValue, JSLocale::NATIVE *& theLocale);
bool convertFrom(JSContext *cx, jsval theValue, JSLocale::OWNERPTR & theLocale);

jsval as_jsval(JSContext *cx, JSLocale::OWNERPTR theOwner);
}

#endif // _ac_jslib_JSLocale_h_

