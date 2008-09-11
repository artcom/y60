//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#ifndef _ac_jslib_JSParticleSystem_h_
#define _ac_jslib_JSParticleSystem_h_

#include <y60/jsbase/JSWrapper.h>
#include "ParticleSystem.h"

class JSParticleSystem : public jslib::JSWrapper<ParticleSystem, asl::Ptr<ParticleSystem> , jslib::StaticAccessProtocol> {
    JSParticleSystem() {}
    
public:
    typedef ParticleSystem NATIVE;
    typedef asl::Ptr<NATIVE> OWNERPTR;
    typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;
    
    JSParticleSystem(OWNERPTR theOwner, NATIVE * theNative)
        : Base(theOwner, theNative)
        {}
    
    virtual ~JSParticleSystem();
    
    static const char * ClassName() {
            return "GPUParticles";
    }
    
    static JSFunctionSpec * Functions();
    
    enum PropertyNumbers {
        PROP_material = -100,
        PROP_body,
        PROP_animation,
        PROP_image 
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
    
    static JSParticleSystem & getObject(JSContext *cx, JSObject * obj) {
        return dynamic_cast<JSParticleSystem &>(JSParticleSystem::getJSWrapper(cx,obj));
    }
};

namespace jslib {
    template <>
    struct JSClassTraits<JSParticleSystem::NATIVE>
        : public JSClassTraitsWrapper<JSParticleSystem::NATIVE , JSParticleSystem> {};
    
} // namespace jslib

jsval as_jsval(JSContext *cx, JSParticleSystem::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSParticleSystem::OWNERPTR theOwner, JSParticleSystem::NATIVE * theSerial);

#endif

