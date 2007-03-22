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
    float myFloat;

    thePos = theSource.readData(myFloat, thePos);
    myValue.setLeft(myFloat);
    thePos = theSource.readData(myFloat, thePos);
    myValue.setRight(myFloat);
    thePos = theSource.readData(myFloat, thePos);
    myValue.setBottom(myFloat);
    thePos = theSource.readData(myFloat, thePos);
    myValue.setTop(myFloat);
    thePos = theSource.readData(myFloat, thePos);
    myValue.setNear(myFloat);
    thePos = theSource.readData(myFloat, thePos);
    myValue.setFar(myFloat);

    asl::ProjectionType myType;
    thePos = theSource.readData(myType, thePos);
    myValue.setType(myType);

    closeWriteableValue();
    return thePos;
}

}

