//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_JSSound_h_
#define _ac_y60_JSSound_h_

#include "JSSoundManager.h"
#include "JSGrainSource.h"

#include <y60/sound/Sound.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/IScriptablePlugin.h>

#include <asl/base/PlugInBase.h>
#include <asl/audio/Pump.h>


namespace jslib {

    class JSSound : public jslib::JSWrapper<y60::Sound, y60::SoundPtr, jslib::StaticAccessProtocol> {
            JSSound() {}
        public:
            typedef y60::Sound NATIVE;
            typedef y60::SoundPtr OWNERPTR;
            typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;
    
            JSSound(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            {}
    
            ~JSSound() {};
            
            static const char * ClassName() {
                return "Sound";
            }
            static JSFunctionSpec * Functions();
            static JSFunctionSpec * StaticFunctions();
    
            enum PropertyNumbers {
                PROP_volume = -100,
                PROP_duration,
                PROP_canseek,
                PROP_looping,
                PROP_playing,
                PROP_time,
                PROP_src
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
    
            static JSSound & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSSound &>(JSSound::getJSWrapper(cx,obj));
            }
    };

    jsval as_jsval(JSContext *cx, JSSound::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSSound::OWNERPTR theOwner, JSSound::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSSound::NATIVE> 
        : public JSClassTraitsWrapper<JSSound::NATIVE, JSSound> {};

}

namespace y60 {
	class JSSoundPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
    	public:
    		JSSoundPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

    		virtual void initClasses(JSContext * theContext,
    			JSObject *theGlobalObject) 
            {
                jslib::JSSound::initClass(theContext, theGlobalObject);
                jslib::JSSoundManager::initClass(theContext, theGlobalObject);
                jslib::JSGrainSource::initClass(theContext, theGlobalObject);
    		}

    		const char * ClassName() {
    		    static const char * myClassName = "Sound";
    		    return myClassName;
    		}
	};
}

#endif
