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

// own header
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

template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector2i> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector3i> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector4i> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector2f> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector3f> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector4f> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector2d> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector3d> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Vector4d> & theVector);

template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point2i> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point3i> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point4i> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point2f> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point3f> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point4f> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point2d> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point3d> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<Point4d> & theVector);

template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector2i> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector3i> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector4i> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector2f> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector3f> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector4f> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector2d> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector3d> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Vector4d> & theVector);

template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point2i> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point3i> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point4i> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point2f> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point3f> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point4f> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point2d> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point3d> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<Point4d> & theVector);

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
    if (JSVAL_IS_OBJECT(theValue) && !JSVAL_IS_NULL(theValue)) {
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
