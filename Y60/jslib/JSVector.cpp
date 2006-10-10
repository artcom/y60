//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSVector.h"
#include "JSVector.impl"

using namespace asl;

namespace jslib {
 
template struct JSVector<asl::Vector2i>;
template struct JSVector<asl::Vector3i>;
template struct JSVector<asl::Vector4i>;
template struct JSVector<asl::Vector2f>;
template struct JSVector<asl::Vector3f>;
template struct JSVector<asl::Vector4f>;
template struct JSVector<asl::Vector2d>;
template struct JSVector<asl::Vector3d>;
template struct JSVector<asl::Vector4d>;

template struct JSVector<asl::Point2i>;
template struct JSVector<asl::Point3i>;
template struct JSVector<asl::Point4i>;
template struct JSVector<asl::Point2f>;
template struct JSVector<asl::Point3f>;
template struct JSVector<asl::Point4f>;
template struct JSVector<asl::Point2d>;
template struct JSVector<asl::Point3d>;
template struct JSVector<asl::Point4d>;

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

template <class VECTOR_NF>
template <template <class> class VECTOR, class NUMBER>
bool Converter<VECTOR_NF>::convertFromVN(JSContext *cx, jsval theValue,
    VECTOR<NUMBER> & theVector)
{
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myValObj = JSVAL_TO_OBJECT(theValue);
        if (JSVector<VECTOR<NUMBER> >::Class() != JSA_GetClass(cx, myValObj)) {
            myValObj = JSVector<VECTOR<NUMBER> >::Construct(cx, theValue);
        }
        if (myValObj) {
            const VECTOR<NUMBER> & myNativeArg = JSVector<VECTOR<NUMBER> >::getJSWrapper(cx, myValObj).getNative();
            theVector = myNativeArg;
            return true;
        } else {
            // [CH] The caller should be responsible for error reporting, otherwise we cannot overload functions, properly
            /*
            JS_ReportError(cx,"convertFrom VECTOR<NUMBER>: bad argument type, Vector of size %d expected",
                    JValueTypeTraits<VECTOR<NUMBER> >::SIZE);
            */
            return false;
        }
    }
    return false;
}

}
