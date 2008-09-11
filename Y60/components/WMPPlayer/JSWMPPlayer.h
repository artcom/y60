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
//   $RCSfile: JSCTScan.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/31 13:12:12 $
//
//
//=============================================================================

#ifndef _ac_y60_components_jswmpplayer_h_
#define _ac_y60_components_jswmpplayer_h_

#include "WMPPlayer.h"
#include <y60/jsbase/JSWrapper.h>

namespace jslib {
    
class JSWMPPlayer : public jslib::JSWrapper<y60::WMPPlayer, asl::Ptr<y60::WMPPlayer> , jslib::StaticAccessProtocol> {
        JSWMPPlayer() {}
    public:
        typedef y60::WMPPlayer NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSWMPPlayer(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSWMPPlayer() {};
        
        static const char * ClassName() {
            return "WMPPlayer";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        enum PropertyNumbers {
            PROP_width = -100,
            PROP_height,
            PROP_canvasposition,
            PROP_currentposition,
            PROP_canvassize,
            PROP_playstate,
            PROP_volume,
            PROP_duration,
            PROP_visible,
            PROP_loopcount
        };
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

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSWMPPlayer & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSWMPPlayer &>(JSWMPPlayer::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSWMPPlayer::NATIVE> 
    : public JSClassTraitsWrapper<JSWMPPlayer::NATIVE, JSWMPPlayer> {};


bool convertFrom(JSContext *cx, jsval theValue, JSWMPPlayer::OWNERPTR & theWMPPlayer);

jsval as_jsval(JSContext *cx, JSWMPPlayer::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSWMPPlayer::OWNERPTR theOwner, JSWMPPlayer::NATIVE * theWMPPlayer);

} // namespace jslib
#endif


