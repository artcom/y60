//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSVector.h,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSVECTOR_INCLUDED_
#define _Y60_ACXPSHELL_JSVECTOR_INCLUDED_

#include "JScppUtils.h"
#include "JSWrapper.h"

#include <asl/settings.h>
#include <asl/string_functions.h>
#include <asl/Singleton.h>
#include <dom/Value.h>
#include <y60/DataTypes.h>
#include <asl/Vector234.h>

namespace jslib {

template<class NATIVE_VECTOR> struct JSVector;

template <class VECTOR_NF>
class Converter {
    template <template <class> class VECTOR, class NUMBER>
    static
    bool convertFromVN(JSContext *cx, jsval theValue, VECTOR<NUMBER> & theVector);
public:
    static
    bool convertFrom(JSContext *cx, jsval theValue, VECTOR_NF & theVector) {
        return convertFromVN(cx,theValue,theVector);
    }
};
#define DEFINE_CONVERT_FROM(THE_TYPE) \
inline \
bool convertFrom(JSContext *cx, jsval theValue, THE_TYPE & theVector) { \
    return Converter<THE_TYPE>::convertFrom(cx,theValue,theVector); \
}

DEFINE_CONVERT_FROM(asl::Vector2f)
DEFINE_CONVERT_FROM(asl::Vector3f)
DEFINE_CONVERT_FROM(asl::Vector4f)

DEFINE_CONVERT_FROM(asl::Vector2d)
DEFINE_CONVERT_FROM(asl::Vector3d)
DEFINE_CONVERT_FROM(asl::Vector4d)

DEFINE_CONVERT_FROM(asl::Vector2i)
DEFINE_CONVERT_FROM(asl::Vector3i)
DEFINE_CONVERT_FROM(asl::Vector4i)

DEFINE_CONVERT_FROM(asl::Point2f)
DEFINE_CONVERT_FROM(asl::Point3f)
DEFINE_CONVERT_FROM(asl::Point4f)

DEFINE_CONVERT_FROM(asl::Point2d)
DEFINE_CONVERT_FROM(asl::Point3d)
DEFINE_CONVERT_FROM(asl::Point4d)

DEFINE_CONVERT_FROM(asl::Point2i)
DEFINE_CONVERT_FROM(asl::Point3i)
DEFINE_CONVERT_FROM(asl::Point4i)

template <template<class> class NATIVE_VECTOR, class NUMBER>
jsval as_jsval(JSContext *cx, const NATIVE_VECTOR<NUMBER> & theValue) {
    JSObject * myReturnObject = JSVector<NATIVE_VECTOR<NUMBER> >::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

template <template<class> class NATIVE_VECTOR, class NUMBER>
jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, NATIVE_VECTOR<NUMBER> *) {
    JSObject * myReturnObject = JSVector<NATIVE_VECTOR<NUMBER> >::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

template <template <class> class NATIVE_VECTOR, class NUMBER>
jsval as_jsval(JSContext *cx,
               asl::Ptr<dom::VectorValue<NATIVE_VECTOR<NUMBER> >,dom::ThreadingModel> theValue)
{
    JSObject * myReturnObject = JSVector<NATIVE_VECTOR<NUMBER> >::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

template <>
struct JValueTypeTraits<asl::Vector2f> {
    typedef asl::Vector2f self_type;
    typedef float elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "Vector2f";
    }
    enum {SIZE = 2};
};
template <>
struct JValueTypeTraits<asl::Vector3f> {
    typedef asl::Vector3f self_type;
    typedef float elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "Vector3f";
    }
    enum {SIZE = 3};
};
template <>
struct JValueTypeTraits<asl::Vector4f> {
    typedef asl::Vector4f self_type;
    typedef float elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "Vector4f";
    }
    enum {SIZE = 4};
};
template <>
struct JValueTypeTraits<asl::Vector2d> {
    typedef asl::Vector2d self_type;
    typedef double elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "d";
    }
    static std::string Name() {
        return "Vector2d";
    }
    enum {SIZE = 2};
};
template <>
struct JValueTypeTraits<asl::Vector3d> {
    typedef asl::Vector3d self_type;
    typedef double elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "d";
    }
    static std::string Name() {
        return "Vector3d";
    }
    enum {SIZE = 3};
};
template <>
struct JValueTypeTraits<asl::Vector4d> {
    typedef asl::Vector4d self_type;
    typedef double elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "d";
    }
    static std::string Name() {
        return "Vector4d";
    }
    enum {SIZE = 4};
};

template <>
struct JValueTypeTraits<asl::Vector2i> {
    typedef asl::Vector2i self_type;
    typedef int elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "Vector2i";
    }
    enum {SIZE = 2};
};

template <>
struct JValueTypeTraits<asl::Vector3i> {
    typedef asl::Vector3i self_type;
    typedef int elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "Vector3i";
    }
    enum {SIZE = 3};
};
template <>
struct JValueTypeTraits<asl::Vector4i> {
    typedef asl::Vector4i self_type;
    typedef int elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef self_type vector_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "Vector4i";
    }
    enum {SIZE = 4};
};

