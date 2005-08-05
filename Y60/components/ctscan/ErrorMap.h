//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
#ifndef _ctscan_error_map_included
#define _ctscan_error_map_included

#include <asl/numeric_functions.h>
#include <asl/Assure.h>
#include <asl/Dashboard.h>
#include <set>
#include <map>
#include <vector>

namespace y60 {

class ErrorMap {
    typedef std::map<float, std::set<int> > MapSet; 
    public:
        ErrorMap();
        int front() const;
        void updateError(int theEdge, float theNewError);
        void resize(int theSize);
        void clear();
        void insert(int theIndex, float theError);
        void remove(int theIndex, int theCount);
        unsigned setMaxError(float theMaxError);

        bool empty() const {
            return _myMap.empty() || _myReverseLookup.size() <= 12;
        }
        float at(int theIndex) const {
            return _myReverseLookup[theIndex];
        }
        unsigned getErrorCount() const {
            return _myErrorCount;
        }

    private:
        MapSet _myMap;
        std::vector<float> _myReverseLookup;
        int _myErrorCount;
        float _myMaxError;
        unsigned eraseFromMap(int theIndex, float theOldError);
        unsigned insertIntoMap(int theIndex, float theNewValue);
};

} // namespace

#endif
