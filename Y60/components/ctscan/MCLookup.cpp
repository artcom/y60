#include "MCLookup.h"

#include <asl/base/Exception.h>
#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>
#include <algorithm>

using namespace std;
using namespace asl;

struct Pattern {
    Unsigned8 cubeCase;
    int face(int i) {
        switch (i % 3) {
            case 0:
                return _myFaces[i+2];
            case 1:
                return _myFaces[i];
            case 2:
                return _myFaces[i-2];
            default:
                throw Exception("modulo is broken",PLUS_FILE_LINE);
        }
    }
    const int _myFaces[15];
    
};
Pattern ourPatterns[] = {
// these are the base 15 pattern from Lorensen and Cline, (Sharman,1998)
    { 0, { -1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}} ,
    { 1, { 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}} ,
    { 3, { 1, 8, 3, 1, 9, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1}} ,
    {33, { 0, 8, 3, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1}} ,
    {65, { 0, 8, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}} ,
    {13, { 0, 8, 10, 8, 11, 10, 1, 0, 10, -1, -1, -1, -1, -1, -1}} ,
    {67, { 1, 8, 3, 1, 9, 8, 6, 5, 10, -1, -1, -1, -1, -1, -1}} ,
   {161, {0, 8, 3, 5, 4, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1}} ,
    {15, {9, 8, 11, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1}} ,
    {27, {4, 7, 9, 9, 7, 1, 1, 7, 11, 11, 2, 1, -1, -1, -1}} ,
    {85, {0, 4, 7, 0, 7, 3, 1, 6, 5, 1, 2, 6, -1, -1, -1}} ,
   {139, {1, 9, 8, 1, 8, 6, 8, 7, 6, 1, 6 ,2, -1, -1, -1}} ,
    {45, {4, 9, 5, 11, 10, 8, 8, 10, 0, 10, 1, 0, -1, -1, -1}} ,
   {165, {0, 8, 3, 4, 9, 5, 6, 11, 7, 1, 2, 10, -1, -1, -1}} ,
    {29, {0, 4, 1, 1, 4, 11, 1, 11, 10, 11, 4, 7, -1, -1, -1}},
// these are the extra patterns by Shoeb
    {143, {9, 8, 7, 7, 10, 9, 6, 10, 7, -1, -1, -1, -1, -1, -1}},
    {203, {8, 7, 5, 5, 2, 8,10, 2,  5, 9, 8, 2, 1, 9, 2}},
    {207, {9, 8, 7, 7, 5, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {173, {9, 5, 4, 7, 0, 8, 6, 0, 7, 1, 0, 6, 6, 10, 1}},
    {235, {4, 8, 7, 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {239, {7, 4, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {245, {0, 11, 3, 0, 9, 11, 2, 11, 9, 2, 9, 1, -1, -1, -1}},
    {255, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}
};

// helper tables to rotate the above patterns
const int rot_k_v[8] = { 1, 2, 3, 0, 5, 6, 7, 4 }; // how to rotate the cubeCase bits
const int rot_k_e[12] = { 1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8}; // how to rotate the face values

const int rot_i_v[8] = { 3, 2, 6, 7, 0, 1, 5, 4 };
const int rot_i_e[12] = { 2, 10, 6, 11, 0, 9, 4, 8, 3, 1, 5, 7 };

const int rot_j_v[8] = { 4, 0, 3, 7, 5, 1, 2, 6 };
const int rot_j_e[12] = { 8, 3, 11, 7, 9, 1, 10, 5, 4, 0, 2, 6 };

MCLookup::MCLookup() {
    cubeCases.resize(256);
    initFaceData();
    initEdgeData();
    initHalfEdgeData();
}

void
MCLookup::addFaceData(int theCubeCase, const vector<int> & theFaces) {
    if (!cubeCases[theCubeCase].faces.empty()) {
        throw asl::Exception(string("Case ") + as_string(theCubeCase) + " already filled", PLUS_FILE_LINE);
    }
    cubeCases[theCubeCase].faces = theFaces;
}

void
MCLookup::rotatePattern(int & theCubeCase, vector<int> & theFaces, const int * theCaseRotation, const int* theFaceRotation) {
  int index_buf=0;

  for(int i = 0; i < 8; i++ )
    if(theCubeCase & ( 1<<i ))
        index_buf |= (1<<theCaseRotation[i]);

  theCubeCase = index_buf;

  for (int i = 0; i < theFaces.size(); ++i) {
      theFaces[i] = theFaceRotation[theFaces[i]];
  }
}

void 
MCLookup::addPattern(int thePatternIndex, map<int,int> & theMatchedMap) {
    int myCubeCase = ourPatterns[thePatternIndex].cubeCase;
    vector<int> myFaces;

    for (int i = 0; i < 15; ++i) {
        if (ourPatterns[thePatternIndex].face(i) == -1) {
            break;
        }
        myFaces.push_back(ourPatterns[thePatternIndex].face(i) );
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                // sanity check
                map<int,int>::iterator pos = theMatchedMap.find(myCubeCase);
                if (pos != theMatchedMap.end()) {
                    if (pos->second != thePatternIndex) {
                        throw asl::Exception(string("while writing ")+as_string(myCubeCase)+
                                ", derived from "+as_string(thePatternIndex)+
                                " conflict with "+as_string(pos->second), PLUS_FILE_LINE);
                    }
                } else {
                    theMatchedMap.insert(make_pair(myCubeCase, thePatternIndex));
                    addFaceData(myCubeCase, myFaces);
                }
                rotatePattern(myCubeCase, myFaces, rot_k_v, rot_k_e );
            }
            rotatePattern(myCubeCase, myFaces, rot_j_v, rot_j_e );
        }
        rotatePattern(myCubeCase, myFaces, rot_i_v, rot_i_e );
    }
}

void
MCLookup::initFaceData() {
    AC_INFO << "initFaceData";
    map<int, int> myMatchedCases;
    for (int i = 0; i < sizeof(ourPatterns)/sizeof(ourPatterns[0]); ++i) {
        addPattern(i, myMatchedCases);
    } 
       
    /*
    for (int i = 0; i < 256; ++i) {
        std::vector<int> & myLine = cubeCases[i].faces;
        for (int j = 0; j < 12; ++j) {
            if (ourFaceData[i][j] == -1) {
                break;
            }
            myLine.push_back(ourFaceData[i][j]);
        }
    }
    */
    if (myMatchedCases.size() != 256) {
        throw asl::Exception(string("only have ") + as_string(myMatchedCases.size()) + " of 256 cases!", PLUS_FILE_LINE);
    }
}

void
MCLookup::initEdgeData() {
    AC_INFO << "initEdgeData";
    const int edge_reduce[12] = { 11, 1, 2, 0, 4, 6, 3, 7, 5, 8, 9, 10};

    for (int i = 0; i < 256; ++i) {
        const std::vector<int> & myFaces = cubeCases[i].faces;
        std::vector<int> & myEdges = cubeCases[i].edges;
        for (int j = 0; j < 12; ++j) {
            vector<int>::const_iterator pos = find(myFaces.begin(), myFaces.end(), edge_reduce[j]);
            if (pos != myFaces.end()) {
                myEdges.push_back(edge_reduce[j]);
            }
        }
    }
}
void
MCLookup::initHalfEdgeData() {
    AC_INFO << "initHalfEdgeData";
    for (int i = 0; i < 256; ++i) {
        std::vector<HalfEdgeNeighbor> & myLine = cubeCases[i].neighbors;
        const std::vector<int> & myFaces = cubeCases[i].faces;
        for (int j = 0; j < myFaces.size(); ++j) {
            int curEdge = myFaces[j];
            int nextEdge = myFaces[ (j/3)*3 + ((j+1)%3) ]; 
            HalfEdgeNeighbor myNeighbor;
            myNeighbor.type = classifyEdge(curEdge, nextEdge );
            if (myNeighbor.type == INTERNAL) {
                myNeighbor.internal_index = calcInnerEdge(myFaces, curEdge, nextEdge);
            }
            myLine.push_back(myNeighbor);
        }
    }
}

int
MCLookup::calcInnerEdge(const std::vector<int> & theFaces, int theCurEdge, int theNextEdge) {
    for (int myTriangle = 0; myTriangle < theFaces.size(); myTriangle +=3) {
        if (theFaces[myTriangle] == theNextEdge && theFaces[myTriangle+1] == theCurEdge) {
            return myTriangle;
        }
        if (theFaces[myTriangle+1] == theNextEdge && theFaces[myTriangle+2] == theCurEdge) {
            return myTriangle+1;
        }
        if (theFaces[myTriangle+2] == theNextEdge && theFaces[myTriangle] == theCurEdge) {
            return myTriangle+2;
        }
    }
    throw asl::Exception("matching halfedge not found " + as_string(theCurEdge) + " / " + as_string(theNextEdge) , PLUS_FILE_LINE);
}

bool
MCLookup::intInArray(int theValue, const int * theArray) {
    for (int i= 0; i < 4; ++i) {
        if (theArray[i] == theValue) {
            return true;
        }
    }
    return false;
}

MCLookup::HalfEdgeType
MCLookup::classifyEdge(int theCurEdge, int theNextEdge) {
    const int MIN_X_EDGES[] = {0,9,4,8};
    const int MAX_X_EDGES[] = {2,10,6,11};
    const int MIN_Y_EDGES[] = {8,3,11,7};
    const int MAX_Y_EDGES[] = {9,1,10,5};
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

