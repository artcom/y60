//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FrustumValue.h"

namespace dom {
void 
FrustumValue::binarize(asl::WriteableStream & theDest) const {
    const asl::Frustum & myValue = getValue();

    theDest.appendData(myValue.getLeft());
    theDest.appendData(myValue.getRight());
    theDest.appendData(myValue.getBottom());
    theDest.appendData(myValue.getTop());
    theDest.appendData(myValue.getNear());
    theDest.appendData(myValue.getFar());

    theDest.appendData(myValue.getType());
}

asl::AC_SIZE_TYPE 
FrustumValue::debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
    asl::Frustum & myValue = openWriteableValue();
    float myLeft, myRight, myBottom, myTop, myNear, myFar;

    thePos = theSource.readData(myLeft, thePos);
    thePos = theSource.readData(myRight, thePos);
    thePos = theSource.readData(myBottom, thePos);
    thePos = theSource.readData(myTop, thePos);
    thePos = theSource.readData(myNear, thePos);
    thePos = theSource.readData(myFar, thePos);
    myValue.set(myLeft, myRight, myBottom, myTop, myNear, myFar);

    asl::ProjectionType myType;
    thePos = theSource.readData(myType, thePos);
    myValue.setType(myType);

    closeWriteableValue();
    return thePos;
}

}

