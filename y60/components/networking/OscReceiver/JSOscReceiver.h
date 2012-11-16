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
