#include "Mesh.h"
#include "ErrorMap.h"
#include <asl/Logger.h>
#include <list>
#include <y60/NodeNames.h>
#include <asl/Assure.h>
#include <asl/GeometryUtils.h>
#include "EdgeList.h"
#include <y60/Scene.h>

using namespace asl;
using namespace std;

namespace y60 {

Mesh::Mesh(dom::NodePtr theShapeNode) : _myShapeNode(theShapeNode), _myPositions(0), _myNormals(0), _myColors(0), _myHalfEdges(0), _myWriteLockCount(0), _myEdgeList(0), _myMaxError(0.0), _mySimplifyMode(false), _myNormalVerticesNode(0)  {
    _myElementsNode = theShapeNode->childNode(PRIMITIVE_LIST_NAME)->childNode(ELEMENTS_NODE_NAME);
    _myHalfEdgesNode = _myElementsNode->childNode(HALFEDGES_NODE_NAME);
    //_myIndexNodes.push_back(_myHalfEdgesNode);
    //for (int i = 0; i < _myElementsNode->childNodesLength(); ++i) {
    //    _myIndexNodes.push_back(_myElementsNode->childNode(i));
    //}

    _myColorIndexNode = _myElementsNode->childNodeByAttribute(VERTEX_INDICES_NAME, VERTEX_DATA_ROLE_ATTRIB, COLOR_ROLE);
    _myNormalIndexNode = _myElementsNode->childNodeByAttribute(VERTEX_INDICES_NAME, VERTEX_DATA_ROLE_ATTRIB, NORMAL_ROLE);
    _myPositionIndexNode = _myElementsNode->childNodeByAttribute(VERTEX_INDICES_NAME, VERTEX_DATA_ROLE_ATTRIB, POSITION_ROLE);
    _myVertexDataNode = theShapeNode->childNode(VERTEX_DATA_NAME);

    string myPositionsName = _myPositionIndexNode->getAttributeValue<string>(VERTEX_DATA_ATTRIB);
    _myPositionIndexNode = _myPositionIndexNode->childNode(0);
    if (_myColorIndexNode) {
        _myColorIndexNode = _myColorIndexNode->childNode(0);
    }
    _myHalfEdgesNode = _myHalfEdgesNode->childNode(0);
    _myPositionVerticesNode = _myVertexDataNode->childNodeByAttribute(SOM_VECTOR_VECTOR3F_NAME, NAME_ATTRIB, myPositionsName)->childNode(0);
    if (_myNormalIndexNode) {
        string myNormalsName = _myNormalIndexNode->getAttributeValue<string>(VERTEX_DATA_ATTRIB);
        _myNormalIndexNode = _myNormalIndexNode->childNode(0);
        _myNormalVerticesNode = _myVertexDataNode->childNodeByAttribute(SOM_VECTOR_VECTOR3F_NAME, NAME_ATTRIB, myNormalsName)->childNode(0);
    }

}

unsigned 
Mesh::colorize(unsigned int theStartIndex, unsigned theColorIndex) {
    lockWrite();
    unsigned int myCount = 0;
    if (_myColors) {
        // recursive depth painter
        // XXX Works only for single primitives
        EdgeList::iterator iter = _myEdgeList->getHalfEdge(theStartIndex);
        if (theStartIndex < 0 || theStartIndex > _myPositions->size()) {
            AC_ERROR << "Illegal Index " << theStartIndex << ". The Shape has only " << _myPositionIndexNode->nodeValueRef<VectorOfUnsignedInt>().size() << " indices.";
            return 0;
        } 
        list<unsigned int> myToDoList;
        myToDoList.push_back(theStartIndex);

        // color this face
        while(myToDoList.begin() != myToDoList.end()) {
            *iter = myToDoList.front();
            myToDoList.pop_front();
            if ((*_myColors)[*iter] == theColorIndex) continue;
            for (unsigned int i = 0; i < 3; ++i) {
                (*_myColors)[*iter] = theColorIndex;
                if (iter.twinIndex() != -1 && (*_myColors)[iter.twinIndex()] != theColorIndex) {
                    myToDoList.push_back(iter.twinIndex());
                }
                ++iter;
                ++myCount;
            }
        }
    }
    unlockWrite();
    return myCount;
}

int 
Mesh::deleteFace(unsigned theIndex) {
    MAKE_SCOPE_TIMER(deleteFace);
    int myReverseIndex = _myPositions->size()-_myFaceSize;
    int myIndex = (theIndex / _myFaceSize) * _myFaceSize;
    int myDelta = 0;
    if (myIndex < myReverseIndex) {
        for (int i = 0; i < _myFaceSize; ++i) {
            (*_myPositions)[myIndex+i] = (*_myPositions)[myReverseIndex+i];
            if (_myColors) {
                (*_myColors)[myIndex+i] = (*_myColors)[myReverseIndex+i];
            }
            if (_myNormals) {
                (*_myNormals)[myIndex+i] = (*_myNormals)[myReverseIndex+i];
            }
            if (_myHalfEdges) {
                int myHalfEdgeValue = (*_myHalfEdges)[myReverseIndex+i];
                (*_myHalfEdges)[myIndex+i] = myHalfEdgeValue;
                if (myHalfEdgeValue >= 0) {
                    (*_myHalfEdges)[myHalfEdgeValue] = myIndex+i;
                }
            }
        }
        //AC_INFO << "Overwriting " << myIndex << " with " << myReverseIndex;
        myDelta = myReverseIndex - myIndex;
    } 
    unsigned myNewSize = myReverseIndex;
    if (_myNormals) {
        _myNormals->resize(myNewSize);
    }
    _myPositions->resize(myNewSize);
    if (_myHalfEdges) {
        _myHalfEdges->resize(myNewSize);
    }
    if (_myColors) {
        _myColors->resize(myNewSize);
    }
    _myErrorMap.remove(myIndex, _myFaceSize);
    return myDelta;
}

unsigned
Mesh::colorizeError(float theMaximumError, unsigned theColor) {
    lockWrite();
    unsigned myCount = 0;
    for (unsigned i = 0; i < _myPositions->size(); ++i) {
        if (_myErrorMap.at(i) < theMaximumError) {
            (*_myColors)[i] = theColor;
            ++myCount;
        } 
    }
    unlockWrite();
    return myCount;
}

float
Mesh::calcEdgeError(unsigned theIndex) {
    MAKE_SCOPE_TIMER(calcEdgeError);
    if ((*_myHalfEdges)[theIndex] >= 0) {
        vector<Vector3f> & myVertices = *_myPositionVertices;
        vector<unsigned> & myPositions = *_myPositions;
        EdgeList::iterator myEdge = _myEdgeList->getHalfEdge(theIndex);
        EdgeList::iterator myTwin = _myEdgeList->getHalfEdge(myEdge.twinIndex());
        const Vector3f myEdgeV[] = {myVertices[myPositions[*myEdge]], myVertices[myPositions[*myEdge.next()]], myVertices[myPositions[*myEdge.next().next()]]};
        const Vector3f myTwinV[] = {myVertices[myPositions[*myTwin]], myVertices[myPositions[*myTwin.next()]], myVertices[myPositions[*myTwin.next().next()]]};

        //float myFaceArea = computeFaceArea(myEdgeV[0], myEdgeV[1], myEdgeV[2]);
        //float myTwinArea = computeFaceArea(myTwinV[0], myTwinV[1], myTwinV[2]);
        Vector3f myNormal = generateFaceNormal(myEdgeV[0], myEdgeV[1], myEdgeV[2]);
        Vector3f myTwinNormal = generateFaceNormal(myTwinV[0], myTwinV[1], myTwinV[2]);
        //float myCreaseAngle = dot(myNormal, myTwinNormal);
        float myEdgeLength = length(myEdgeV[1] - myEdgeV[0]);

        // walk around star of myEdge
        float myAngleMax = 0;
        vector<int> myStar = myEdge.getInnerStar();
        // Don't remove outer vertices!
        if (myStar.empty()) {
            return NumericTraits<float>::max();
        }
        for (vector<int>::const_iterator i = myStar.begin(); i != myStar.end(); ++i) {
            MESH_ASSURE(*i >= 0);
            EdgeList::iterator myStarEdge = _myEdgeList->getHalfEdge(*i);
            const Vector3f myStarFace[] = {myVertices[myPositions[*myStarEdge]], myVertices[myPositions[*myStarEdge.next()]], myVertices[myPositions[*myStarEdge.next().next()]]};
            Vector3f myStarNormal = generateFaceNormal(myStarFace[0], myStarFace[1], myStarFace[2]); 
            myAngleMax = max(myAngleMax, min((1.0f-dot(myStarNormal, myTwinNormal))/2.0f, 
                                             (1.0f-dot(myStarNormal, myNormal))/2.0f));
        }
        //float myReturn = (1.0f - myCreaseAngle) * (myFaceArea + myTwinArea) + (myAngleSum * (myEdgeLength + 1));
        float myReturn = (myAngleMax * myEdgeLength);
        //AC_INFO << "F-A: " << myCreaseAngle << ", Size: " << (myFaceArea + myTwinArea) << ", E-A: " << myAngleSum  
        //    << ", E-L: " << myEdgeLength << " -> [" << theIndex << "] = " << myReturn;
        return myReturn;
    } else {
        // Large Value
        return NumericTraits<float>::max();
    }
}

void
Mesh::computeError() {
    MAKE_SCOPE_TIMER(computeError);
    lockWrite();
    _myErrorMap.resize(_myPositions->size());

    for (int i = 0; i < _myPositions->size(); ++i) {
        _myErrorMap.insert(i,calcEdgeError(i));
        int myPositionsSize = _myPositions->size();
        if (i % (myPositionsSize / 100 + 1) == 0) {
            notifyProgress(i / double(_myPositions->size()), "computing costs");
        }
    }
    unlockWrite();
}

bool
Mesh::check() const {
    MAKE_SCOPE_TIMER(Mesh_check);
    bool mySuccess = true;
    const VectorOfUnsignedInt & myPositions = _myPositions ? *_myPositions : _myPositionIndexNode->nodeValueRef<VectorOfUnsignedInt>();
    const VectorOfUnsignedInt * myNormals = _myNormals ? _myNormals : &_myNormalIndexNode->nodeValueRef<VectorOfUnsignedInt>();
    const VectorOfUnsignedInt * myColors = _myColors ? _myColors : &_myColorIndexNode->nodeValueRef<VectorOfUnsignedInt>();
    const VectorOfSignedInt & myHalfEdges = _myHalfEdges ? *_myHalfEdges : _myHalfEdgesNode->nodeValueRef<VectorOfSignedInt>();
    if (myPositions.size() != myHalfEdges.size()) {
        AC_WARNING << "Indices node sizes inconsistent! Positions: " << myPositions.size() << ", HalfEdges: " << myHalfEdges.size();
        mySuccess = false;
    }
    if (myNormals && myNormals->size() != myPositions.size()) {
        AC_WARNING << "Indices node sizes inconsistent! Positions: " << myPositions.size() << ", Normals: " << myNormals->size();
        mySuccess = false;
    }
    if (myColors && myColors->size() != myPositions.size()) {
        AC_WARNING << "Indices node sizes inconsistent! Colors: " << myColors->size() << ", Positions: " << myPositions.size();
        mySuccess = false;
    }
    // Check halfedges connectivity
    for (int i = 0; i < myHalfEdges.size(); ++i) {
        int myValue = myHalfEdges.at(i);
        if (myValue >= 0) {
            if (myValue >= myHalfEdges.size()) {
                AC_ERROR << "HalfEdge at " << i << " does point to " << myValue << " which is out of bounds (" << myHalfEdges.size() << ")!";
            } else if (myHalfEdges.at(myValue) != i) {
                mySuccess = false;
                AC_WARNING << "HalfEdge at " << i << " does point to " << myHalfEdges.at(i) << " but there is reverse connectivity to " << myHalfEdges.at(myHalfEdges.at(i));
            }
        }
    }

    // Check position and normals are equal
    if (myNormals) {
        for (int i = 0; i < myPositions.size(); ++i) {
            if (myPositions[i] != (*myNormals)[i]) {
                AC_WARNING << "Position at " << i << " is different to its normal position";
                mySuccess = false;
            }
            // XXX TODO: Check if myPositions[i] is inside vertices.size()
        }
    }
    AC_INFO << "# of faces: " << myPositions.size() / _myFaceSize; 
    return mySuccess;
}

void
Mesh::lockWrite() {
    if (_myWriteLockCount++ == 0) {
        _myPositions = _myPositionIndexNode->dom::Node::nodeValuePtrOpen<vector<unsigned int> >();
        if (_myNormalIndexNode) {
            _myNormals = _myNormalIndexNode->dom::Node::nodeValuePtrOpen<vector<unsigned int> >();
        } else {
            _myNormals = 0;
        }
        if (_myColorIndexNode) {
            _myColors = _myColorIndexNode->dom::Node::nodeValuePtrOpen<vector<unsigned int> >();
        } else {
            _myColors = 0;
        }
        _myHalfEdges = _myHalfEdgesNode->dom::Node::nodeValuePtrOpen<vector<signed int> >();
        _myPositionVertices = _myPositionVerticesNode->dom::Node::nodeValuePtrOpen<vector<Vector3f> >();
        _myNormalVertices = _myNormalVerticesNode->dom::Node::nodeValuePtrOpen<vector<Vector3f> >();        
        _myEdgeList = EdgeListPtr(new EdgeList(*_myHalfEdges, *_myPositions));
    }
}

void
Mesh::unlockWrite(bool theForceFlag /*= false*/) {
    if (_myWriteLockCount == 1 || theForceFlag) {
        _myPositionIndexNode->dom::Node::nodeValuePtrClose<vector<unsigned int> >();
        _myPositions = 0;
        if (_myNormalIndexNode) {
            _myNormalIndexNode->dom::Node::nodeValuePtrClose<vector<unsigned int> >();
            _myNormals = 0;
        }
        if (_myColorIndexNode) {
            _myColorIndexNode->dom::Node::nodeValuePtrClose<vector<unsigned int> >();
            _myColors = 0;
        }
        _myHalfEdgesNode->dom::Node::nodeValuePtrClose<vector<signed int> >();
        _myHalfEdges = 0;
        _myPositionVerticesNode->dom::Node::nodeValuePtrClose<vector<Vector3f> >();
        _myPositionVertices = 0;
        _myNormalVerticesNode->dom::Node::nodeValuePtrClose<vector<Vector3f> >();
        _myNormalVertices = 0;
        _myEdgeList = EdgeListPtr(0);
        _myWriteLockCount = 0;
    }
}

void
Mesh::findHalfEdges(int thePosition, unsigned theDistance, set<int> & theDirtySet) {
    MAKE_SCOPE_TIMER(findHalfEdges);
    // XXX This algo kinda sucks and does a lot of redundant work!!!
    typedef map<int, unsigned> WorkList;
    WorkList myWorkList;
    myWorkList.insert(WorkList::value_type(thePosition, theDistance));
    while (myWorkList.begin() != myWorkList.end()) {
        // get an edge from the worklist
        WorkList::value_type myValue = *myWorkList.begin();
        //AC_INFO << "Working on: " << myValue.first;
        myWorkList.erase(myWorkList.begin());
        const unsigned & myDistance = myValue.second;
        theDirtySet.insert(myValue.first);
        if (myDistance <= 0) {
            continue;
        } else {
            const EdgeList::Star myStar = _myEdgeList->getStar(myValue.first);
            for (EdgeList::Star::const_iterator i = myStar.begin(); i != myStar.end(); ++i) {
                int myIndex = *i;
                MESH_ASSURE(myIndex >= 0);
                MESH_ASSURE(myIndex < _myHalfEdges->size());
                int myNewValue = (*_myHalfEdges)[myIndex];
                if (myNewValue >= 0) {
                    WorkList::const_iterator myExistingValue = myWorkList.find(myNewValue);
                    if (myExistingValue == myWorkList.end() || myExistingValue->second < myDistance-1) {
                        myWorkList.insert(WorkList::value_type(myNewValue, myDistance-1));
                        //AC_INFO << "Inserting: (" << *i << "|" << (*_myHalfEdges)[*i] << " -> " << myDistance-1 << ")";
                    } else {
                        //AC_INFO << "Skipping: (" << *i << "|" << (*_myHalfEdges)[*i] << " -> " << myDistance-1 << ")";
                    }
                }
            }
        }
    }
    return;
}

bool
Mesh::edgeCollapse(unsigned thePosition) {
    MAKE_SCOPE_TIMER(edgeCollapse);
    lockWrite();
    int myPos1 = thePosition;
    int myPos2 = (*_myHalfEdges)[thePosition];
    AC_TRACE << "Collapsing " << myPos1 << " to " << myPos2;
    if (myPos2 == -1) {
        AC_INFO << "Error trying to collapse outer edge #" << thePosition;
        unlockWrite();
        return false;
    }
    if ((myPos1 / 3) == (myPos2 / 3)) {
        AC_INFO << "Error trying to collapse simplicit object #" << thePosition;
        unlockWrite();
        return false;
    }
    /* 
    float myNewError = calcEdgeError(thePosition);
    float myOldError = _myEdgeError[thePosition];
    if (!almostEqual(myOldError, myNewError)) {
        AC_ERROR << "error not up to date @ " << thePosition;
        AC_ERROR << "     old error " << myOldError;
        AC_ERROR << "     new error " << myNewError;
        unlockWrite();
        return false;
    }
    */
    // this is an EdgeList transaction
    EdgeList::iterator myEdge = _myEdgeList->getHalfEdge(thePosition);
    EdgeList::iterator myTwin = myEdge.twin();
    MESH_ASSURE(myTwin.twinIndex() == *myEdge);
    MESH_ASSURE(myTwin.next().twinIndex() >= 0 || myTwin.prev().twinIndex() >=0);
    MESH_ASSURE(myEdge.next().twinIndex() >= 0 || myEdge.prev().twinIndex() >=0);
    int mySafeIndex = myTwin.prev().twinIndex();
    if (mySafeIndex < 0) {
        mySafeIndex = myEdge.prev().twinIndex();
    }
    MESH_ASSURE(mySafeIndex >= 0);
    // walk around star of myEdge and set the indicent vertex to *myTwin
    EdgeList::Star myIncidentEdges = _myEdgeList->getStar(thePosition);
    int myOldPosition = (*_myPositions)[thePosition];
    for (EdgeList::Star::const_iterator i = myIncidentEdges.begin(); i != myIncidentEdges.end(); ++i) {
        int myCurrentStar = *i;
        MESH_ASSURE((*_myPositions)[myCurrentStar] == myOldPosition);
        (*_myPositions)[myCurrentStar] = (*_myPositions)[myPos2];
        if (_myNormals) {
            (*_myNormals)[myCurrentStar] = (*_myNormals)[myPos2];
        }
        if (_myColors) {
            (*_myColors)[myCurrentStar] = (*_myColors)[myPos2];
        }
    }
    // XXX Possible Test: myOldPosition should now no longer exist in the positions list
    // connect myTwin.next().twin() with myTwin.prev().twin()
    int next = myTwin.next().twinIndex();
    int prev = myTwin.prev().twinIndex();
    (*_myHalfEdges)[next] = prev;
    (*_myHalfEdges)[prev] = next;
    //AC_TRACE << "Connecting " << prev << " to " << next;
    // connect myEdge.next().twin() with myEdge.prev().twin()
    next = myEdge.next().twinIndex();
    prev = myEdge.prev().twinIndex();
    (*_myHalfEdges)[next] = prev;
    (*_myHalfEdges)[prev] = next;
    //AC_TRACE << "Connecting " << prev << " to " << next;
    int myTwinIndex = *myTwin;
    // completely remove myEdge, myEdge.next(), myEdge.prev(), 
    // myTwin, myTwin.next(), myTwin.prev() from all indices
    //set all the twins of all edges of twin to -1
    EdgeList::iterator iter = myTwin;
    do {
        (*_myHalfEdges)[*iter] = -1;
        ++iter;
    } while (myTwin != iter);

    AC_TRACE << "Deleting face: " << *myEdge << " and " << myTwinIndex << ", Safe: " << mySafeIndex << ", size: " << _myHalfEdges->size();
    int myDelta = deleteFace(*myEdge);
    if (mySafeIndex >= _myHalfEdges->size()) {
        //AC_TRACE << "Moving mySafeIndex " << myTwinIndex << " -> " << mySafeIndex - myDelta;
        mySafeIndex -= myDelta;
    }
    if (myTwinIndex >= _myHalfEdges->size()) {
        //AC_TRACE << "Moving TwinIndex " << myTwinIndex << " -> " << myTwinIndex - myDelta;
        myTwinIndex -= myDelta;
    }
    AC_TRACE << "Deleting face: " << myTwinIndex << ", Safe: " << mySafeIndex << ", size: " << _myHalfEdges->size();
    myDelta = deleteFace(myTwinIndex);
    if (mySafeIndex >= _myHalfEdges->size()) {
        //AC_TRACE << "Moving mySafeIndex " << myTwinIndex << " -> " << mySafeIndex - myDelta;
        mySafeIndex -= myDelta;
    }
    //if (!check()) {
    //    throw InconsistencyException("Check failed after EdgeCollapse", PLUS_FILE_LINE);
    // }
    //AC_TRACE << "Finding HalfEdges for " << mySafeIndex;
    set<int> myDirtySet;
    findHalfEdges(mySafeIndex, 2, myDirtySet);
    AC_TRACE << "recalculating " << myDirtySet.size() << " edge errors";

    // End of EdgeList transaction

    // remove the vertex pointed by myEdge
    // XXX We do this in post-processing

    // recalculate error for the stars of all halfedges we did touch

    for (set<int>::const_iterator i = myDirtySet.begin(); i != myDirtySet.end(); ++i) {
        _myErrorMap.updateError(*i, calcEdgeError(*i));
        AC_INFO << "Recomputed: " << *i << " to " << _myErrorMap.at(*i);
        //(*_myColors)[*i] = 2;
    }

    unlockWrite();
    return true;
}

void
Mesh::setColor(unsigned theColor) {
    lockWrite();
    for (unsigned i = 0; i < _myColors->size(); ++i) {
        (*_myColors)[i] = theColor;
    }
    unlockWrite();
}

unsigned
Mesh::collapseByError(float theMaxError) {
    lockWrite();
    // check();
    //bool myFirst = true;
    unsigned myNumModified = 0;
    getDashboard().reset();
    _mySimplifyMode = true;
    try {
        computeError();
        notifyProgress(0.0, "");
        unsigned myInitialErrors = _myErrorMap.setMaxError(theMaxError);
        AC_INFO << "Going to delete about " << myInitialErrors << " edges";

        while (!_myErrorMap.empty() && _mySimplifyMode) {
            AC_INFO << "getting front";
            int myEdge = _myErrorMap.front();
            AC_INFO << "Collapsing edge: " << myEdge;
            AC_INFO << "         error = " << _myErrorMap.at(myEdge); 
            AC_INFO << "         count = " << _myErrorMap.getErrorCount();
            if (edgeCollapse(myEdge)) {
                ++myNumModified;
            } else {
                // Something went wrong with this one. So lets for now remove it from our map
                _myErrorMap.updateError(myEdge, NumericTraits<float>::max());
                AC_INFO << "aborted edge " << myEdge;
            }
            if ((_myErrorMap.getErrorCount() % (myInitialErrors / 1000 + 1)) == 0) {
                double myProgress = min(1.0, 1.0 - (double(_myErrorMap.getErrorCount()) / double(myInitialErrors)));
                notifyProgress(myProgress, "collapsing");
                AC_TRACE << "Progress: " << myProgress * 100;
            }
        }
    } catch (InconsistencyException e) {
        AC_ERROR << "Mesh went Inconsistent. Check: " << check();        
    //} catch (...) {
    //    AC_ERROR << "Some Exception occured during simplification. Check: " << check();        
    }
    _myErrorMap.clear();
    unlockWrite(true);
    getDashboard().print(std::cerr);
    getDashboard().cycle();
    AC_INFO << "done simplification";
    return myNumModified;
}

unsigned
Mesh::deleteIndicesByColor(unsigned int theColor) {
    lockWrite();
    //MESH_ASSURE((_myColors->size() == _myPositions->size()) && (_myPositions->size() == _myNormals->size()) && (_myNormals->size() == _myHalfEdges->size()));
    //AC_TRACE << "Colors: " << _myColors->size() << ", Positions: " << _myPositions->size() << ", Normals: " << _myNormals->size() << ", HalfEdges: " << _myHalfEdges->size();

    int myReverseIndex = _myColors->size()-_myFaceSize;
    int myIndex = 0;
    MESH_ASSURE((_myColors->size() % _myFaceSize) == 0);
    while (myIndex <= myReverseIndex) {
        if ((*_myColors)[myIndex] == theColor) {
            // move something here from the myReverseIndex
            while ((*_myColors)[myReverseIndex] == theColor && myReverseIndex > myIndex) {
                myReverseIndex -= _myFaceSize;
            }
            if (myReverseIndex <= myIndex) {
                // ready. New size is myIndex
                break;
            } else {
                for (int i = 0; i < _myFaceSize; ++i) {
                    (*_myColors)[myIndex+i] = (*_myColors)[myReverseIndex+i];
                    int myHalfEdgeValue = (*_myHalfEdges)[myReverseIndex+i];
                    (*_myHalfEdges)[myIndex+i] = myHalfEdgeValue;
                    if (myHalfEdgeValue >= 0) {
                        (*_myHalfEdges)[myHalfEdgeValue] = myIndex+i;
                    }
                    if (_myNormals) {
                        (*_myNormals)[myIndex+i] = (*_myNormals)[myReverseIndex+i];
                    }
                    (*_myPositions)[myIndex+i] = (*_myPositions)[myReverseIndex+i];
                }
                myReverseIndex -= _myFaceSize;
            }
        }
        myIndex += _myFaceSize;
    }
    unsigned myOldSize = _myPositions->size();
    if (_myNormals) {
        _myNormals->resize(myIndex);
    }
    _myPositions->resize(myIndex);
    _myHalfEdges->resize(myIndex);
    if (_myColors) {
        _myColors->resize(myIndex);
    }
    //MESH_ASSURE((_myColors->size() == _myPositions->size()) && (_myPositions->size() == _myNormals->size()) && (_myNormals->size() == _myHalfEdges->size()) && (_myHalfEdges->size() == myIndex));
    unlockWrite();
    return myOldSize - myIndex;
}

unsigned
Mesh::deleteDanglingVertices() {
    lockWrite();
    MESH_ASSURE(_myPositionVertices->size() == _myNormalVertices->size());

    vector<vector<int> > myVertexHelper;
    myVertexHelper.resize(_myPositionVertices->size());
    for (unsigned i = 0; i < _myPositions->size(); ++i) {
        myVertexHelper[(*_myPositions)[i]].push_back(i);
    }
    int myVertexHelperEnd = myVertexHelper.size()-1;
    int myIndex = 0;
    while (myIndex <= myVertexHelperEnd) {
        if (myVertexHelper[myIndex].empty()) {
            // move from back to here
            while (myVertexHelper[myVertexHelperEnd].empty() && myVertexHelperEnd > myIndex) {
                --myVertexHelperEnd;
            }
            if (myVertexHelperEnd > myIndex) {
                // copy
                const vector<int> & myList = myVertexHelper[myVertexHelperEnd];
                (*_myPositionVertices)[myIndex] = (*_myPositionVertices)[myVertexHelperEnd];
                (*_myNormalVertices)[myIndex] = (*_myNormalVertices)[myVertexHelperEnd];
                // adjust links
                for (int i = 0; i < myList.size(); ++i) {
                    (*_myPositions)[myList[i]] = myIndex;
                    (*_myNormals)[myList[i]] = myIndex;
                }
                --myVertexHelperEnd;
            } else {
                break;
            }
        }
        ++myIndex;
    }
    int myOldSize = _myPositionVertices->size();
    myVertexHelper.clear();
    _myPositionVertices->resize(myIndex);
    _myNormalVertices->resize(myIndex);
    unlockWrite();
    return myOldSize - myIndex;
}

bool Mesh::test(dom::NodePtr theShapeNode) {
    return true;
}

unsigned
Mesh::colorSweptSphere(const asl::Sphere<float> & theSphere, 
        const asl::Vector3f & theMotion, 
        const dom::NodePtr & theTransformationRoot,
        unsigned int theColor) 
{
    CollisionInfoVector myCollisions;
    Scene::collideWithBodies(theTransformationRoot, theSphere, theMotion, myCollisions);
    AC_DEBUG << "we collided with " << myCollisions.size() << " bodys";
    for (int i = 0; i < myCollisions.size(); ++i) {
        CollisionInfo & myCollision = myCollisions[i];
        ShapePtr & myShape = myCollisions[i]._myShape;
        // XXX Works only for single primitives
        dom::NodePtr myElements = myShape->getNode().childNode(PRIMITIVE_LIST_NAME)->childNode(ELEMENTS_NODE_NAME);
        dom::NodePtr myColorNode = myElements->childNodeByAttribute(VERTEX_INDICES_NAME, VERTEX_DATA_ROLE_ATTRIB, COLOR_ROLE)->childNode(0);
        dom::Node::WritableValue<VectorOfUnsignedInt> myColorIndexLock(myColorNode);
        VectorOfUnsignedInt & myColorIndex = myColorIndexLock.get();

        for (int j = 0; j < myCollisions[i]._myPrimitiveSphereContacts->size(); ++j) {
            Primitive::SphereContacts & myContacts = myCollisions[i]._myPrimitiveSphereContacts->at(j);
            Primitive::Element myElement = myContacts._myElement;
            for (unsigned int k = myElement._myStartVertex; 
                    k < myElement._myStartVertex+myElement._myVertexCount; ++k) 
            {
                myColorIndex.at(myElement._myPrimitive->getDomIndex()+k) = theColor; 
            }
        }

    }
    return myCollisions.size();
}


void 
Mesh::notifyProgress(double theProgress, const std::string & theMessage) {
    //cerr << "progress = " << theProgress << endl;
    _myProgressSignal.emit(theProgress, Glib::ustring(theMessage));
}


}
