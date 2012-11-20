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
//
//   $RCSfile: JSASSOscClient.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/31 13:12:12 $
//
//
//=============================================================================

#ifndef _ac_y60_components_jsctscan_h_
#define _ac_y60_components_jsctscan_h_

#include "y60_oscclient_settings.h"

#include "ASSOscClient.h"

#include <y60/jsbase/JSWrapper.h>

namespace jslib {

class JSASSOscClient : public jslib::JSWrapper<y60::ASSOscClient,
        asl::Ptr<y60::ASSOscClient> , jslib::StaticAccessProtocol>
{
        JSASSOscClient() {}
    public:
        typedef y60::ASSOscClient NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSASSOscClient(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSASSOscClient() {};

        static const char * ClassName() {
            return "ASSOscClient";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        /*
        enum PropertyNumbers {
        };
        */
        static JSPropertySpec * Properties();

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

        static JSASSOscClient & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSASSOscClient &>(JSASSOscClient::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSASSOscClient::NATIVE>
    : public JSClassTraitsWrapper<JSASSOscClient::NATIVE, JSASSOscClient> {};


bool convertFrom(JSContext *cx, jsval theValue, JSASSOscClient::OWNERPTR & theASSOscClient);

jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner, JSASSOscClient::NATIVE * theASSOscClient);

} // namespace jslib

#endif

