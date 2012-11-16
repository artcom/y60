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

#ifndef _ac_jslib_JSLogger_h_
#define _ac_jslib_JSLogger_h_

#include "y60_jslib_settings.h"

#include <asl/base/Logger.h>
#include <asl/base/Ptr.h>
#include <y60/jsbase/JSWrapper.h>

namespace jslib {

    /**
     * @ingroup y60jslib
     *
     * Javascript interface to asl::Logger.
     * @see asl::Logger
     */
    class JSLogger : public JSWrapper<asl::Logger, asl::Ptr<asl::Logger>, StaticAccessProtocol> {
        JSLogger() {}
    public:
        typedef asl::Logger NATIVE;
        typedef asl::Ptr<asl::Logger> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;
        static const char * ClassName() {
            return "Logger";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        static JSPropertySpec * Properties();
        static JSPropertySpec * StaticProperties();
        static JSConstIntPropertySpec * ConstIntProperties();
        virtual unsigned long length() const {
            return 1;
        }
        // getproperty handling
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        // setproperty handling
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, asl::Ptr<asl::Logger> theLogger, asl::Logger * theNative) {
            return Base::Construct(cx, theLogger, theNative);
        }
        JSLogger(asl::Ptr<asl::Logger> theLogger, asl::Logger * theNative)
            : Base(theLogger, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    };
}

#endif // _ac_jslib_JSLogger_h_
