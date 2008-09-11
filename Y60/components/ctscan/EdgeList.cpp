#include "EdgeList.h"
#include <asl/base/Assure.h>

namespace y60 {
EdgeList::EdgeList(dom::NodePtr theHalfEdgesNode, dom::NodePtr theIndicesNode) : _myHalfEdges(theHalfEdgesNode->nodeValueRef<VectorOfSignedInt>()), _myIndices(theIndicesNode->nodeValueRef<VectorOfUnsignedInt>()) {
}

EdgeList::EdgeList(const VectorOfSignedInt & theHalfEdges, const VectorOfUnsignedInt & theIndices) : _myHalfEdges(theHalfEdges), _myIndices(theIndices) {
}

EdgeList::iterator::iterator(const VectorOfSignedInt & theHalfEdges, const VectorOfUnsignedInt & theIndices, int theIndex) :
_myHalfEdges(theHalfEdges),
_myIndices(theIndices),
_myIndex(theIndex)
{
    if (theIndex >= _myHalfEdges.size()) {
        AC_ERROR << "theIndex =" << theIndex << ", size=" << _myHalfEdges.size();
    }
    MESH_ASSURE(theIndex >= 0);
    MESH_ASSURE(theIndex < _myHalfEdges.size());
    _myFaceSize = 3;
}

void EdgeList::iterator::operator ++() {
    if (_myIndex >= 0) {
        unsigned int myFaceStart = _myIndex - _myIndex % _myFaceSize;
        unsigned int myFacePosition = _myIndex - myFaceStart;
        _myIndex = myFaceStart + ((myFacePosition + 1) % _myFaceSize);
    }
}

void EdgeList::iterator::operator --() {
    if (_myIndex >= 0) {
        unsigned int myFaceStart = _myIndex - _myIndex % _myFaceSize;
        unsigned int myFacePosition = _myIndex - myFaceStart;
        _myIndex = myFaceStart + ((myFacePosition - 1 + _myFaceSize) % _myFaceSize);
    }
}

int EdgeList::iterator::operator *() const {
    return _myIndex;
}

int & EdgeList::iterator::operator *() {
    return _myIndex;
}


EdgeList::iterator EdgeList::iterator::twin() const{
    EdgeList::iterator myReturn = *this;
    myReturn._myIndex = _myHalfEdges[_myIndex];
    return myReturn;
}

EdgeList::iterator EdgeList::iterator::prev() const {
    EdgeList::iterator myReturn = *this;
    --myReturn;
    return myReturn;
}

EdgeList::iterator EdgeList::iterator::next() const {
    EdgeList::iterator myReturn = *this;
    ++myReturn;
    return myReturn;
}

EdgeList::iterator EdgeList::iterator::nextStar() const {
    return prev().twin();
}

EdgeList::iterator EdgeList::iterator::prevStar() const {
    return twin().next();
}

EdgeList::Star EdgeList::iterator::getInnerStar() const {
    EdgeList::iterator myStart = *this;
    EdgeList::iterator i = *this;
    EdgeList::Star myResult;
    do {
        if (*i < 0) {
            return EdgeList::Star();
        }
        myResult.push_back(*i);
        i = i.nextStar();
    } while (i != myStart);
    return myResult;
}

EdgeList::iterator EdgeList::getHalfEdge(int theIndex) const {
    return EdgeList::iterator(_myHalfEdges, _myIndices, theIndex);
}

EdgeList::iterator EdgeList::undefined() const {
    return getHalfEdge(-1);
}

EdgeList::Star EdgeList::getStar(int theIndex) const {
    EdgeList::iterator myStart(_myHalfEdges, _myIndices, theIndex);
    EdgeList::Star myResult;
    EdgeList::iterator i = myStart;
    if (*i == -1) return myResult;
    bool myReverseMode = false;
    // clockwise 
    do {
        if (myResult.size() > 5000) {
            AC_ERROR << "Strange loop in star of " << *myStart;
            throw asl::Exception("Strange loop in star", PLUS_FILE_LINE);
        }
        int myValue = *i;
        if (!myReverseMode) {
            MESH_ASSURE(myValue >= 0 && myValue < myStart.getListSize());
            myResult.push_back(myValue);
            i = i.nextStar();
            MESH_ASSURE(*i < 0 || *i.prevStar() == myValue);
            if (*i == -1) {
                // Switch direction to clockwise and start again from myStart
                myReverseMode = true;
                i = myStart.prevStar();
                if (*i == -1) {
                    break;
                }
            } 
        } else {
            if (myValue == -1) {
                break;
            }
            MESH_ASSURE(myValue >= 0 && myValue < myStart.getListSize());
            myResult.push_front(myValue);
            i = i.prevStar();
            MESH_ASSURE(*i <= 0 || *i != myValue);
        }
    } while (i != myStart);
    return myResult;
}

} // namespace y60

