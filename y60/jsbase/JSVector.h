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

#ifndef _Y60_ACXPSHELL_JSVECTOR_INCLUDED_
#define _Y60_ACXPSHELL_JSVECTOR_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JScppUtils.h"
#include "JSWrapper.h"

#include <asl/base/begin_end.h>
#include <asl/base/settings.h>
#include <asl/base/Singleton.h>
#include <asl/math/Vector234.h>
#include <y60/base/DataTypes.h>

namespace jslib {

template<class NATIVE_VECTOR> struct JSVector;

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector2i & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector3i & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector4i & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector2f & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector3f & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector4f & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector2d & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector3d & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Vector4d & theVector);

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point2i & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point3i & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point4i & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point2f & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point3f & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point4f & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point2d & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point3d & theVector);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Point4d & theVector);

namespace detail {
    template <template<class> class NATIVE_VECTOR, class NUMBER>
    struct as_jsval_helper {
        static jsval as_jsval(JSContext *cx, const NATIVE_VECTOR<NUMBER> & theValue) {
            JSObject * myReturnObject = JSVector<NATIVE_VECTOR<NUMBER> >::Construct(cx, theValue);
            return OBJECT_TO_JSVAL(myReturnObject);
        }
    };
    template <template<class> class NATIVE_VECTOR>
    struct as_jsval_helper<NATIVE_VECTOR,asl::AC_SIZE_TYPE> {
        static jsval as_jsval(JSContext *cx, const NATIVE_VECTOR<asl::AC_SIZE_TYPE> & theValue) {
            NATIVE_VECTOR<int> tmp( reinterpret_cast<const int*>(asl::begin_ptr(theValue))
                                  , reinterpret_cast<const int*>(asl::end_ptr  (theValue)) );
            JSObject * myReturnObject = JSVector<NATIVE_VECTOR<int> >::Construct(cx, tmp);
            return OBJECT_TO_JSVAL(myReturnObject);
        }
    };
}

template <template<class> class NATIVE_VECTOR, class NUMBER>
inline jsval as_jsval(JSContext *cx, const NATIVE_VECTOR<NUMBER> & theValue) {
    return detail::as_jsval_helper<NATIVE_VECTOR,NUMBER>::as_jsval(cx,theValue);
}

template <template<class> class NATIVE_VECTOR, class NUMBER>
inline jsval as_jsval(JSContext *cx, dom::ValuePtr theValue, NATIVE_VECTOR<NUMBER> *) {
    JSObject * myReturnObject = JSVector<NATIVE_VECTOR<NUMBER> >::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

template <template <class> class NATIVE_VECTOR, class NUMBER>
inline jsval as_jsval(JSContext *cx,
                      asl::Ptr<dom::VectorValue<NATIVE_VECTOR<NUMBER> >,
                      dom::ThreadingModel> theValue)
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
    typedef typename asl::Ptr<typename dom::ValueWrapper<NATIVE_VECTOR>::Type, dom::ThreadingModel>
        NativeValuePtr;

    typedef typename JValueTypeTraits<NATIVE_VECTOR>::elem_type ELEM;
    typedef typename JValueTypeTraits<NATIVE_VECTOR>::vector_type vector_type;
    typedef JSVector Base;
    typedef JSBool (*NativePropertyGetter)(const NATIVE_VECTOR & theNative, JSContext *cx, jsval *vp);
    typedef JSBool (*NativePropertySetter)(const NATIVE_VECTOR & theNative, JSContext *cx, jsval *vp);

    static
    JSVector * getJSWrapperPtr(JSContext *cx, JSObject *obj);

    static
    JSVector & getJSWrapper(JSContext *cx, JSObject *obj);

    Y60_JSBASE_DECL static
    bool matchesClassOf(JSContext *cx, jsval theVal);

    // This functions must be called only on JSObjects containing the correct
    // native ValueBase pointer in their private field
    static
    const NATIVE_VECTOR & getNativeRef(JSContext *cx, JSObject *obj);
    static
    NATIVE_VECTOR & openNativeRef(JSContext *cx, JSObject *obj);
    static
    void closeNativeRef(JSContext *cx, JSObject *obj);

    virtual NATIVE_VECTOR & openNative();
    virtual void closeNative();
    virtual const NATIVE_VECTOR & getNative() const;
    virtual NativeValuePtr & getOwner();
    virtual const NativeValuePtr & getOwner() const;

    static JSBool
    toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    clone(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    callMethod(void (NATIVE_VECTOR::*theMethod)(const vector_type &),
         JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    enum PropertyEnum { PROP_LENGTH = -100, PROP_VALUE };

    static JSBool
    add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    sub(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    mult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    newResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);

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

    static const char * ClassName();

    static void finalizeImpl(JSContext *cx, JSObject *obj);

    static JSFunctionSpec * Functions();

    static JSPropertySpec * Properties();

    static JSConstIntPropertySpec * ConstIntProperties();

    static JSPropertySpec * StaticProperties();

    static JSFunctionSpec * StaticFunctions();

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

    static JSClass * Class();

    JSVector();

    JSVector(JSContext * cx, NativeValuePtr theValue);

    JSVector(const asl::FixedVector<SIZE,jsdouble> & theArgs);

    JSVector(const NATIVE_VECTOR & theVector);

    JSVector(const JSVector & theVector);

    JSVector & operator=(const JSVector & theVector);

    virtual ~JSVector();

    Y60_JSBASE_DECL static
    JSObject * Construct(JSContext *cx, jsval theVectorArgument);

    Y60_JSBASE_DECL static
    JSObject * Construct(JSContext *cx, uintN argc, jsval *argv);

    Y60_JSBASE_DECL static
    JSObject * Construct(JSContext *cx, const NATIVE_VECTOR & theVector);

    Y60_JSBASE_DECL static
    JSObject * Construct(JSContext *cx, NativeValuePtr theVector);

    //TODO: implement the following three functions according to those in JSWrapper
    static bool registerMethod(const char * theName, JSNative theFunction, uint8 theMinArgCount) {
        //return registerFunction(JSClassSingleton::get().getMethodsRef(), theName, theFunction, theMinArgCount);
        return false;
    }
    static bool registerStaticFunction(const char * theName, JSNative theFunction, uint8 theMinArgCount) {
        //return registerFunction(JSClassSingleton::get().getStaticFunctionsRef(), theName, theFunction, theMinArgCount);
        return false;
    }
    static bool registerProperty(const char * theName, JSPropertyOp theGetter, JSPropertyOp theSetter) {
        // return registerProperty(JSClassSingleton::get().getPropertiesRef(), theName, theGetter, theSetter);
        return false;
    }
private:
    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
public:
    Y60_JSBASE_DECL static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
private:
    NativeValuePtr _myOwner;
};
#define DEFINE_VECTOR_CLASS_TRAITS(TYPE) \
template <> \
struct Y60_JSBASE_DECL JSClassTraits<TYPE> : public JSClassTraitsWrapper<TYPE, JSVector<TYPE> > {};


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

#undef DEFINE_VECTOR_CLASS_TRAITS

} // namespace

#endif