template <>
struct JValueTypeTraits<asl::Point2d> {
    typedef asl::Point2d self_type;
    typedef double elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector2d vector_type;
    static std::string Postfix() {
        return "d";
    }
    static std::string Name() {
        return "Point2d";
    }
    enum {SIZE = 2};
};

template <>
struct JValueTypeTraits<asl::Point3d> {
    typedef asl::Point3d self_type;
    typedef double elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector3d vector_type;
    static std::string Postfix() {
        return "d";
    }
    static std::string Name() {
        return "Point3d";
    }
    enum {SIZE = 3};
};
template <>
struct JValueTypeTraits<asl::Point4d> {
    typedef asl::Point4d self_type;
    typedef double elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector4d vector_type;
    static std::string Postfix() {
        return "d";
    }
    static std::string Name() {
        return "Point4d";
    }
    enum {SIZE = 4};
};

template <>
struct JValueTypeTraits<asl::Point2f> {
    typedef asl::Point2f self_type;
    typedef float elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector2f vector_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "Point2f";
    }
    enum {SIZE = 2};
};
template <>
struct JValueTypeTraits<asl::Point3f> {
    typedef asl::Point3f self_type;
    typedef float elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector3f vector_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "Point3f";
    }
    enum {SIZE = 2};
};
template <>
struct JValueTypeTraits<asl::Point4f> {
    typedef asl::Point4f self_type;
    typedef float elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector4f vector_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "Point4f";
    }
    enum {SIZE = 4};
};

template <>
struct JValueTypeTraits<asl::Point2i> {
    typedef asl::Point2i self_type;
    typedef int elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector2i vector_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "Point2i";
    }
    enum {SIZE = 2};
};
template <>
struct JValueTypeTraits<asl::Point3i> {
    typedef asl::Point3i self_type;
    typedef int elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector3i vector_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "Point3i";
    }
    enum {SIZE = 3};
};
template <>
struct JValueTypeTraits<asl::Point4i> {
    typedef asl::Point4i self_type;
    typedef int elem_type;
    typedef dom::VectorValue<self_type> wrapper_type;
    typedef asl::Vector4i vector_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "Point4i";
    }
    enum {SIZE = 4};
};


template <class NATIVE_VECTOR>
struct JSVector  {
    enum { SIZE = NATIVE_VECTOR::SIZE };
    typedef asl::Ptr<typename dom::ValueWrapper<NATIVE_VECTOR>::Type, dom::ThreadingModel>
        NativeValuePtr;

    typedef typename JValueTypeTraits<NATIVE_VECTOR>::elem_type ELEM;
    typedef typename JValueTypeTraits<NATIVE_VECTOR>::vector_type vector_type;
    typedef JSVector Base;

    static
    JSVector * getJSWrapperPtr(JSContext *cx, JSObject *obj) {
        if (JSA_GetClass(cx,obj) != JSVector::Class()) {
            JS_ReportError(cx,"JSVector::getJSWrapperPtr: class type %s expected",JSVector::ClassName());
            return 0;
        }
        JSVector * myJSVector = static_cast<JSVector*>(JS_GetPrivate(cx,obj));
        if (!myJSVector) {
            JS_ReportError(cx,"JSVector::getJSWrapperPtr: internal error, binding object does not exist");
        }
        return myJSVector;
    }

    static
    JSVector & getJSWrapper(JSContext *cx, JSObject *obj) {
        JSVector * myJSWrapper = getJSWrapperPtr(cx,obj);
        if (!myJSWrapper) {
            JS_ReportError(cx,"JSVector::getJSWrapper: internal error, binding object does not exist");
            static JSVector myDummy;
            return myDummy;
        }
        return *myJSWrapper;
    }

    // This functions must be called only on JSObjects containing the correct
    // native ValueBase pointer in their private field
    static
    const NATIVE_VECTOR & getNativeRef(JSContext *cx, JSObject *obj) {
        return JSClassTraits<NATIVE_VECTOR>::getNativeRef(cx, obj);
    }
    static
    NATIVE_VECTOR & openNativeRef(JSContext *cx, JSObject *obj) {
        return JSClassTraits<NATIVE_VECTOR>::openNativeRef(cx, obj);
    }
    static
    void closeNativeRef(JSContext *cx, JSObject *obj) {
        JSClassTraits<NATIVE_VECTOR>::closeNativeRef(cx, obj);
    }

    virtual NATIVE_VECTOR & openNative() {
        return _myOwner->openWriteableValue();
    }
    virtual void closeNative() {
        return _myOwner->closeWriteableValue();
    }
    virtual const NATIVE_VECTOR & getNative() const {
        return _myOwner->getValue();
    }
    virtual NativeValuePtr & getOwner() {
        return _myOwner;
    }
    virtual const NativeValuePtr & getOwner() const {
        return _myOwner;
    }

