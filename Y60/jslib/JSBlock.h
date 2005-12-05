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
//   $RCSfile: JSBlock.h,v $
//   $Author: ulrich $
//   $Revision: 1.5 $
//   $Date: 2005/04/22 14:58:47 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSBLOCK_INCLUDED_
#define _Y60_ACXPSHELL_JSBLOCK_INCLUDED_

#include "JSWrapper.h"

#include <asl/Block.h>

namespace jslib {

class JSBlock : public JSWrapper<asl::Block, asl::Ptr<asl::Block>, StaticAccessProtocol>
{
        JSBlock() {}
    public:
        typedef asl::Block NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Block";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_size = -100,
            PROP_capacity
        };

        static JSPropertySpec * Properties();
        static JSFunctionSpec * StaticFunctions();
        static JSPropertySpec * StaticProperties();

        virtual unsigned long length() const {
            return getNative().size();
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSBlock(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSBlock & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSBlock &>(JSBlock::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<asl::Block> : public JSClassTraitsWrapper<asl::Block, JSBlock> {};

bool convertFrom(JSContext *cx, jsval theValue, JSBlock::NATIVE *& theBlock);

jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner, JSBlock::NATIVE * theBlock);

}

#endif

