//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSStringMover_INCLUDED_
#define _Y60_ACXPSHELL_JSStringMover_INCLUDED_

#include <Y60/JSWrapper.h>
#include "CharacterMover.h"

#include <asl/string_functions.h>

namespace jslib {

class JSStringMover : public JSWrapper<StringMover, asl::Ptr<StringMover>, StaticAccessProtocol>
{
        JSStringMover() {}
    public:
        typedef StringMover NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "StringMover";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_STATE = -100
        };
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

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

        JSStringMover(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSStringMover & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSStringMover &>(JSStringMover::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<StringMover> : public JSClassTraitsWrapper<StringMover, JSStringMover> {};

}

#endif

