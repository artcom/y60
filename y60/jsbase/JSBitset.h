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

#ifndef _Y60_ACXPSHELL_JSBITSET_INCLUDED_
#define _Y60_ACXPSHELL_JSBITSET_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JScppUtils.h"
#include "JSWrapper.h"

#include <asl/base/settings.h>
#include <asl/base/string_functions.h>
#include <asl/base/Singleton.h>
#include <asl/base/Enum.h>
#include <asl/dom/Value.h>
#include <y60/base/DataTypes.h>

namespace jslib {

template <class BITSET>
class JSBitset : public JSWrapper<BITSET> {
    private:
        JSBitset();  // hide default constructor
    public:
        virtual ~JSBitset() {
        }
        typedef BITSET NATIVE;
        typedef JSWrapper<NATIVE> Base;
        typedef typename Base::NativeValuePtr NativeValuePtr;
        typedef NativeValuePtr OWNERPTR;

        static std::string buildClassName( const char * theName ) {
            std::string myName(theName);
            std::string::size_type myBegin = myName.rfind(':');
            if (myBegin == std::string::npos) {
                return myName;
            }
            return myName.substr( myBegin + 1);
        }

        static const char * ClassName() {
            static std::string myClassName = buildClassName( BITSET::getName());
            return myClassName.c_str();
        };
        static JSFunctionSpec * Functions();

        virtual unsigned long length() const {
            return BITSET::Flags::MAX;
        }
        virtual JSBool getPropertyByNumericId(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
            if (theID >= 0 && theID < BITSET::Flags::MAX) {
                const NATIVE & myBitset = this->getNative();
                *vp = BOOLEAN_TO_JSVAL(myBitset.test(theID));
                return JS_TRUE;
            }
            return JS_FALSE;
        }
        virtual JSBool setPropertyByNumericId(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
            typename JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
            if (theID >= 0 && theID < BITSET::Flags::MAX) {
                bool myFlag;
                convertFrom(cx, *vp, myFlag);
                myObj.getNative()[theID] = myFlag;
                return JS_TRUE;
            }
            return JS_FALSE;
        }

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            DOC_BEGIN("Creates a bitset.");
            DOC_END;
            if (JSA_GetClass(cx,obj) != JSBitset<BITSET>::Class()) {
                JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
                return JS_FALSE;
            }
            JSBitset * myNewObject = 0;
            NativeValuePtr myNewValue = NativeValuePtr(new dom::SimpleValue<BITSET>(0));
            /*BITSET & myNewBitset =*/ myNewValue->openWriteableValue(); // we close it only on success, otherwise we trash it anyway
            if (argc == 0) {
                // construct empty
                myNewObject = new JSBitset(myNewValue);
            } else {
                /* XXX TODO
                if (argc == 2) {
                    JSObject * myObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[0]);
                    if (!myObject) {
                        JS_ReportError(cx,"JSBitset::Constructor: argument #1 must be a normalized Vector3f");
                        return JS_FALSE;
                    }
                    jsdouble myNumber;
                    if (JS_ValueToNumber(cx,argv[1],&myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
                        // construct from axis and angle
                        myNewBitset = asl::Bitset<Number>(JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject), Number(myNumber));
                    } else {
                        JSObject * mySecondObject = JSVector<asl::Vector3<Number> >::Construct(cx, argv[1]);
                        if (mySecondObject) {
                            // Given two normalized Vectors, compute the quaternion between them
                            myNewBitset = asl::Bitset<Number>(JSVector<asl::Vector3<Number> >::getNativeRef(cx,myObject), JSVector<asl::Vector3<Number> >::getNativeRef(cx,mySecondObject));
                        } else {
                            JS_ReportError(cx,"JSBitset::Constructor: argument #2 must be a float or a Vector3f");
                            return JS_FALSE;
                        }
                    }
                    myNewObject = new JSBitset(myNewValue);
                } else if (argc == 1) {
                    // construct from one Bitset
                    if (JSVAL_IS_VOID(argv[0])) {
                        JS_ReportError(cx,"JSBitset::Constructor: need one argument");
                        return JS_FALSE;
                    }

                    asl::Vector4<Number> myVector4;

                    if (convertFrom(cx, argv[0], myNewBitset)) {
                        myNewObject = new JSBitset(myNewValue);

                    } else if (convertFrom(cx, argv[0], myVector4)) {
                        myNewBitset = asl::Bitset<Number>(myVector4);
                        myNewObject = new JSBitset(myNewValue);

                    } else {
                        JS_ReportError(cx,"JSBitset::Constructor: argument #1 must be a Bitset or a Vector4");
                        return JS_FALSE;
                    }

                } else if (argc == 4) {
                    // construct from four numbers
                    for (int i = 0; i < 4 ;++i) {
                        jsdouble myNumber;
                        if (!JS_ValueToNumber(cx,argv[i],&myNumber) && !JSDOUBLE_IS_NaN(myNumber)) {
                            JS_ReportError(cx,"JSBitset::Constructor: argument #%d must be a float", i);
                            return JS_FALSE;
                        }
                        myNewBitset[i] = float(myNumber);
                    }
                    myNewObject = new JSBitset(myNewValue);
                } else {
                    */
                    JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0,1 or 2, got %d",ClassName(), argc);
                    return JS_FALSE;
                //}
            }
            if (myNewObject) {
                JS_SetPrivate(cx,obj,myNewObject);
                myNewValue->closeWriteableValue();
                return JS_TRUE;
            }
            JS_ReportError(cx,"JSBitset::Constructor: bad parameters");
            return JS_FALSE;
        }

