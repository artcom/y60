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
#include "ArgumentHolder.h"

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
    template struct Y60_JSBASE_EXPORT ArgumentHolder<bool>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<short>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<unsigned short>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<char>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<unsigned char>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<int>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<unsigned int>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<unsigned int const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<long>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<long const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<unsigned long>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<unsigned long const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<float>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<float const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<double>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<double const &>;
                 
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector2<int> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector3<int> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector4<int> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector2<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector3<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector4<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector2<double> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector3<double> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector4<double> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Point2<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Point3<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Point4<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Time>;
 
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector2<int> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector3<int> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector4<int> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector2<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector3<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector4<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector2<double> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector3<double> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Vector4<double> const &>;
                    
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Point2<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Point3<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Point4<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Time const&>;
                
    template struct Y60_JSBASE_EXPORT ArgumentHolder<std::vector<unsigned int> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<std::vector<float> >;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<std::vector<std::string> const &>;
                 
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Quaternion<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Sphere<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Box2<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Box3<float> const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<asl::Matrix4<float> const &>;
               
    template struct Y60_JSBASE_EXPORT ArgumentHolder<std::string>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<std::string const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::Node const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::NodePtr>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::NodePtr const>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::NodePtr &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::NodePtr const &>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::Node::CloneDepth>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::EventListenerPtr>;
    template struct Y60_JSBASE_EXPORT ArgumentHolder<dom::EventPtr>;

    template struct Y60_JSBASE_EXPORT ResultConverter<bool>;
    template struct Y60_JSBASE_EXPORT ResultConverter<unsigned short>;
    template struct Y60_JSBASE_EXPORT ResultConverter<unsigned long>;
    template struct Y60_JSBASE_EXPORT ResultConverter<int>;
    template struct Y60_JSBASE_EXPORT ResultConverter<unsigned int>;
    template struct Y60_JSBASE_EXPORT ResultConverter<long>;
    template struct Y60_JSBASE_EXPORT ResultConverter<float>;
    template struct Y60_JSBASE_EXPORT ResultConverter<double>;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector2<int> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector3<int> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector4<int> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector2<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector3<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector4<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector2<double> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector3<double> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Vector4<double> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<std::vector<asl::Vector3<float> > >;
                 
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point2<int> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point3<int> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point4<int> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point2<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point3<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point4<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point2<double> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point3<double> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Point4<double> >;
                   
    template struct Y60_JSBASE_EXPORT ResultConverter<std::vector<asl::Point3<float> > >;
    template struct Y60_JSBASE_EXPORT ResultConverter<asl::Quaternion<float> >;
    template struct Y60_JSBASE_EXPORT ResultConverter<dom::NodePtr>;
    template struct Y60_JSBASE_EXPORT ResultConverter<dom::ValuePtr>;
}                   