    static JSBool
    toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a string representation of the vector.");
    DOC_END;
        std::string myStringRep = asl::as_string(getNativeRef(cx,obj));
        JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
        *rval = STRING_TO_JSVAL(myString);
        return JS_TRUE;
    }

    static JSBool
    clone(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Copy constructor");
    DOC_END;
        *rval = OBJECT_TO_JSVAL(Construct(cx,getNativeRef(cx,obj)));
        return JS_TRUE;
    }

    static JSBool
        callMethod(void (NATIVE_VECTOR::*theMethod)(const vector_type &),
         JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        IF_NOISY(AC_TRACE << "JSVector::callMethod()" << std::endl);
        if (argc != 1) {
            JS_ReportError(cx, "JSVector::add: must have 1 argument");
            return JS_FALSE;
        }
        JSObject * myArgument;
        if (JS_ValueToObject(cx, argv[0], &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == Class()) {
                const NATIVE_VECTOR & myVector = getNativeRef(cx,myArgument);
                (openNativeRef(cx,obj).*theMethod)(myVector);
                closeNativeRef(cx, obj);
                return JS_TRUE;
            }
        }
#ifdef NO_TEMPORARY
        JSObject * myArgument;
        if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
            JS_ReportError(cx, "JSVector::callMethod: bad argument type");
            return JS_FALSE;
        }
        jsuint myArrayLength = 0;
        if (!JS_HasArrayLength(cx,myArgument,&myArrayLength)) {
            JS_ReportError(cx, "JSVector::callMethod: argument must be a vector or array");
            return JS_FALSE;
        }

        if (myArrayLength != SIZE) {
            JS_ReportError(cx, "JSVector::callMethod: argument array length must be %d, but is %d", SIZE, myArrayLength);
            return JS_FALSE;
        }
        FixedVector<SIZE,jsval> myArgElems;
        FixedVector<SIZE,jsdouble> myNumbers;
        NATIVE_VECTOR myVector;
        for (int i = 0; i < SIZE;++i) {
            if (!JS_GetProperty(cx,myArgument,as_cstring(i),&myArgElems[i])) {
                JS_ReportError(cx, "JSVector::callMethod: argument[%d] does not exist", i);
                return JS_FALSE;
            }
            if (!JS_ValueToNumber(cx,myArgElems[i],&myNumbers[i])) {
                JS_ReportError(cx, "JSVector::callMethod: argument[%d] is not a number", i);
                return JS_FALSE;
            }
            myVector[i] = ELEM(myNumbers[i]);
        }
#else
        myArgument = Construct(cx, argv[0]);
        if (!myArgument) {
            JS_ReportError(cx, "JSVector::callMethod: failed to construct temporary %s from arguments",
                JValueTypeTraits<NATIVE_VECTOR>::Name().c_str());
            return JS_FALSE;
        }
        const NATIVE_VECTOR & myVector = getNativeRef(cx,myArgument);
