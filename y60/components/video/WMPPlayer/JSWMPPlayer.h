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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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


