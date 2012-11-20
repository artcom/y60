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

#ifndef _ac_y60_frustumvalue_h_
#define _ac_y60_frustumvalue_h_

#include "y60_base_settings.h"

#include <asl/dom/Value.h>
#include <asl/math/Frustum.h>

namespace dom {

/* PM: Where does this FrustumValue differ from a simple instantiation of SimpleValue<asl::Frustum> ?
   Do we really need it ?
*/

class FrustumValue : public SimpleValue<asl::Frustum> {
    public:
        typedef SimpleValue<asl::Frustum> Base;
        FrustumValue(Node * theNode) : Base(theNode) {}
        FrustumValue(const asl::Frustum  & theValue, Node * theNode) : Base(theValue, theNode) {}
        FrustumValue(const DOMString & theStringValue, Node * theNode) : Base(theStringValue, theNode) {}
        FrustumValue(const asl::ReadableBlock & theValue, Node * theNode) : Base(theValue, theNode) {}
        Y60_BASE_DECL virtual void binarize(asl::WriteableStream & theDest) const;
        Y60_BASE_DECL virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos);

    virtual ValuePtr clone(Node * theNode) const {
        this->onGetValue();
        return ValuePtr(new FrustumValue(this->getValue(), theNode));
    }
    virtual ValuePtr create(Node * theNode) const {
        return ValuePtr(new FrustumValue(theNode));
    }
    virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
        return ValuePtr(new FrustumValue(theValue, theNode));
    }
    virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
        return ValuePtr(new FrustumValue(theValue, theNode));
    }
};

template <>
struct ValueWrapper<asl::Frustum> {
    typedef FrustumValue Type;
};


} // namespace


#endif