#endif
        (openNativeRef(cx,obj).*theMethod)(myVector);
        closeNativeRef(cx, obj);
        return JS_TRUE;
    }
    enum PropertyEnum { PROP_LENGTH = -100, PROP_VALUE };

    static JSBool
    add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Add another vector or a scalar.");
    DOC_PARAM("Number", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("Number", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("Vector", DOC_TYPE_VECTOR3F);
    DOC_END;
        return callMethod(&NATIVE_VECTOR::add, cx, obj, argc, argv, rval);
    }
    static JSBool
    sub(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Subtract another vector or a scalar.");
    DOC_PARAM("Number", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("Number", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("Vector", DOC_TYPE_VECTOR3F);
    DOC_END;
        return callMethod(&NATIVE_VECTOR::sub, cx, obj, argc, argv, rval);
    }
    static JSBool
    mult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Multiply with another vector or a scalar.");
    DOC_PARAM("Number", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("Number", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("Vector", DOC_TYPE_VECTOR3F);
    DOC_END;
        return callMethod(&NATIVE_VECTOR::mult, cx, obj, argc, argv, rval);
    }
    static JSBool
    div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Divide by another vector or a scalar.");
    DOC_PARAM("Number", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("Number", DOC_TYPE_FLOAT);
    DOC_RESET;
    DOC_PARAM("Vector", DOC_TYPE_VECTOR3F);
    DOC_END;
        return callMethod(&NATIVE_VECTOR::div, cx, obj, argc, argv, rval);
    }

#if 1
    static JSBool
    getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
    {
        IF_NOISY(printParams("JSVector::getProperty",cx,obj,id,vp));
        if (JSVAL_IS_INT(id)) {
            // index or length access
            const NATIVE_VECTOR & myNativeVector = getJSWrapper(cx, obj).getNative();
            int myIndex = JSVAL_TO_INT(id);
            if ((myIndex>= 0) && (myIndex<SIZE)) {
                return JS_NewDoubleValue(cx, myNativeVector[myIndex], vp);
            }
            switch (myIndex) {
                case PROP_LENGTH:
                    *vp = INT_TO_JSVAL(SIZE);
                    return JS_TRUE;
                 case PROP_VALUE:
                    *vp = as_jsval(cx,myNativeVector);
                    return JS_TRUE;
           }
        } else {
            // try swizzling
            JSString *myJSStr = JS_ValueToString(cx, id);
            if (!myJSStr) {
                return JS_TRUE;
            }
            std::string myProperty = JS_GetStringBytes(myJSStr);
            if (myProperty.size()>0 && myProperty.size()<=4) {
                //NATIVE_VECTOR & myNativeVector = getNative(cx,obj);
                const NATIVE_VECTOR & myNativeVector = getJSWrapper(cx, obj).getNative();
                std::vector<jsval> mySwizzledArgs(myProperty.size());
                for (int i = 0; i < myProperty.size(); ++i) {
                    int n;
                    double v;
                    switch(myProperty[i]) {
                        case 'x': n = 0; break;
                        case 'y': n = 1; break;
                        case 'z': n = 2; break;
                        case 'w': n = 3; break;
                        case '0': n = -1; v = 0; break;
                        case '1': n = -2; v = 1; break;
                        default: return JS_TRUE;
                    }
                    if (n >= 0) {
                        if (n>=SIZE) {
                            AC_WARNING << "swizzle property char '"<<myProperty[i]<<"' ignored on vector with size "<<SIZE<<std::endl;
                            return JS_TRUE;
                        }
                        v = myNativeVector[n];
                    }

                    if (!JS_NewDoubleValue(cx, v, &mySwizzledArgs[i])) {
                        AC_ERROR << "JS_NewDoubleValue failed"<<std::endl;
                        return JS_FALSE;
                    }
                }
                JSObject *myReturnObject;
                switch (myProperty.size()) {
                    case 1: *vp = mySwizzledArgs[0];
                            return JS_TRUE;
                    case 2: myReturnObject = JSVector<asl::Vector2d>::Construct(cx, mySwizzledArgs.size(), &mySwizzledArgs[0]);
                            break;
                    case 3: myReturnObject = JSVector<asl::Vector3d>::Construct(cx, mySwizzledArgs.size(), &mySwizzledArgs[0]);
                            break;
                    case 4: myReturnObject = JSVector<asl::Vector4d>::Construct(cx, mySwizzledArgs.size(), &mySwizzledArgs[0]);
                            break;
                    default:AC_ERROR << "JSVector::getProperty: internal error, bad index" << std::endl;
                            break;
                }
                *vp = OBJECT_TO_JSVAL(myReturnObject);
            }
        }
        return JS_TRUE;
    }
    static JSBool
    setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
    {
        IF_NOISY(printParams("JSVector::setProperty",cx,obj,id,vp));
        if (JSVAL_IS_INT(id)) {
            //NATIVE_VECTOR & myNativeVector = getNative(cx,obj);
            typename JSClassTraits<NATIVE_VECTOR>::ScopedNativeRef myObjRef(cx, obj);
            int myIndex = JSVAL_TO_INT(id);
            if ((myIndex>= 0) && (myIndex<SIZE)) {
                jsdouble myParam;
                if (JS_ValueToNumber(cx,*vp, &myParam)) {
                    myObjRef.getNative()[myIndex] = ELEM(myParam);
                    return JS_TRUE;
                } else {
                    JS_ReportError(cx, "JSVector::setProperty: index is not a number");
                    return JS_FALSE;
                }
                return JS_TRUE;
            }
            switch (myIndex) {
                case PROP_LENGTH:
                    JS_ReportError(cx, "JSVector::setProperty: length is a read-only property");
                    return JS_TRUE;
                case PROP_VALUE:
                    NATIVE_VECTOR myNativeArg;
                    if (convertFrom(cx, *vp, myNativeArg)) {
                        myObjRef.getNative() = myNativeArg;
                        return JS_TRUE;
                    }
                    JS_ReportError(cx, "JSVector::setProperty: assign to 'value': bad argument");
                    return JS_FALSE;
            }
        } else {
            // try swizzling
            JSString *myJSStr = JS_ValueToString(cx, id);
            if (!myJSStr) {
                return JS_TRUE;
            }
            std::string myProperty = JS_GetStringBytes(myJSStr);
            if (myProperty.size()>0 && myProperty.size()<=SIZE) {
                asl::QuadrupleOf<bool> isUsed(false,false,false,false);
                asl::Vector4i myIndices;
                for (int i = 0; i < myProperty.size(); ++i) {
                    int n;
                    switch(myProperty[i]) {
                        case 'x': n = 0; break;
                        case 'y': n = 1; break;
                        case 'z': n = 2; break;
                        case 'w': n = 3; break;
                        //default: return JS_TRUE;
                    }
                    if (n>=SIZE) {
                        AC_WARNING << "swizzle property char '"<<myProperty[i]<<"' ignored on vector with size "<<SIZE<<std::endl;
                        AC_INFO << " if you do not want to swizzle use a propertyname not starting with x,y,z,w or is longer than "<<SIZE<<" characters"<<std::endl;
                        return JS_TRUE;
                    }
                    myIndices[i] = n;
                    if (isUsed[n]) {
                        AC_WARNING << "swizzle property char '"<<myProperty[i]<<"' already used in property "<<std::endl;
                        return JS_TRUE;
                    }
                    isUsed[n] = true;
                }
                //NATIVE_VECTOR & myNativeVector = getNative(cx,obj);
                typename JSClassTraits<NATIVE_VECTOR>::ScopedNativeRef myObjRef(cx, obj);
                JSObject *myTmpObject;

                switch (myProperty.size()) {
                    case 1: {
                                jsdouble myParam;
                                if (JS_ValueToNumber(cx,*vp, &myParam)) {
                                    myObjRef.getNative()[myIndices[0]] = ELEM(myParam);
                                    return JS_TRUE;
                                }
                                std::cerr << "Warning: Illegal argument type, must be a number" << std::endl;
                                return JS_FALSE;
                            }
                    case 2:
                        myTmpObject = JSVector<asl::Vector2d>::Construct(cx, *vp);
                        if (myTmpObject) {
                            //asl::Vector2d & myVec = JSVector<asl::Vector2d>::getNative(cx,myTmpObject);
                            const asl::Vector2d & myVec = JSVector<asl::Vector2d>::getJSWrapper(cx, myTmpObject).getNative();
                            myObjRef.getNative()[myIndices[0]] = ELEM(myVec[0]);
                            myObjRef.getNative()[myIndices[1]] = ELEM(myVec[1]);
                            return JS_TRUE;
                        }
                        break;
                    case 3:
                        myTmpObject = JSVector<asl::Vector3d>::Construct(cx, *vp);
                        if (myTmpObject) {
                            //asl::Vector3d & myVec = JSVector<asl::Vector3d>::getNative(cx,myTmpObject);
                            const asl::Vector3d & myVec = JSVector<asl::Vector3d>::getJSWrapper(cx, myTmpObject).getNative();
                            myObjRef.getNative()[myIndices[0]] = ELEM(myVec[0]);
                            myObjRef.getNative()[myIndices[1]] = ELEM(myVec[1]);
                            myObjRef.getNative()[myIndices[2]] = ELEM(myVec[2]);
                            return JS_TRUE;
                        }
                        break;
                    case 4:
                        myTmpObject = JSVector<asl::Vector4d>::Construct(cx, *vp);
                        if (myTmpObject) {
                            //asl::Vector4d & myVec = JSVector<asl::Vector4d>::getNative(cx,myTmpObject);
                            const asl::Vector4d & myVec = JSVector<asl::Vector4d>::getJSWrapper(cx, myTmpObject).getNative();
                            myObjRef.getNative()[myIndices[0]] = ELEM(myVec[0]);
                            myObjRef.getNative()[myIndices[1]] = ELEM(myVec[1]);
                            myObjRef.getNative()[myIndices[2]] = ELEM(myVec[2]);
                            myObjRef.getNative()[myIndices[3]] = ELEM(myVec[3]);
                            return JS_TRUE;
                        }
                        break;
                    default:AC_ERROR << "getProperty: internal error, bad index" << std::endl;
                            break;
                }
            }
        }
        return JS_TRUE;
    }
#else
#endif

    static JSBool
    newResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
    {
        IF_NOISY(
            printParams("newresolve",cx,obj,id);
            if (flags & JSRESOLVE_QUALIFIED) {
                std::cerr << "JSRESOLVE_QUALIFIED"<<std::endl;
            }
            if (flags & JSRESOLVE_ASSIGNING) {
                std::cerr << "JSRESOLVE_ASSIGNING"<<std::endl;
            }
            std::cerr << "objp=" << (void*)*objp<<std::endl;
        )

        if (JSVAL_IS_INT(id)) {
            int myID = JSVAL_TO_INT(id);
            if ((myID >= 0) && (myID<2)) {
                *objp = obj;
                IF_NOISY(std::cerr<<"newResolve "<<myID<<std::endl);
                return JS_TRUE;
            }
        }
        *objp = 0;
        return JS_TRUE;
    }


    // --- if char* comparison is used instead of strcmp()
    // the following is needed to make sure the char* are the same
    // in all modules (see Singletonmanager.h)
    // If not, It doesn't hurt either.
    class JSClassNameSingleton :
        public asl::Singleton<JSClassNameSingleton>
    {
        friend class asl::SingletonManager;
    public:
        const std::string & getMyClassName() const {
            return _myClassName;
        }
    protected:
        JSClassNameSingleton() :
            _myClassName(JValueTypeTraits<NATIVE_VECTOR>::Name())
        {}
    private:
        std::string _myClassName;
    };
    static const char * ClassName() {
        static const char * myClassName = JSClassNameSingleton::get().getMyClassName().c_str();
        return myClassName;
    }
    // ---
#if 0
    static const char * ClassName() {
        static std::string myClassName = JValueTypeTraits<NATIVE_VECTOR>::Name();
        return myClassName.c_str();
    }
#endif
    static void finalizeImpl(JSContext *cx, JSObject *obj)
    {
        IF_NOISY_F(std::cerr << "finalize "<<ClassName() << (void*)obj << std::endl);
        JSVector * myImpl = static_cast<JSVector*>(JS_GetPrivate(cx,obj));
        delete myImpl;
    }
    static JSFunctionSpec * Functions() {
        AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<std::endl;
        static JSFunctionSpec myFunctions[] = {
            /* name         native          nargs    */
            {"add",        JSVector::add,       1},
            {"sub",        JSVector::sub,       1},
            {"mult",       JSVector::mult,      1},
            {"div",        JSVector::div,       1},
            {"toString",   JSVector::toString,  0},
            {"clone",      JSVector::clone,     0},
            {0}
        };
        return myFunctions;
    }
    
    static JSPropertySpec * Properties() {
        static JSPropertySpec myProperties[] = {
            {"length",   PROP_LENGTH,      JSPROP_ENUMERATE},
            {"value",   PROP_VALUE,      JSPROP_ENUMERATE},
            {0}
        };
        return myProperties;
    }

    static JSConstIntPropertySpec * ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    static JSPropertySpec * StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    static JSFunctionSpec * StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    // --
    class JSClassSingleton :
        public asl::Singleton<JSClassSingleton>
    {
    public:
        friend class asl::SingletonManager;
        JSClass * getClass() {
            return &_myJSClass;
        }
    private:
        JSClassSingleton() {
            JSClass myTempClass = {
                ClassName(),                           // const char          *name;
                JSCLASS_HAS_PRIVATE |                  // uint32              flags;
                //JSCLASS_HAS_PRIVATE |               /* objects have private slot */
                //JSCLASS_NEW_ENUMERATE |             /* has JSNewEnumerateOp hook */
                //JSCLASS_NEW_RESOLVE //|                 /* has JSNewResolveOp hook */
                //JSCLASS_PRIVATE_IS_NSISUPPORTS |    /* private is (nsISupports *) */
                //JSCLASS_SHARE_ALL_PROPERTIES |      /* all properties are SHARED */
                //JSCLASS_NEW_RESOLVE_GETS_START      //JSNewResolveOp gets starting
                                                      //object in prototype chain
                                                      //passed in via *objp in/out
                                                      //parameter */
                0,
                /* Mandatory non-null function pointer members. */
                NoisyAddProperty,    // JSPropertyOp        addProperty;
                NoisyDelProperty,    // JSPropertyOp        delProperty;
                JSVector::getProperty,         // JSPropertyOp        getProperty;
                JSVector::setProperty,         // JSPropertyOp        setProperty;
                NoisyEnumerate,      // JSEnumerateOp       enumerate;
                NoisyResolve,        // JSResolveOp         resolve;
                //(JSResolveOp)newResolve,  // JSResolveOp         resolve;
                NoisyConvert,        // JSConvertOp         convert;
                JSVector::finalizeImpl,           // JSFinalizeOp        finalize;

                JSCLASS_NO_OPTIONAL_MEMBERS
                /* Optionally non-null members start here. */
                                    // JSGetObjectOps      getObjectOps;
                                    // JSCheckAccessOp     checkAccess;
                                    // JSNative            call;
                                    // JSNative            construct;
                                    // JSXDRObjectOp       xdrObject;
                                    // JSHasInstanceOp     hasInstance;
                                    // JSMarkOp            mark;
                                    // jsword      ;
            };
            _myJSClass = myTempClass;
        }
    private:
        JSClass _myJSClass;
    };

    static JSClass * Class() {
        return JSClassSingleton::get().getClass();
    }

    JSVector()
        : _myOwner(NativeValuePtr(
                new typename dom::ValueWrapper<NATIVE_VECTOR>::Type(0)))
    {}
        //: _myNativeVectorOwner(asl::Ptr<NATIVE_VECTOR>(new NATIVE_VECTOR)),
        //   _myNativeVector(&(*_myNativeVectorOwner)) {}

    JSVector(JSContext * cx, NativeValuePtr theValue)
        : _myOwner(theValue)
        //: _myNativeValueOwner(theValue),
        //  _myNativeVector(dom::dynamic_cast_Value<NATIVE_VECTOR>(&(*_myNativeValueOwner)))
    {
        //if (!_myNativeVector) {
        //    JS_ReportError(cx,"JSVector() for %s : internal type error",ClassName());
        //}
    }
    JSVector(const asl::FixedVector<SIZE,jsdouble> & theArgs)
        : _myOwner(NativeValuePtr(
                new typename dom::ValueWrapper<NATIVE_VECTOR>::Type(0)))
        //: _myNativeVectorOwner(asl::Ptr<NATIVE_VECTOR>(new NATIVE_VECTOR)),
        //  _myNativeVector(&(*_myNativeVectorOwner))
    {
        NATIVE_VECTOR & myVecRef = openNative();
        for (int i = 0; i < SIZE; ++i) {
            myVecRef[i] = ELEM(theArgs[i]);
        }
        IF_NOISY2(AC_TRACE << "JSVector<"<<ClassName()<<"> = "<<myVecRef<< std::endl);
        closeNative();
    }
    /*
    JSVector(asl::Ptr<NATIVE_VECTOR> theValue)
        : _myNativeVectorOwner(theValue),
          _myNativeVector(dom::dynamic_cast_Value<NATIVE_VECTOR>(&(*_myNativeVectorOwner)))
    {}
    */
    JSVector(const NATIVE_VECTOR & theVector)
        : _myOwner(NativeValuePtr(
                new typename dom::ValueWrapper<NATIVE_VECTOR>::Type(theVector, 0)))
        //: _myNativeVectorOwner(asl::Ptr<NATIVE_VECTOR>(new NATIVE_VECTOR(theVector))),
          //_myNativeVector(&(*_myNativeVectorOwner))
    {
    }
    JSVector(const JSVector & theVector)
        : _myOwner(theVector.getOwner())
/*
        : _myNativeVectorOwner(theVector._myNativeVectorOwner),
          _myNativeValueOwner(theVector._myNativeValueOwner),
          _myNativeVector(theVector._myNativeVector)
*/
    {
    }
    JSVector & operator=(const JSVector & theVector) {
        _myOwner = theVector.getOwner();
        /*
        _myNativeVectorOwner = theVector._myNativeVectorOwner;
        _myNativeValueOwner = theVector._myNativeValueOwner;
        _myNativeVector = theVector._myNativeVector;
        */
        return *this;
    }
    ~JSVector() {
        IF_NOISY_F(std::cerr << "~JSVector<"<<ClassName()<<">()" << std::endl);
    }

    static
    JSObject * Construct(JSContext *cx, jsval theVectorArgument) {
        return JS_ConstructObjectWithArguments(cx, Class(), 0, 0, 1, &theVectorArgument);
    }
    static
    JSObject * Construct(JSContext *cx, uintN argc, jsval *argv) {
        JSObject * myNewObj = JS_ConstructObjectWithArguments(cx, Class(), 0, 0, argc, argv);
        JSVector * myJSVector = getJSWrapperPtr(cx, myNewObj);
        if (!myJSVector->getOwner()) {
            NATIVE_VECTOR myNative;
            *myJSVector = JSVector(myNative);
        }
        return myNewObj;
    }

    static
    JSObject * Construct(JSContext *cx, const NATIVE_VECTOR & theVector) {
         JSObject * myNewObj = JS_ConstructObject(cx, Class(), 0, 0);
         JSVector * myJSVector = getJSWrapperPtr(cx, myNewObj);
         *myJSVector = JSVector(theVector);
         return myNewObj;
    }
    static
    JSObject * Construct(JSContext *cx, NativeValuePtr theVector) {
         JSObject * myNewObj = JS_ConstructObject(cx, Class(), 0, 0);
         JSVector * myJSVector = getJSWrapperPtr(cx, myNewObj);
         *myJSVector = JSVector(cx, theVector);
         return myNewObj;
    }
private:
    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a JSVector from a provided array, a given size or constructs an empty one.");
    DOC_END;
        IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << std::endl);
        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
            return JS_FALSE;
        }
        JSVector * myNewObject = 0;
        if (argc == 0) {
            myNewObject=new JSVector();
        } else {
            if (argc == SIZE) {
                asl::FixedVector<SIZE,jsdouble> myArgs;
                for (int i = 0; i < SIZE;++i) {
                    if (!JS_ValueToNumber(cx, argv[i], &myArgs[i])) {
                        JS_ReportError(cx,"JSVector::Constructor: parameter %d must be a number",i);
                        return JS_FALSE;
                    }
                }
                myNewObject = new JSVector(myArgs);
            } else if (argc == 1) {
                JSObject * myArgument;

                if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
                    JS_ReportError(cx,"JSVector<%s>::Constructor: bad argument type",ClassName());
                    return JS_FALSE;
                }
                if (JSA_GetClass(cx,myArgument) == Class()) {
                    myNewObject=new JSVector(getNativeRef(cx, myArgument));
                } else {
                    // try to get values from array
                    jsuint myArrayLength = 0;
                    if (!JS_HasArrayLength(cx,myArgument,&myArrayLength)) {
                        JS_ReportError(cx,"JSVector<%s>::Constructor: argument must be a vector or array",ClassName());
                        return JS_FALSE;
                    }

                    if (myArrayLength != SIZE) {
                        JS_ReportError(cx,"JSVector<%s>::Constructor: argument array length must be %d, but is %d",
                            ClassName(),SIZE, myArrayLength);
                        return JS_FALSE;
                    }
                    asl::FixedVector<SIZE,jsval> myArgElems;
                    asl::FixedVector<SIZE,jsdouble> myNumbers;
                    NATIVE_VECTOR myVector;
                    for (int i = 0; i < SIZE;++i) {
    #ifdef USE_SLOW_PROPERTY_LOOKUP
                        if (!JS_GetProperty(cx,myArgument,as_cstring(i),&myArgElems[i])) {
                            JS_ReportError(cx,"JSVector<%s>::Constructor: argument %d does not exist",ClassName(),i);
                            return JS_FALSE;
                        }
    #else
                        if (!JS_GetElement(cx,myArgument,i,&myArgElems[i])) {
                            JS_ReportError(cx,"JSVector<%s>::Constructor: argument %d does not exist",ClassName(),i);
                            return JS_FALSE;
                        }
    #endif

                        if (!JS_ValueToNumber(cx,myArgElems[i],&myNumbers[i]) || JSDOUBLE_IS_NaN(myNumbers[i])) {
                            JS_ReportError(cx,"JSVector<%s>::Constructor: argument %d is not a number",ClassName(),i);
                            return JS_FALSE;
                        }
                        myVector[i] = ELEM(myNumbers[i]);
                    }
                    IF_NOISY2(AC_TRACE << "Constructor myNumbers =" << myNumbers << std::endl);
                    myNewObject = new JSVector(myNumbers);
                }
            }
        }

        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
        JS_ReportError(cx,"JSVector<%s>::Constructor: must have either 0 or %d parameters, but got %d",ClassName(),SIZE,argc);
        return JS_FALSE;
    }
