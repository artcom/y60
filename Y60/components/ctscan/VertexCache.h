//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_ctscan_VertexCache_h_
#define _ac_ctscan_VertexCache_h_

#include <vector>
#include <asl/Ptr.h>

namespace y60 {
/**
 * Cache for Vertices on Marching-Cubes-Edges for use in MarchingCubes.h
 * @ingroup Y60componentsctscan
 */ 

struct IJItem {
    IJItem() : i(-1), j(-1) {}
    IJItem(int theI, int theJ) : i(theI), j(theJ) {}
    int i;
    int j;
};

template <class ItemT>
class VertexCache {
    public:
        VertexCache(int iSize, int jSize, const ItemT & theValue = ItemT()) {
            _myISize = iSize;
            _myJSize = jSize;
            _myVector.resize(iSize * jSize, theValue);
        }

        void reset(const ItemT & theValue = ItemT()) {
            _myVector.clear();
            _myVector.resize(_myISize * _myJSize, theValue);
        }

        ItemT & get(int theI, int theJ) {
            return _myVector[theJ * _myISize + theI];
        }


        const ItemT & get(int theI, int theJ) const {
            return _myVector[theJ * _myISize + theI];
        }

    private:
        std::vector<ItemT> _myVector;
        int _myISize;
        int _myJSize;
};

typedef VertexCache<IJItem> IJCache;
typedef VertexCache<int> KCache;

typedef asl::Ptr<IJCache> IJCachePtr;
typedef asl::Ptr<KCache> KCachePtr;

}

#endif // _ac_ctscan_VertexCache_h_
