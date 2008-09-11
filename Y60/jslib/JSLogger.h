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

#ifndef _ac_jslib_JSLogger_h_
#define _ac_jslib_JSLogger_h_

#include <asl/base/Logger.h>
#include <asl/base/Ptr.h>
#include <y60/jsbase/JSWrapper.h>

namespace jslib {

    /**
     * @ingroup y60jslib
     *
     * Javascript interface to asl::Logger.
     * @see asl::Logger
     */
    class JSLogger : public JSWrapper<asl::Logger, asl::Ptr<asl::Logger>, StaticAccessProtocol> {
        JSLogger() {}
    public:
        typedef asl::Logger NATIVE;
        typedef asl::Ptr<asl::Logger> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;
        static const char * ClassName() {
            return "Logger";
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
        JSObject * Construct(JSContext *cx, asl::Ptr<asl::Logger> theLogger, asl::Logger * theNative) {
            return Base::Construct(cx, theLogger, theNative);
        }
        JSLogger(asl::Ptr<asl::Logger> theLogger, asl::Logger * theNative)
            : Base(theLogger, theNative)
        {}
    
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    };
}

#endif // _ac_jslib_JSLogger_h_
