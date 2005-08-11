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


#ifndef _ac_scene_EdgeList_h_
#define _ac_scene_EdgeList_h_

#include <dom/Nodes.h>
#include <y60/DataTypes.h>
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
