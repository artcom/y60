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
#include "PropertyValue.h"

#include "JSVector.h"
#include "JSBox.h"
#include "JSBox2f.h"
#include "JSMatrix.h"
#include "JSNode.h"
#include "PropertyValue.impl"

namespace y60 {

template Y60_JSBASE_DECL int PropertyValue::get() const;
template unsigned Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Unsigned16 Y60_JSBASE_DECL PropertyValue::get() const;
template char Y60_JSBASE_DECL PropertyValue::get() const;
template unsigned char Y60_JSBASE_DECL PropertyValue::get() const;
template bool Y60_JSBASE_DECL PropertyValue::get() const;
template float Y60_JSBASE_DECL PropertyValue::get() const;
template long Y60_JSBASE_DECL PropertyValue::get() const;
template std::string Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Vector2i Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Vector2f Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Vector3i Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Vector3f Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Vector4i Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Vector4f Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Matrix4f Y60_JSBASE_DECL PropertyValue::get() const;
template std::vector<float> Y60_JSBASE_DECL PropertyValue::get() const;
template std::vector<std::string> Y60_JSBASE_DECL PropertyValue::get() const;
template std::vector<asl::Vector2f> Y60_JSBASE_DECL PropertyValue::get() const;
template std::vector<asl::Box2f> Y60_JSBASE_DECL PropertyValue::get() const;
template dom::NodePtr Y60_JSBASE_DECL PropertyValue::get() const;
//template asl::Box2i Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Box2f Y60_JSBASE_DECL PropertyValue::get() const;
//template asl::Box3i Y60_JSBASE_DECL PropertyValue::get() const;
template asl::Box3f Y60_JSBASE_DECL PropertyValue::get() const;

template bool Y60_JSBASE_DECL PropertyValue::get(int& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(unsigned& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Unsigned16& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(char & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(unsigned char & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(bool& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(float& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(long& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(std::string& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Vector2i& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Vector2f& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Vector3i& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Vector3f& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Vector4i& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Vector4f& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Matrix4f& theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(std::vector<std::string> & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(std::vector<float> & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(std::vector<asl::Vector2f> & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(dom::NodePtr& theReturnValue) const;
//template bool Y60_JSBASE_DECL PropertyValue::get(asl::Box2i & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Box2f & theReturnValue) const;
//template bool Y60_JSBASE_DECL PropertyValue::get(asl::Box3i & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(asl::Box3f & theReturnValue) const;
template bool Y60_JSBASE_DECL PropertyValue::get(std::vector<asl::Box2f> & theReturnValue) const;

template void Y60_JSBASE_DECL PropertyValue::set(const int & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const unsigned & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Unsigned16 & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const char & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const unsigned char & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const bool & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const float & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const long & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const std::string & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Vector2i & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Vector2f & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Vector3i & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Vector3f & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Vector4i & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Vector4f & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Matrix4f & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const std::vector<float> & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const std::vector<std::string> & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const std::vector<asl::Vector2f> & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const dom::NodePtr & theValue);
//template void Y60_JSBASE_DECL PropertyValue::set(const asl::Box2i & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Box2f & theValue);
//template void Y60_JSBASE_DECL PropertyValue::set(const asl::Box3i & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const asl::Box3f & theValue);
template void Y60_JSBASE_DECL PropertyValue::set(const std::vector<asl::Box2f> & theValue);

}
