//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_frustumvalue_h_
#define _ac_y60_frustumvalue_h_

#include <dom/Value.h>
#include <asl/Frustum.h>

namespace dom {

class FrustumValue : public SimpleValue<asl::Frustum> {
    public:
        typedef SimpleValue<asl::Frustum> Base;
        FrustumValue(Node * theNode) : Base(theNode) {}
        FrustumValue(const asl::Frustum  & theValue, Node * theNode) : Base(theValue, theNode) {}
        FrustumValue(const DOMString & theStringValue, Node * theNode) : Base(theStringValue, theNode) {}
        FrustumValue(const asl::ReadableBlock & theValue, Node * theNode) : Base(theValue, theNode) {}
        virtual void binarize(asl::WriteableStream & theDest) const;
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos);

    virtual ValuePtr clone(Node * theNode) const {
        updateValueFromString();
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
