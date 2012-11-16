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
//   $RCSfile: JSBlock.h,v $
//   $Author: ulrich $
//   $Revision: 1.5 $
//   $Date: 2005/04/22 14:58:47 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSBLOCK_INCLUDED_
#define _Y60_ACXPSHELL_JSBLOCK_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSWrapper.h"

#include <asl/base/Block.h>

namespace jslib {

class Y60_JSBASE_DECL JSBlock : public JSWrapper<asl::Block, asl::Ptr<asl::Block>, VectorValueAccessProtocol>
{
        JSBlock() {}
    public:
        typedef asl::Block NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,VectorValueAccessProtocol> Base;

        static const char * ClassName() {
            return "Block";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_size = -100,
            PROP_capacity,
            PROP_bytes
        };

        static JSPropertySpec * Properties();
        static JSFunctionSpec * StaticFunctions();
        static JSPropertySpec * StaticProperties();

        virtual unsigned long length() const {
            return getNative().size();
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

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

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, JSBlock::NATIVE *& theBlock);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, JSBlock::OWNERPTR & theBlock);

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, JSBlock::OWNERPTR theOwner, JSBlock::NATIVE * theBlock);

}

#endif

