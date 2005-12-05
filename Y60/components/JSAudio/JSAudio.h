//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSAudio.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2004/10/04 13:36:42 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSAUDIO_INCLUDED_
#define _Y60_ACXPSHELL_JSAUDIO_INCLUDED_

#include <audio/AudioController.h>
#include <asl/string_functions.h>
#include <asl/PlugInBase.h>

#include <y60/IScriptablePlugin.h>
#include <y60/JSWrapper.h>



class JSAudio : public jslib::JSWrapper<int, asl::Ptr<int>, jslib::StaticAccessProtocol > {
        JSAudio() {}
    public:

        typedef int NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR,jslib::StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "AudioController";
        }
        static JSFunctionSpec * Functions();
        static JSPropertySpec * Properties();
        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();


        virtual unsigned long length() const {
            return 1;
        }

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSAudio(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSAudio & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSAudio &>(JSAudio::getJSWrapper(cx,obj));
        }

    private:
};

namespace y60 {
	class JSAudioPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
    	public:
    		JSAudioPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

    		virtual void initClasses(JSContext * theContext,
    			JSObject *theGlobalObject) {
    			JSAudio::initClass(theContext, theGlobalObject);
    		}

    		const char * ClassName() {
    		    static const char * myClassName = "AudioController";
    		    return myClassName;
    		}
	};
}

extern "C"
EXPORT asl::PlugInBase * jsaudio_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSAudioPlugIn(myDLHandle);
}
namespace jslib {
template <>
struct JSClassTraits<JSAudio::NATIVE> : public JSClassTraitsWrapper<JSAudio::NATIVE, JSAudio> {};
}
//bool convertFrom(JSContext *cx, jsval theValue, JSAudio::NATIVE & theNative);

//jsval as_jsval(JSContext *cx, JSAudio::OWNERPTR theOwner);
//jsval as_jsval(JSContext *cx, JSAudio::OWNERPTR theOwner, JSAudio::NATIVE * theNative);


#endif

