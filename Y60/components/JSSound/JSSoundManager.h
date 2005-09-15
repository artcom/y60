//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_JSSoundManager_h_
#define _ac_y60_JSSoundManager_h_

#include <y60/SoundManager.h>
#include <y60/JSWrapper.h>

namespace jslib {

    class JSSoundManager : public jslib::JSWrapper<y60::SoundManager, 
                                  y60::SoundManager *, jslib::StaticAccessProtocol> 
    {
            JSSoundManager() {}
        public:
            typedef y60::SoundManager NATIVE;
            typedef y60::SoundManager * OWNERPTR;
            typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;
    
            JSSoundManager(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            {}
    
            ~JSSoundManager() {};
            
            static const char * ClassName() {
                return "SoundManager";
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
    
            virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, 
                    JSObject *obj, jsval id, jsval *vp);
            virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, 
                    JSObject *obj, jsval id, jsval *vp);
    
            static JSBool
            Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    
            static
            JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
                return Base::Construct(cx, theOwner, theNative);
            }        
    
            static jslib::JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    
            static JSSoundManager & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSSoundManager &>(JSSoundManager::getJSWrapper(cx,obj));
            }
    };

    jsval as_jsval(JSContext *cx, JSSoundManager::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSSoundManager::OWNERPTR theOwner, 
            JSSoundManager::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSSoundManager::NATIVE> 
        : public JSClassTraitsWrapper<JSSoundManager::NATIVE, JSSoundManager> {};

}


#endif

