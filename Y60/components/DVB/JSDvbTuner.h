//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#ifndef _ac_jslib_JSDvbTuner_h_
#define _ac_jslib_JSDvbTuner_h_

#include <y60/JSWrapper.h>
#include "DvbTuner.h"

class JSDvbTuner : public jslib::JSWrapper<DvbTuner, asl::Ptr<DvbTuner> , jslib::StaticAccessProtocol> {
    JSDvbTuner() {}

    public:
        typedef DvbTuner NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;
    
        JSDvbTuner(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSDvbTuner();

        static const char * ClassName() {
            return "DvbTuner";
        }
    
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_carrierFrequency = -100,
            PROP_videoPID,
            PROP_audioPID,
            PROP_videoTextPID,
            PROP_deviceName,
            PROP_channel
        };

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

        static JSDvbTuner & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSDvbTuner &>(JSDvbTuner::getJSWrapper(cx,obj));
        }
};

namespace jslib {
template <>
struct JSClassTraits<JSDvbTuner::NATIVE>
    : public JSClassTraitsWrapper<JSDvbTuner::NATIVE , JSDvbTuner> {};

} // namespace jslib

jsval as_jsval(JSContext *cx, JSDvbTuner::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSDvbTuner::OWNERPTR theOwner, JSDvbTuner::NATIVE * theSerial);

#endif

