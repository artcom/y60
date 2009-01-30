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


#ifndef _ac_scene_EdgeList_h_
#define _ac_scene_EdgeList_h_

#include <asl/dom/Nodes.h>
#include <y60/base/DataTypes.h>
#include <deque>

//#ifdef DEBUG
#define ASSURE_POLICY AssurePolicy::Throw
#define MESH_ASSURE(x) ASSURE(x)
//#else
//#define MESH_ASSURE(x)
//#endif

namespace y60 {
/**
 * 
 * @ingroup Y60scene
 * 
 */ 
class EdgeList {
public:
    EdgeList(dom::NodePtr theHalfEdgesNode, dom::NodePtr theIndicesNode);
    EdgeList(const VectorOfSignedInt & theHalfEdges, const VectorOfUnsignedInt & theIndices);

    typedef std::deque<int> Star;

    class iterator {
    public:
        //iterator(dom::NodePtr theHalfEdgesNode, dom::NodePtr theIndicesNode, int theIndex = 0);
        iterator(const VectorOfSignedInt & theHalfEdges, const VectorOfUnsignedInt & theIndices, int theIndex = 0);
        /**
         * iterate counterclockwise around a face
         * @return iterator to HalfEdge that follows the current in counterclockwise order
         */
        void operator ++();
        /**
         * iterate clockwise around a face
         * @return iterator to HalfEdge that follows the current in clockwise order
         */
        void operator --();
        /**
         * Set the iterator to some other iterator
         */
        void operator =(const EdgeList::iterator & i) { _myIndex = *i; }
        /**
         * @return true, if this halfedge has a twin
         */
        bool has_twin() const { return _myHalfEdges[_myIndex] >= 0; }
        /**
         * @return twin of this halfedge
         */
        EdgeList::iterator twin() const;

        /**
         * @return prev of this halfedge
         */
        EdgeList::iterator prev() const;

        /**
         * @return next of this halfedge
         */
        EdgeList::iterator next() const;

        /**
         * @return next halfedge in clockwise fashion around the 
         *         incident vertex of this halfedge
         */
        EdgeList::iterator prevStar() const;

        /**
         * @return next halfedge in counterclockwise fashion around the 
         *         incident vertex of this halfedge
         */
        EdgeList::iterator nextStar() const;

        /**
         * @return index of the twin (-1 if no twin)
         */
        unsigned int twinIndex() const { return _myHalfEdges[_myIndex]; }

        /**
         * @return index of the vertex of this halfedge
         */
        unsigned int vertex() const { return _myIndices[_myIndex]; }

        /**
         * 
         * @return index of current edge
         */
        int operator *() const;

        /**
         * 
         * @return index of current edge
         */
        int & operator *();
        /**
         * @return The number of halfedges
         */
        int getListSize() const { return _myHalfEdges.size(); }
        /**
         * @return the star of the current halfedge if it is not on the
         *         border, else an empty vector
         */
        EdgeList::Star EdgeList::iterator::getInnerStar() const;

        DEFINE_EXCEPTION(Exception, asl::Exception);

    private:
        int _myIndex;
        const VectorOfSignedInt & _myHalfEdges;
        const VectorOfUnsignedInt & _myIndices;

        unsigned int _myFaceSize;
    };

    EdgeList::iterator getHalfEdge(int theIndex) const;
    Star EdgeList::getStar(int theIndex) const;
    EdgeList::iterator undefined() const;

    /**
     * @return the invalid iterator for this edgelist.
     */
    //EdgeList::iterator & invalid() const;
private:
    const VectorOfSignedInt & _myHalfEdges;
    const VectorOfUnsignedInt & _myIndices;
};

typedef asl::Ptr<EdgeList> EdgeListPtr;

inline bool operator==(const EdgeList::iterator & i1, const EdgeList::iterator & i2)
{
    return (*i1 == *i2);
}

inline bool operator!=(const EdgeList::iterator & i1, const EdgeList::iterator & i2)
{
    return (*i1 != *i2);
}



}

#endif // _ac_scene_EdgeList_h_
