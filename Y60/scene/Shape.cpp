//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
#include "Shape.h"

namespace y60 {
    bool Shape::collide(const asl::Sphere<float> & theSphere,
        const asl::Vector3f & theMotion,
        const asl::Matrix4f & theTransformation,
        Primitive::SphereContactsList & theSphereContacts)
    {
        MAKE_SCOPE_TIMER(Shape_collide);
        bool myResult = false;
        for (int i = 0; i < _myPrimitives.size();++i) {
            if (_myPrimitives[i]->collide(theSphere, theMotion, theTransformation, theSphereContacts)) {
                myResult = true;
            }
        }
        return myResult;
    }

    bool Shape::collide(const asl::Sphere<float> & theSphere,
        const asl::Vector3f & theMotion,
        const asl::Matrix4f & theTransformation,
        const asl::SweptSphereContact<float> & theCurrentContact,
        Primitive::SphereContacts & theNewContactInfo)
    {
        MAKE_SCOPE_TIMER(Shape_collide_first);
        const asl::SweptSphereContact<float> * myCurrentContact = &theCurrentContact;
        bool myResult = false;
        for (int i = 0; i < _myPrimitives.size();++i) {
            if (_myPrimitives[i]->collide(theSphere, theMotion, theTransformation, *myCurrentContact, theNewContactInfo)) {
                myCurrentContact = &(theNewContactInfo._myMinContact);
                myResult = true;
            }
        }
        return myResult;
    }
}

