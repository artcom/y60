//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSWrapper.h"
#include "JSNode.h"
#include "JSEvent.h"
#include "JSVector.h"
#include "JSQuaternion.h"
#include "JSBox.h"
#include "JSBox2f.h"
#include "JSSphere.h"
#include "JSMatrix.h"
#include "ArgumentHolder.impl"

#include <string>
#include <vector>

namespace jslib {
    template struct ArgumentHolder<bool>;
    template struct ArgumentHolder<short>;
    template struct ArgumentHolder<unsigned short>;
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
    template struct ArgumentHolder<asl::Time>;
    
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
    template struct ResultConverter<dom::NodePtr>;
    template struct ResultConverter<dom::ValuePtr>;
}
