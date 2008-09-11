//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