public:
#if 1
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
        DOC_MODULE_CREATE("math",JSVector);
        JSObject * myProtoObj = JS_InitClass(cx, theGlobalObject, NULL, Class(),
                /* native constructor function and min arg count */
                Constructor, 0,
                /* prototype object properties and methods -- these
                will be "inherited" by all instances through
                delegation up the instance's prototype link. */
                Properties(), Functions(),

                /* class constructor properties and methods */
                //static_props, static_methods
                0,0
                );
        return myProtoObj;
    }
#endif
private:
    /*
    asl::Ptr<NATIVE_VECTOR> _myNativeVectorOwner;//  smart pointer for holding reference to an owned native vector;
                                                 // is zero when native vector is owned or inside a dom::Value
    dom::ValuePtr _myNativeValueOwner; // alternative smart pointer for holding reference to a dom::Value;
                                       // is zero when native vector is not inside a dom::Value
    NATIVE_VECTOR * _myNativeVector;
    */
    NativeValuePtr _myOwner;
};
#define DEFINE_VECTOR_CLASS_TRAITS(TYPE) \
template <> \
struct JSClassTraits<TYPE> : public JSClassTraitsWrapper<TYPE, JSVector<TYPE> > {};

DEFINE_VECTOR_CLASS_TRAITS(asl::Vector2f)
DEFINE_VECTOR_CLASS_TRAITS(asl::Vector3f)
DEFINE_VECTOR_CLASS_TRAITS(asl::Vector4f)

