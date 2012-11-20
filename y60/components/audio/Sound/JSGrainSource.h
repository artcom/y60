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

#ifndef _JSGRAINSOURCE_H_
#define _JSGRAINSOURCE_H_

#include "y60_jssound_settings.h"

#include <y60/jsbase/JSWrapper.h>
#include <y60/sound/GrainSource.h>

namespace jslib {

    class JSGrainSource : public jslib::JSWrapper<y60::GrainSource, y60::GrainSourcePtr, jslib::StaticAccessProtocol> {

        JSGrainSource() {};

    public:

        typedef y60::GrainSource NATIVE;
        typedef y60::GrainSourcePtr OWNERPTR;
        typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSGrainSource(OWNERPTR theOwner, NATIVE * theNative) :
            Base(theOwner, theNative)
        {}

        ~JSGrainSource() {};

        static const char * ClassName() {
            return "GrainSource";
        }

        virtual unsigned long length() const {
            return 1;
        }

        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        enum PropertyNumbers {
            PROP_volume = -100,
            PROP_size,
            PROP_sizejitter,
            PROP_rate,
            PROP_ratejitter,
            PROP_position,
            PROP_positionjitter,
            PROP_ratio,
            PROP_ratiojitter,
            PROP_transposition
        };
        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * Properties();
        static JSPropertySpec * StaticProperties();

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSGrainSource & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGrainSource &>(JSGrainSource::getJSWrapper(cx,obj));
        }
    };

    bool convertFrom(JSContext *cx, jsval theValue, y60::GrainSourcePtr & theGrainSource);

    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner, JSGrainSource::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSGrainSource::NATIVE>
        : public JSClassTraitsWrapper<JSGrainSource::NATIVE, JSGrainSource> {};

}

#endif
