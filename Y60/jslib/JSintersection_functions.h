//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef __y60_jslib_jsintersection_functions_h_included_
#define __y60_jslib_jsintersection_functions_h_included_

#include <y60/JScppUtils.h>

#include <y60/Primitive.h>
#include <y60/Scene.h>

namespace jslib {

jsval as_jsval(JSContext *cx, const y60::Primitive::Intersection & theIntersection, 
        const asl::Matrix4f & myTransformation, const asl::Matrix4f & myInverseTransformation);

jsval as_jsval(JSContext *cx, const y60::Primitive::IntersectionList & theVector, 
        const asl::Matrix4f & theMatrix, const asl::Matrix4f & theInverseMatrix);

jsval as_jsval(JSContext *cx, const y60::IntersectionInfo  & theInfo);

jsval as_jsval(JSContext *cx, const asl::SweptSphereContact<float> & theContact);

jsval as_jsval(JSContext *cx, const y60::Primitive::SphereContacts & theContacts, 
        const asl::Matrix4f & theTransformation);

jsval as_jsval(JSContext *cx, const y60::Primitive::SphereContactsList & theVector, 
        const asl::Matrix4f & theTransformation);

jsval as_jsval(JSContext *cx, const y60::CollisionInfo & theInfo);

}

#endif
