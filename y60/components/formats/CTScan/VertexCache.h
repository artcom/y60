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

#ifndef _ac_ctscan_VertexCache_h_
#define _ac_ctscan_VertexCache_h_

#include <vector>
#include <asl/base/Ptr.h>

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
            _myISize = iSize + 2; // make them 2 bigger so we can get -1 and len
            _myJSize = jSize + 2; // for CloseAtClippingBoxFlag
            _myVector.resize(_myISize * _myJSize, theValue);
        }

        void reset(const ItemT & theValue = ItemT()) {
            _myVector.clear();
            _myVector.resize(_myISize * _myJSize, theValue);
        }

        ItemT & get(int theI, int theJ) {
            return _myVector[(theJ+1) * _myISize + (theI+1)];
        }


        const ItemT & get(int theI, int theJ) const {
            return _myVector[(theJ+1) * _myISize + (theI+1)];
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
