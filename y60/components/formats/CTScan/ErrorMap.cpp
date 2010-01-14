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

#include <asl/base/settings.h>
#include "ErrorMap.h"

namespace y60 {

ErrorMap::ErrorMap() : _myErrorCount(0), _myMaxError(asl::NumericTraits<float>::min())
{}

void
ErrorMap::updateError(int theEdge, float theNewError) {
    float theOldError = _myReverseLookup[theEdge];
    if (theOldError != theNewError) {
        eraseFromMap(theEdge, theOldError);
        insertIntoMap(theEdge, theNewError);
    }
    _myReverseLookup[theEdge] = theNewError;
}

void
ErrorMap::resize(int theSize) {
    _myReverseLookup.resize(theSize);
}

void
ErrorMap::clear() {
    _myReverseLookup.clear();
    _myMap.clear();
    _myErrorCount = 0;
}
int
ErrorMap::front() const {
    const std::set<int> & mySet = _myMap.begin()->second;
    int myEdge = *(mySet.begin());
    return myEdge;
}

void
ErrorMap::insert(int theIndex, float theError) {
    _myReverseLookup.at(theIndex) = theError;
    insertIntoMap(theIndex, theError);
}

void
ErrorMap::remove(int theIndex, int theCount) {
    for (int i = 0; i < theCount; ++i) {
        int myItemToDelete = theIndex+i;
        int myItemToMove = _myReverseLookup.size()-theCount+i;
        eraseFromMap(myItemToDelete, _myReverseLookup[myItemToDelete]);
        if (myItemToMove != myItemToDelete) {
            eraseFromMap(myItemToMove, _myReverseLookup[myItemToMove]);
            _myReverseLookup[myItemToDelete] = _myReverseLookup[myItemToMove];
            insertIntoMap(myItemToDelete, _myReverseLookup[myItemToDelete]);
        }
    }
    _myReverseLookup.resize(_myReverseLookup.size()-theCount);
}

unsigned
ErrorMap::setMaxError(float theMaxError) {
    _myMaxError = theMaxError;
    _myMap.clear();
    _myErrorCount = 0;
    for (int i = 0; i < _myReverseLookup.size(); ++i) {
        insertIntoMap(i, _myReverseLookup[i]);
    }
    return _myErrorCount;
}

unsigned
ErrorMap::eraseFromMap(int theIndex, float theOldError) {
    if (theOldError > _myMaxError) {
        return 0;
    }
    MapSet::iterator myItem = _myMap.find(theOldError);
    if (myItem == _myMap.end()) {
        AC_ERROR << "item should be in map but isn't: " << theIndex;
        return 0;
    } else {
        AC_TRACE << "removing " << theIndex << " from map ";
        // erase
        unsigned myReturn = myItem->second.erase(theIndex);
        if (myItem->second.empty()) {
            _myMap.erase(myItem);
        }
        _myErrorCount--;
        return myReturn;
    }
}

unsigned
ErrorMap::insertIntoMap(int theIndex, float theNewValue) {
    MAKE_SCOPE_TIMER(insertIntoMap);
    if (theNewValue > _myMaxError) {
        return 0;
    }
    AC_TRACE << "inserting " << theIndex << " into map ";
    MapSet::iterator myItem = _myMap.find(theNewValue);
    if (myItem == _myMap.end()) {
        std::set<int> mySet; mySet.insert(theIndex);
        MapSet::value_type myValue(theNewValue, mySet);
        myItem = _myMap.insert(myValue).first;
    } else {
        std::set<int> & mySet = myItem->second;
        mySet.insert(theIndex);
        //ErrorMap::value_type myValue(myErrorValue, mySet);
        //myItem = _myMap.insert(myValue);
    }
    _myErrorCount++;
    return 1;
}

}

