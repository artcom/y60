//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSENUM_INCLUDED_
#define _Y60_ACXPSHELL_JSENUM_INCLUDED_

#include "JScppUtils.h"
#include "JSWrapper.h"

#include <asl/settings.h>
#include <asl/string_functions.h>
#include <asl/Singleton.h>
#include <dom/Value.h>
#include <y60/DataTypes.h>

namespace jslib {

template <class ENUM>    
class JSEnum : public JSWrapper<ENUM> {
    private:
        JSEnum();  // hide default constructor
    public:
        virtual ~JSEnum() {
        }
        typedef ENUM NATIVE;
        typedef JSWrapper<NATIVE> Base;
        typedef typename Base::NativeValuePtr NativeValuePtr;
        typedef NativeValuePtr OWNERPTR;

        static std::string buildClassName( const char * theName ) {
            std::string myName(theName);
            std::string::size_type myBegin = myName.rfind(':');
            if (myBegin == std::string::npos) {
                return myName;
            }
            return myName.substr( myBegin + 1);
        }

        static const char * ClassName() {
            static std::string myClassName = buildClassName( ENUM::getName());
            return myClassName.c_str();
        };
        
        virtual unsigned long length() const {
            return 1;
        }
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
            return JS_FALSE;
        }
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
            return JS_FALSE;
        }

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Not instantiatable.");
            DOC_END;
            JS_ReportError(cx,"Constructor for %s called. Class is not instantiatable.",ClassName());
            return JS_FALSE;
        }

        static
        JSObject * Construct(JSContext *cx, NativeValuePtr theValue) {
            return Base::Construct(cx, theValue, 0);
        }
        static
        JSObject * Construct(JSContext *cx, const ENUM & theEnum) {
            NativeValuePtr myValue(new typename dom::ValueWrapper<ENUM>::Type(theEnum, 0));
            return Base::Construct(cx, myValue, 0);
        }
        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }
        
        JSEnum(NativeValuePtr theValue)
            : Base(theValue, 0)
        {}
        
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

    private:
};

template <class NATIVE_ENUM, int THE_MAX>
struct JSClassTraits<asl::Enum<NATIVE_ENUM, THE_MAX> > : public JSClassTraitsWrapper<asl::Enum<NATIVE_ENUM, THE_MAX>, JSEnum<asl::Enum<NATIVE_ENUM, THE_MAX> > > {};

template <class NATIVE_ENUM, int THE_MAX>
jsval as_jsval(JSContext *cx,
        asl::Ptr<dom::SimpleValue<asl::Enum<NATIVE_ENUM, THE_MAX> > , dom::ThreadingModel> theValue)
{
    return as_jsval(cx, theValue->getValue().asString());
}

template <class NATIVE_ENUM, int THE_MAX>
jsval as_jsval(JSContext *cx, const asl::Enum<NATIVE_ENUM, THE_MAX> & theNative) {
    return as_jsval(cx, theNative.asString());
}

template <class NATIVE_ENUM, int THE_MAX>
bool convertFrom(JSContext *cx, jsval theValue, asl::Enum<NATIVE_ENUM, THE_MAX> & theEnum) {
    if (JSVAL_IS_STRING(theValue)) {
        std::string myString;
        convertFrom(cx, theValue, myString);
        theEnum.fromString(myString);
        return true;
    }
    return false;
}
template <class NATIVE_ENUM, int THE_MAX>
bool convertFrom(JSContext *cx, jsval theValue, 
        asl::Ptr<dom::SimpleValue<asl::Enum<NATIVE_ENUM, THE_MAX> >, dom::ThreadingModel> theNativeValue) 
{
    if (JSVAL_IS_STRING(theValue)) {
        std::string myString;
        convertFrom(cx, theValue, myString);
        theNativeValue->openWriteableValue().fromString(myString);
        theNativeValue->closeWriteableValue();
        return true;
    }
    return false;
}

template <class ENUM>    
JSObject * 
JSEnum<ENUM>::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0, 0, 0, 0);
    if (myClassObject) {
        jsval myConstructorFuncObjVal;
        if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
            uintN myFlags = JSPROP_READONLY | JSPROP_PERMANENT;
            for (unsigned i = 0; i < ENUM::MAX; ++i) {
                JSBool myOK = JS_DefineProperty(cx, JSVAL_TO_OBJECT(myConstructorFuncObjVal), ENUM::getString(i),
                        as_jsval(cx, ENUM::getString(i)), NULL, NULL, myFlags);
            }
        }
    } else {
        std::cerr << "JSEnum::initClass: constructor function object not found, could not initialize static members"<<std::endl;
    }
    return myClassObject;
}


} // namespace
#endif