        static
        JSObject * Construct(JSContext *cx, NativeValuePtr theValue) {
            return Base::Construct(cx, theValue, 0);
        }
        static
        JSObject * Construct(JSContext *cx, const BITSET & theBitset) {
            NativeValuePtr myValue(new typename dom::ValueWrapper<BITSET>::Type(theBitset, 0));
            return Base::Construct(cx, myValue, 0);
        }
        /*
        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }
        */

        JSBitset(NativeValuePtr theValue)
            : Base(theValue, 0)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

    private:
};

template<class BITSET>
static JSBool
bitsetToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("returns string representation of the Bitset.");
    DOC_RVAL("", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = asl::as_string(JSBitset<BITSET>::getJSWrapper(cx,obj).getNative());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

template <class BITSET>
JSFunctionSpec *
JSBitset<BITSET>::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<std::endl;
    static JSFunctionSpec myFunctions[] = {
        /* name                native          nargs    */
        {"toString",           &bitsetToString<BITSET>,              0},
        {0}
    };
    return myFunctions;
}

template <class ENUM>
struct JSClassTraits<asl::Bitset<ENUM> > : public JSClassTraitsWrapper<asl::Bitset<ENUM>, JSBitset<asl::Bitset<ENUM> > > {};

template <class ENUM>
jsval as_jsval(JSContext *cx,
        asl::Ptr<dom::SimpleValue<asl::Bitset<ENUM> > , dom::ThreadingModel> theValue)
{
    JSObject * myReturnObject = JSBitset<asl::Bitset<ENUM> >::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

template <class ENUM>
jsval as_jsval(JSContext *cx, const asl::Bitset<ENUM> & theValue) {
    JSObject * myObject = JSBitset<asl::Bitset<ENUM> >::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myObject);
}

template <class ENUM>
bool convertFrom(JSContext *cx, jsval theValue, asl::Bitset<ENUM> & theBitset) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Bitset<ENUM> >::Class()) {
                theBitset = JSClassTraits<asl::Bitset<ENUM> >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}
template <class ENUM>
bool convertFrom(JSContext *cx, jsval theValue,
        asl::Ptr<dom::SimpleValue<asl::Bitset<ENUM> >, dom::ThreadingModel> theValuePtr)
{
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Bitset<ENUM> >::Class()) {
                theValuePtr = JSClassTraits<asl::Bitset<ENUM> >::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

template <class BITSET>
JSObject *
JSBitset<BITSET>::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,
            Functions(), 0, 0, 0);
    if (myClassObject) {
        uintN myFlags = JSPROP_ENUMERATE | JSPROP_PERMANENT;
        for (int8 i = 0; i < BITSET::Flags::MAX; ++i) {
            /*JSBool myOK =*/ JS_DefinePropertyWithTinyId(cx, myClassObject, BITSET::Flags::getString(i), i,
                    JS_FALSE, NULL, NULL, myFlags);
        }
        /*
        jsval myConstructorFuncObjVal;
        if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
            uintN myFlags = JSPROP_READONLY | JSPROP_PERMANENT;
            for (unsigned i = 0; i < ENUM::MAX; ++i) {
                JSBool myOK = JS_DefineProperty(cx, JSVAL_TO_OBJECT(myConstructorFuncObjVal), ENUM::getString(i),
                        as_jsval(cx, ENUM::getString(i)), NULL, NULL, myFlags);
            }
        }
        */
    } else {
        std::cerr << "JSBitset::initClass: constructor function object not found, could not initialize class"<<std::endl;
    }
    return myClassObject;
}


} // namespace
#endif
