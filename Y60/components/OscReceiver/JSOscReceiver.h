//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef JS_OSC_RECEIVER_INCLUDED
#define JS_OSC_RECEIVER_INCLUDED

#include <y60/jsbase/JSWrapper.h>
#include "OscReceiver.h"

namespace jslib {

    class JSOscReceiver : public jslib::JSWrapper<y60::OscReceiver, y60::OscReceiverPtr, jslib::StaticAccessProtocol> {
        JSOscReceiver() {}
    public:
        typedef y60::OscReceiver NATIVE;
        typedef y60::OscReceiverPtr OWNERPTR;
        typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;
    
        JSOscReceiver(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}
    
            ~JSOscReceiver() {};
            
            static const char * ClassName() {
                return "OscReceiver";
            }
            static JSFunctionSpec * Functions();
            static JSFunctionSpec * StaticFunctions();
    
            enum PropertyNumbers {
                PROP_eventSchema = -100
            };

            static jslib::JSConstIntPropertySpec * ConstIntProperties();
            static JSPropertySpec * Properties();
            static JSPropertySpec * StaticProperties();
    
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
    
            static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    
            static JSOscReceiver & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSOscReceiver &>(JSOscReceiver::getJSWrapper(cx,obj));
            }
    };

    jsval as_jsval(JSContext *cx, JSOscReceiver::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSOscReceiver::OWNERPTR theOwner, JSOscReceiver::NATIVE * theNative);

    template <>
        struct JSClassTraits<JSOscReceiver::NATIVE> 
        : public JSClassTraitsWrapper<JSOscReceiver::NATIVE, JSOscReceiver> {};

}

#endif
