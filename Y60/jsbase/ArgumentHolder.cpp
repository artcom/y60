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

#include "JSWrapper.h"
#include "JSNode.h"
#include "JSDomEvent.h"
#include "JSVector.h"
#include "JSQuaternion.h"
#include "JSBox.h"
#include "JSBox2f.h"
#include "JSSphere.h"
#include "JSMatrix.h"
#include "JSBlock.h"
#include "ArgumentHolder.impl"

#include <string>
#include <vector>

namespace jslib {
    template struct ArgumentHolder<bool>;
    template struct ArgumentHolder<short>;
    template struct ArgumentHolder<unsigned short>;
    template struct ArgumentHolder<char>;
    template struct ArgumentHolder<unsigned char>;
    template struct ArgumentHolder<int>;
    template struct ArgumentHolder<unsigned int>;
    template struct ArgumentHolder<unsigned int const &>;
    template struct ArgumentHolder<long>;
    template struct ArgumentHolder<long const &>;
    template struct ArgumentHolder<unsigned long>;
    template struct ArgumentHolder<unsigned long const &>;
    template struct ArgumentHolder<float>;
    template struct ArgumentHolder<float const &>;
    template struct ArgumentHolder<double>;
    template struct ArgumentHolder<double const &>;

    template struct ArgumentHolder<asl::Vector2<int> >;
    template struct ArgumentHolder<asl::Vector3<int> >;
    template struct ArgumentHolder<asl::Vector4<int> >;
    template struct ArgumentHolder<asl::Vector2<float> >;
    template struct ArgumentHolder<asl::Vector3<float> >;
    template struct ArgumentHolder<asl::Vector4<float> >;
    template struct ArgumentHolder<asl::Vector2<double> >;
    template struct ArgumentHolder<asl::Vector3<double> >;
    template struct ArgumentHolder<asl::Vector4<double> >;
    template struct ArgumentHolder<asl::Point2<float> >;
    template struct ArgumentHolder<asl::Point3<float> >;
    template struct ArgumentHolder<asl::Point4<float> >;
    template struct ArgumentHolder<asl::Time>;
 
    template struct ArgumentHolder<asl::Vector2<int> const &>;
    template struct ArgumentHolder<asl::Vector3<int> const &>;
    template struct ArgumentHolder<asl::Vector4<int> const &>;
    template struct ArgumentHolder<asl::Vector2<float> const &>;
    template struct ArgumentHolder<asl::Vector3<float> const &>;
    template struct ArgumentHolder<asl::Vector4<float> const &>;
    template struct ArgumentHolder<asl::Vector2<double> const &>;
    template struct ArgumentHolder<asl::Vector3<double> const &>;
    template struct ArgumentHolder<asl::Vector4<double> const &>;

    template struct ArgumentHolder<asl::Point2<float> const &>;
    template struct ArgumentHolder<asl::Point3<float> const &>;
    template struct ArgumentHolder<asl::Point4<float> const &>;
    template struct ArgumentHolder<asl::Time const&>;
    
    template struct ArgumentHolder<std::vector<unsigned int> const &>;
    template struct ArgumentHolder<std::vector<float> >;
    template struct ArgumentHolder<std::vector<std::string> const &>;

    template struct ArgumentHolder<asl::Quaternion<float> const &>;
    template struct ArgumentHolder<asl::Sphere<float> const &>;
    template struct ArgumentHolder<asl::Box2<float> const &>;
    template struct ArgumentHolder<asl::Box3<float> const &>;
    template struct ArgumentHolder<asl::Matrix4<float> const &>;

    template struct ArgumentHolder<std::string>;
    template struct ArgumentHolder<std::string const &>;
    template struct ArgumentHolder<dom::Node const &>;
    template struct ArgumentHolder<dom::NodePtr>;
    template struct ArgumentHolder<dom::NodePtr const>;
    template struct ArgumentHolder<dom::NodePtr &>;
    template struct ArgumentHolder<dom::NodePtr const &>;
    template struct ArgumentHolder<dom::Node::CloneDepth>;
    template struct ArgumentHolder<dom::EventListenerPtr>;
    template struct ArgumentHolder<dom::EventPtr>;

    template struct ResultConverter<bool>;
    template struct ResultConverter<unsigned short>;
    template struct ResultConverter<unsigned long>;
    template struct ResultConverter<int>;
    template struct ResultConverter<unsigned int>;
    template struct ResultConverter<long>;
    template struct ResultConverter<float>;
    template struct ResultConverter<double>;
    template struct ResultConverter<asl::Vector2<int> >;
    template struct ResultConverter<asl::Vector3<int> >;
    template struct ResultConverter<asl::Vector4<int> >;
    template struct ResultConverter<asl::Vector2<float> >;
    template struct ResultConverter<asl::Vector3<float> >;
    template struct ResultConverter<asl::Vector4<float> >;
    template struct ResultConverter<asl::Vector2<double> >;
    template struct ResultConverter<asl::Vector3<double> >;
    template struct ResultConverter<asl::Vector4<double> >;
    template struct ResultConverter<std::vector<asl::Vector3<float> > >;
   
    template struct ResultConverter<asl::Point2<int> >;
    template struct ResultConverter<asl::Point3<int> >;
    template struct ResultConverter<asl::Point4<int> >;
    template struct ResultConverter<asl::Point2<float> >;
    template struct ResultConverter<asl::Point3<float> >;
    template struct ResultConverter<asl::Point4<float> >;
    template struct ResultConverter<asl::Point2<double> >;
    template struct ResultConverter<asl::Point3<double> >;
    template struct ResultConverter<asl::Point4<double> >;
    
    template struct ResultConverter<std::vector<asl::Point3<float> > >;
    template struct ResultConverter<asl::Quaternion<float> >;
    template struct ResultConverter<dom::NodePtr>;
    template struct ResultConverter<dom::ValuePtr>;
}
