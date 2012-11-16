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

//own header
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

