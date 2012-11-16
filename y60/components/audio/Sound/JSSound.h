/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_y60_JSSound_h_
#define _ac_y60_JSSound_h_

#include "y60_jssound_settings.h"

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
