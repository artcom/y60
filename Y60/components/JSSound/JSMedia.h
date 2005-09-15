//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_JSMedia_h_
#define _ac_y60_JSMedia_h_

#include "Media.h"
#include <y60/JSWrapper.h>

namespace jslib {

    class JSMedia : public jslib::JSWrapper<y60::Media, asl::Ptr<y60::Media>, jslib::StaticAccessProtocol> {
            JSMedia() {}
        public:
            typedef y60::Media NATIVE;
            typedef asl::Ptr<y60::Media> OWNERPTR;
            typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;
    
            JSMedia(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            {}
    
            ~JSMedia() {};
            
            static const char * ClassName() {
                return "MediaController";
            }
            static JSFunctionSpec * Functions();
    
            enum PropertyNumbers {
                PROP_volume = -100,
                PROP_running,
                PROP_soundcount
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
    
            static JSMedia & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSMedia &>(JSMedia::getJSWrapper(cx,obj));
            }
    };

    jsval as_jsval(JSContext *cx, JSMedia::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSMedia::OWNERPTR theOwner, JSMedia::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSMedia::NATIVE> 
        : public JSClassTraitsWrapper<JSMedia::NATIVE, JSMedia> {};

}


#endif

