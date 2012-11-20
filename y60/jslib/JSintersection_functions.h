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

#ifndef __y60_jslib_jsintersection_functions_h_included_
#define __y60_jslib_jsintersection_functions_h_included_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JScppUtils.h>

#include <y60/scene/Primitive.h>
#include <y60/scene/Scene.h>

namespace jslib {

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const y60::Primitive::Intersection & theIntersection,
        const asl::Matrix4f & myTransformation, const asl::Matrix4f & myInverseTransformation);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const y60::Primitive::IntersectionList & theVector,
        const asl::Matrix4f & theMatrix, const asl::Matrix4f & theInverseMatrix);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const y60::IntersectionInfo  & theInfo);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const asl::SweptSphereContact<float> & theContact);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const y60::Primitive::SphereContacts & theContacts,
        const asl::Matrix4f & theTransformation);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const y60::Primitive::SphereContactsList & theVector,
        const asl::Matrix4f & theTransformation);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, const y60::CollisionInfo & theInfo);

}

#endif
