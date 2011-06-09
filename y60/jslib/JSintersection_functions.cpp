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

// own header
#include "JSintersection_functions.h"

#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSTriangle.h>
#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JScppUtils.impl>

/*
found at http://www.gamedev.net/columns/hardcore/dxshader2/page3.asp

So the bullet-proof way to use normals, is to transform the transpose of the inverse
of the matrix, that is used to transform the object. If the matrix used to transform
the object is called M, then we must use the matrix, N, below to transform the normals
of this object.

N = transpose( inverse(M) )

    The normal can be transformed with the transformation matrix (usually the world matrix),
    that is used to transform the object in the following cases:

        * Matrix formed from rotations (orthogonal matrix), because the inverse of an orthogonal matrix is its transpose
        * Matrix formed from rotations and translation (rigid-body transforms), because translations do not affect vector direction
        * Matrix formed from rotations and translation and uniform scalings, because such scalings affect only the length
          of the transformed normal, not its direction. A uniform scaling is simply a matrix which uniformly increases
          or decreases the object’s size, vs. a non-uniform scaling, which can stretch or squeeze an object. If uniform
          scalings are used, then the normals do have to be renormalized.

    Therefore using the world matrix would be sufficient in this example.
*/

// return intersection point and primitive
namespace jslib {

template jsval as_jsval(JSContext *cx, const std::vector<y60::IntersectionInfo> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<y60::CollisionInfo> & theVector);

static bool fillPrimitiveElement(JSContext *cx, JSObject * myObject,
                                 const y60::Primitive::Element & theElement,
                                 const asl::Matrix4f * theTransformation)
{
    //const y60::VertexData3f::VertexDataVector & myPositions = theElement._myPrimitive->getVertexData(y60::POSITIONS).getVertexDataCast<asl::Vector3f>();
    const asl::Ptr<y60::ConstVertexDataAccessor<asl::Vector3f> > myPositionAccessor = theElement._myPrimitive->getConstLockingPositionsAccessor();
    const y60::VertexData3f & myPositions = myPositionAccessor->get();

    if (theElement._myVertexCount == 3) {
        const asl::Triangle<float> * myTriangle = asl::asTriangle(&asl::asPoint(myPositions[theElement._myStartVertex]));
        if (theTransformation) {
            if (!JS_DefineProperty(cx, myObject, "primitive", as_jsval(cx,*myTriangle * (*theTransformation)), 0,0, JSPROP_ENUMERATE)) return false;
        } else {
            if (!JS_DefineProperty(cx, myObject, "primitive", as_jsval(cx,*myTriangle), 0,0, JSPROP_ENUMERATE)) return false;
        }
    }
    if (theElement._myVertexCount == 4) {
        const asl::Point3f * myPolygonPtr = &asl::asPoint(myPositions[theElement._myStartVertex]);
        std::vector<asl::Point3f> myPolygon(myPolygonPtr, myPolygonPtr + theElement._myVertexCount);
        if (theTransformation) {
            for (unsigned int i = 0; i < theElement._myVertexCount; ++i) {
                myPolygon[i] = myPolygon[i] * (*theTransformation);
            }
        }
        if (!JS_DefineProperty(cx, myObject, "primitive", as_jsval(cx,myPolygon), 0,0, JSPROP_ENUMERATE)) return false;
        const asl::Triangle<float> * myFirstTriangle = asl::asTriangle(&myPolygon[0]);
        jsval myPrimitive;
        if (!JS_GetProperty(cx, myObject, "primitive", &myPrimitive)) return false;
        if (!JS_DefineProperty(cx, JSVAL_TO_OBJECT(myPrimitive), "normal", as_jsval(cx,myFirstTriangle->plane().normal), 0,0, JSPROP_ENUMERATE)) return false;
    }
    return true;
}

jsval as_jsval(JSContext *cx,
               const y60::Primitive::Intersection & theIntersection,
               const asl::Matrix4f & myTransformation,
               const asl::Matrix4f & myInverseTransformation)
{
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "startvertex", as_jsval(cx, theIntersection._myElement._myStartVertex), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    //if (!JS_DefineProperty(cx, myReturnObject, "domstartvertex", as_jsval(cx, theIntersection._myElement._myStartVertex + theIntersection._myElement._myPrimitive->getDomIndex()), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "domstartvertex", as_jsval(cx, theIntersection._myElement._myStartVertex), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "rawposition", as_jsval(cx, theIntersection._myPosition), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "position", as_jsval(cx, theIntersection._myPosition * myTransformation), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "rawnormal", as_jsval(cx, theIntersection._myNormal), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "normal", as_jsval(cx, asl::transformedNormal(theIntersection._myNormal, myTransformation)),  0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;

    if (!fillPrimitiveElement(cx, myReturnObject, theIntersection._myElement, &myTransformation)) {
        return JSVAL_VOID;
    }
    return rval;
}

jsval as_jsval(JSContext *cx,
               const y60::Primitive::IntersectionList & theVector,
               const asl::Matrix4f & theMatrix, const asl::Matrix4f & theInverseMatrix)
{
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    for (y60::Primitive::IntersectionList::size_type i = 0; i < theVector.size(); ++i) {
        jsval myValue = as_jsval(cx, theVector[i], theMatrix, theInverseMatrix);
        if (!JS_SetElement(cx, myReturnObject, i, &myValue)) {
            return JS_FALSE;
        }
    }
    return rval;
}

jsval as_jsval(JSContext *cx,
               const y60::IntersectionInfo  & theInfo)
{
    if (theInfo._myShape == 0) {
        return JSVAL_VOID;
    }
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "body",              as_jsval(cx, theInfo._myBody),                  0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "shape",             as_jsval(cx, theInfo._myShape->getXmlNode()),   0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "transform",         as_jsval(cx, theInfo._myTransformation),        0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "inverse_transform", as_jsval(cx, theInfo._myInverseTransformation), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "intersections",     as_jsval(cx, *theInfo._myPrimitiveIntersections, theInfo._myTransformation, theInfo._myInverseTransformation),0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    return rval;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CollisionInfo Layout:
//
// CollisionInfo           = Result of collideWithWorld or collideWithWorldOnce
//   - body                = Collided body node
//   - shape               = Collided shape node
//   - transform           = Collided body transformation matrix
//   - inverse_transform   = Collided body inverse transformation matrix
//   - collisions          = Array of SphereContacts
//      - primitive        = Triangle or Array of Vector3f
//      - count            = Number of collisions
//      - min              = Minimum SweptSphereContact
//          - t            = Position between 0 and 1 on swept sphere ray
//          - position     = Contact point between sphere and primitive
//          - normal       = Contact normal between sphere and primitive
//          - sphereCenter = Center of contact sphere
//          - sphereNormal = (?)
//      - max              = Maximum SweptSphereContact
//          - [...]        = (see above)

jsval as_jsval(JSContext *cx,
               const asl::SweptSphereContact<float> & theContact)
{
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "t", as_jsval(cx, theContact.t), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "position", as_jsval(cx, theContact.contactPoint), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "normal", as_jsval(cx, theContact.contactNormal), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "sphereCenter", as_jsval(cx, theContact.contactSphereCenter), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "sphereNormal", as_jsval(cx, theContact.contactSphereNormal), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "contactType", as_jsval(cx, theContact.contactType), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;

   return rval;
}

jsval as_jsval(JSContext *cx,
               const y60::Primitive::SphereContacts & theContacts,
               const asl::Matrix4f & theTransformation)
{
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!fillPrimitiveElement(cx, myReturnObject, theContacts._myElement, &theTransformation)) {
        return JSVAL_VOID;
    }
    if (!JS_DefineProperty(cx, myReturnObject, "count", as_jsval(cx, theContacts._myNumberOfContacts), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "min", as_jsval(cx, theContacts._myMinContact), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "max", as_jsval(cx, theContacts._myMaxContact), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;

   return rval;
}

jsval as_jsval(JSContext *cx,
               const y60::Primitive::SphereContactsList & theVector,
               const asl::Matrix4f & theTransformation)
{
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    for (y60::Primitive::SphereContactsList::size_type i = 0; i < theVector.size(); ++i) {
        jsval myValue = as_jsval(cx, theVector[i], theTransformation);
        if (!JS_SetElement(cx, myReturnObject, i, &myValue)) {
            return JS_FALSE;
        }
    }
    return rval;
}

jsval as_jsval(JSContext *cx,
               const y60::CollisionInfo & theInfo)
{
    if (theInfo._myShape == 0) {
        return JSVAL_VOID;
    }
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "body",              as_jsval(cx, theInfo._myBody),                   0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "shape",             as_jsval(cx, theInfo._myShape->getXmlNode()),    0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "transform",         as_jsval(cx, theInfo._myTransformation),         0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "inverse_transform", as_jsval(cx, theInfo._myInverseTransformation),  0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "collisions",        as_jsval(cx,*theInfo._myPrimitiveSphereContacts, theInfo._myTransformation),0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    return rval;
}

} // namespace

