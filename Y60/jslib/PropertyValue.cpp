//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "PropertyValue.h"

#include "JSVector.h"
#include "JSNode.h"
#include "PropertyValue.impl"

namespace y60 {

template int PropertyValue::get() const;
template unsigned PropertyValue::get() const;
template bool PropertyValue::get() const;
template float PropertyValue::get() const;
template std::string PropertyValue::get() const;
template asl::Vector2i PropertyValue::get() const;
template dom::NodePtr PropertyValue::get() const;

template bool PropertyValue::get(int& theReturnValue) const;
template bool PropertyValue::get(unsigned& theReturnValue) const;
template bool PropertyValue::get(bool& theReturnValue) const;
template bool PropertyValue::get(float& theReturnValue) const;
template bool PropertyValue::get(std::string& theReturnValue) const;
template bool PropertyValue::get(asl::Vector2i& theReturnValue) const;
template bool PropertyValue::get(dom::NodePtr& theReturnValue) const;

template void PropertyValue::set(const int & theValue);
template void PropertyValue::set(const unsigned & theValue);
template void PropertyValue::set(const bool & theValue);
template void PropertyValue::set(const float & theValue);
template void PropertyValue::set(const std::string & theValue);
template void PropertyValue::set(const asl::Vector2i & theValue);
template void PropertyValue::set(const dom::NodePtr & theValue);

}
