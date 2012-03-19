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

#ifndef _Y60_JSBERKELIUMWINDOW_INCLUDED_
#define _Y60_JSBERKELIUMWINDOW_INCLUDED_

#include <asl/dom/Nodes.h>

#include <y60/jsbase/JSWrapper.h>

#include "AssimpLoader.h"

namespace jslib {

    class JSAssimpLoader : public JSWrapper<y60::AssimpLoader, asl::Ptr< y60::AssimpLoader>, StaticAccessProtocol> {
        JSAssimpLoader();  // hide default constructor
    
        public:

            virtual ~JSAssimpLoader() {};

            typedef y60::AssimpLoader NATIVE;
            typedef asl::Ptr<y60::AssimpLoader> OWNERPTR;

            typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

            static const char * ClassName() {
                return "Loader";
            }

            static JSFunctionSpec * Functions();

            static JSPropertySpec * Properties();

            virtual unsigned long length() const {
                return 1;
            }

            static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                    ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            virtual JSBool getPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, ::JSContext *cx,
                    JSObject *obj, jsval id, jsval *vp);
            virtual JSBool setPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);

            static JSBool
                Constructor(::JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

            static
                JSObject * Construct(::JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
                    return Base::Construct(cx, theOwner, theNative);
                }

            JSAssimpLoader(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            { }

            static JSFunctionSpec * StaticFunctions();
            static JSPropertySpec * StaticProperties();
            static JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(::JSContext *cx, JSObject *theGlobalObject);
            static void addClassProperties(::JSContext * cx, JSObject * theClassProto);

            static JSAssimpLoader & getObject(::JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSAssimpLoader &>(JSAssimpLoader::getJSWrapper(cx,obj));
            }
    };

    template <>
    struct JSClassTraits<JSAssimpLoader::NATIVE>
        : public JSClassTraitsWrapper<JSAssimpLoader::NATIVE, JSAssimpLoader> {};

    jsval as_jsval(JSContext *cx, JSAssimpLoader::OWNERPTR theOwner, JSAssimpLoader::NATIVE * theContext);
    bool convertFrom(JSContext *cx, jsval theValue, JSAssimpLoader::NATIVE *& theContext);
}

#endif /* _Y60_JSBERKELIUMWINDOW_INCLUDED_*/