DEFINE_VECTOR_CLASS_TRAITS(asl::Vector2d)
DEFINE_VECTOR_CLASS_TRAITS(asl::Vector3d)
DEFINE_VECTOR_CLASS_TRAITS(asl::Vector4d)

DEFINE_VECTOR_CLASS_TRAITS(asl::Vector2i)
DEFINE_VECTOR_CLASS_TRAITS(asl::Vector3i)
DEFINE_VECTOR_CLASS_TRAITS(asl::Vector4i)

DEFINE_VECTOR_CLASS_TRAITS(asl::Point2f)
DEFINE_VECTOR_CLASS_TRAITS(asl::Point3f)
DEFINE_VECTOR_CLASS_TRAITS(asl::Point4f)

DEFINE_VECTOR_CLASS_TRAITS(asl::Point2d)
DEFINE_VECTOR_CLASS_TRAITS(asl::Point3d)
DEFINE_VECTOR_CLASS_TRAITS(asl::Point4d)

DEFINE_VECTOR_CLASS_TRAITS(asl::Point2i)
DEFINE_VECTOR_CLASS_TRAITS(asl::Point3i)
DEFINE_VECTOR_CLASS_TRAITS(asl::Point4i)

template <class VECTOR_NF>
template <template <class> class VECTOR, class NUMBER>
bool Converter<VECTOR_NF>::convertFromVN(JSContext *cx, jsval theValue, 
    VECTOR<NUMBER> & theVector) 
{
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myValObj = JSVAL_TO_OBJECT(theValue);
        if (JSVector<VECTOR<NUMBER> >::Class() != JSA_GetClass(cx,myValObj)) {
            myValObj = JSVector<VECTOR<NUMBER> >::Construct(cx, theValue);
        }
        if (myValObj) {
            //VECTOR<NUMBER> & myNativeArg = JSVector<VECTOR<NUMBER> >::getNative(cx, myValObj);
            const VECTOR<NUMBER> & myNativeArg = JSVector<VECTOR<NUMBER> >::getJSWrapper(cx, myValObj).getNative();
            theVector = myNativeArg;
            return true;
        } else {
            JS_ReportError(cx,"convertFrom VECTOR<NUMBER>: bad argument type, Vector of size %d expected",
                    JValueTypeTraits<VECTOR<NUMBER> >::SIZE);
            return false;
        }
    }
    return false;
}

} // namespace

#endif
