#include "HalfEdgeTable.h"
#include "face_data.h"

#include <asl/Exception.h>

HalfEdgeTable::HalfEdgeTable() {
    cubeCases.resize(256);
    for (int i = 0; i < 256; ++i) {
        std::vector<HalfEdgeNeighbor> & myLine = cubeCases[i].neighbors;
        for (int j = 0; j < 12; ++j) {
            int curEdge = ourFaceData[i][j];
            int nextEdge = ourFaceData[i][ (j/3)*3 + ((j+1)%3) ]; 
            if (curEdge == -1) {
                break;
            }
            HalfEdgeNeighbor myNeighbor;
            myNeighbor.type = classifyEdge(curEdge, nextEdge );
            if (myNeighbor.type == INTERNAL) {
                myNeighbor.internal_index = calcInnerEdge(ourFaceData[i], curEdge, nextEdge);
            }
            myLine.push_back(myNeighbor);
        }
    }
}

int
HalfEdgeTable::calcInnerEdge(const int * theFace, int theCurEdge, int theNextEdge) {
    for (int myTriangle = 0; myTriangle < 12; myTriangle +=3) {
        if (theFace[myTriangle] == -1) {
            throw asl::Exception("matching halfedge not found", PLUS_FILE_LINE);
        }
        if (theFace[myTriangle] == theNextEdge && theFace[myTriangle+1] == theCurEdge) {
            return myTriangle;
        }
        if (theFace[myTriangle+1] == theNextEdge && theFace[myTriangle+2] == theCurEdge) {
            return myTriangle+1;
        }
        if (theFace[myTriangle+2] == theNextEdge && theFace[myTriangle] == theCurEdge) {
            return myTriangle+2;
        }
    }
    throw asl::Exception("matching halfedge not found", PLUS_FILE_LINE);
}

bool
HalfEdgeTable::intInArray(int theValue, const int * theArray) {
    for (int i= 0; i < 4; ++i) {
        if (theArray[i] == theValue) {
            return true;
        }
    }
    return false;
}

HalfEdgeTable::Type
HalfEdgeTable::classifyEdge(int theCurEdge, int theNextEdge) {
    const int MIN_X_EDGES[] = {8,3,11,7};
    const int MAX_X_EDGES[] = {9,1,10,5};
    const int MIN_Y_EDGES[] = {0,9,4,8};
    const int MAX_Y_EDGES[] = {2,10,6,11};
    const int MIN_Z_EDGES[] = {0,1,2,3};
    const int MAX_Z_EDGES[] = {4,5,6,7};
   
    if (intInArray(theCurEdge, MIN_X_EDGES) && intInArray(theNextEdge, MIN_X_EDGES)) {
        return MIN_X;
    }
    if (intInArray(theCurEdge, MAX_X_EDGES) && intInArray(theNextEdge, MAX_X_EDGES)) {
        return MAX_X;
    }
    if (intInArray(theCurEdge, MIN_Y_EDGES) && intInArray(theNextEdge, MIN_Y_EDGES)) {
        return MIN_Y;
    }
    if (intInArray(theCurEdge, MAX_Y_EDGES) && intInArray(theNextEdge, MAX_Y_EDGES)) {
        return MAX_Y;
    }
    if (intInArray(theCurEdge, MIN_Z_EDGES) && intInArray(theNextEdge, MIN_Z_EDGES)) {
        return MIN_Z;
    }
    if (intInArray(theCurEdge, MAX_Z_EDGES) && intInArray(theNextEdge, MAX_Z_EDGES)) {
        return MAX_Z;
    }
    return INTERNAL;
}

