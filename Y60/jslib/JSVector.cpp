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
#include "JScppUtils.impl"

using namespace asl;

namespace jslib {
 
template struct JSVector<Vector2i>;
template struct JSVector<Vector3i>;
template struct JSVector<Vector4i>;
template struct JSVector<Vector2f>;
template struct JSVector<Vector3f>;
template struct JSVector<Vector4f>;
template struct JSVector<Vector2d>;
template struct JSVector<Vector3d>;
template struct JSVector<Vector4d>;

template struct JSVector<Point2i>;
template struct JSVector<Point3i>;
template struct JSVector<Point4i>;
template struct JSVector<Point2f>;
template struct JSVector<Point3f>;
template struct JSVector<Point4f>;
template struct JSVector<Point2d>;
template struct JSVector<Point3d>;
template struct JSVector<Point4d>;

template jsval as_jsval(JSContext *cx, const std::vector<Vector2i> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector3i> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector4i> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector2f> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector3f> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector4f> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector2d> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector3d> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Vector4d> & theVector);

template jsval as_jsval(JSContext *cx, const std::vector<Point2i> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point3i> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point4i> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point2f> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point3f> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point4f> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point2d> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point3d> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<Point4d> & theVector);

template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector2i> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector3i> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector4i> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector2f> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector3f> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector4f> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector2d> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector3d> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector4d> & theVector);

template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point2i> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point3i> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point4i> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point2f> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point3f> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point4f> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point2d> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point3d> & theVector);
template bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point4d> & theVector);

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

DEFINE_CONVERT_FROM(Vector2f)
DEFINE_CONVERT_FROM(Vector3f)
DEFINE_CONVERT_FROM(Vector4f)

DEFINE_CONVERT_FROM(Vector2d)
DEFINE_CONVERT_FROM(Vector3d)
DEFINE_CONVERT_FROM(Vector4d)

DEFINE_CONVERT_FROM(Vector2i)
DEFINE_CONVERT_FROM(Vector3i)
DEFINE_CONVERT_FROM(Vector4i)

DEFINE_CONVERT_FROM(Point2f)
DEFINE_CONVERT_FROM(Point3f)
DEFINE_CONVERT_FROM(Point4f)

DEFINE_CONVERT_FROM(Point2d)
DEFINE_CONVERT_FROM(Point3d)
DEFINE_CONVERT_FROM(Point4d)

DEFINE_CONVERT_FROM(Point2i)
DEFINE_CONVERT_FROM(Point3i)
DEFINE_CONVERT_FROM(Point4i)

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
