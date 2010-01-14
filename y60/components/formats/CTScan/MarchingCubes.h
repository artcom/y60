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

#ifndef Y60_MARCHING_CUBES_INCLUDED
#define Y60_MARCHING_CUBES_INCLUDED

// Disable logging in inner loop for performance reasons.
// This also affects logging in MCPolicies
#define DISABLE_LOGGING

#ifdef DISABLE_LOGGING
#   define DB(x) // x
#else
#   define DB(x) x
#endif

#include "MCLookup.h"
#include "MCPolicies.h"
#include "CTScan.h"
#include "VertexCache.h"

#include <asl/math/Box.h>
#include <asl/base/Logger.h>
#include <asl/base/Assure.h>
#include <asl/base/string_functions.h>

#include <algorithm>
#include <stdio.h>


#ifdef ASSURE_POLICY
#undef ASSURE_POLICY
#endif
#define ASSURE_POLICY AssurePolicy::Throw

#include <asl/base/Assure.h>

//#define MC_DONT_SHARE_VERTICES

typedef unsigned char VmDataValue;

namespace y60 {

    DEFINE_EXCEPTION(MarchingCubesException, asl::Exception);

    const int numSegments = 16;

    template <class VoxelT, class OutputPolicy, class SegmentationPolicy>
    class MarchingCubes {
        friend class CountPolygonPolicy<VoxelT, SegmentationPolicy>;
        friend class ExportShapePolicy<VoxelT, SegmentationPolicy>;
        public:

            MarchingCubes(int theDownSampleRate, bool theCloseAtClippingBoxFlag,
                          OutputPolicy & theOutputPolicy,
                          SegmentationPolicy & theSegmentizer, CTScan * theVoxelData) :
                _myOutputPolicy(theOutputPolicy),
                _myVoxelData(theVoxelData),
                _myDownSampleRate(theDownSampleRate),
                _myCloseAtClippingBoxFlag(theCloseAtClippingBoxFlag),
                _myLineStride(0),
                _myMCLookup(),
                _mySegmentizer( theSegmentizer )
            {
                if ( ! _myVoxelData) {
                    throw MarchingCubesException("CTScan ptr is zero.", PLUS_FILE_LINE);
                }
                _myVoxelSize = theVoxelData->getVoxelAspect() * theDownSampleRate;
                _myLineStride = theVoxelData->getVoxelDimensions()[0];

                _myDimensions[0] = theVoxelData->getVoxelDimensions()[0] / _myDownSampleRate;
                _myDimensions[1] = theVoxelData->getVoxelDimensions()[1] / _myDownSampleRate;
                _myDimensions[2] = theVoxelData->getVoxelDimensions()[2] / _myDownSampleRate;

                _myVBox.makeEmpty();
                _myVBox.extendBy(asl::Vector3i(0, 0, 0));
                _myVBox.extendBy(asl::Vector3i(_myDimensions[0] - 1,
                                               _myDimensions[1] - 1,
                                               _myDimensions[2] - 1));

                for (int i=0; i < theVoxelData->getVoxelDimensions()[2]; ++i) {
                    _mySlices.push_back(theVoxelData->CTScan::getSlicePtr<VoxelT>(i));
                }
                marchAllSegments();
            }
            ~MarchingCubes() {}

            void setBox(const asl::Box3i & theBox) {
                asl::Box3i myTempBox(theBox.getMin()[0]/_myDownSampleRate,
                                     theBox.getMin()[1]/_myDownSampleRate,
                                     theBox.getMin()[2]/_myDownSampleRate,
                                     theBox.getMax()[0]/_myDownSampleRate,
                                     theBox.getMax()[1]/_myDownSampleRate,
                                     theBox.getMax()[2]/_myDownSampleRate);
                checkBox(myTempBox);
                _myVBox = myTempBox;
            }

            const asl::Box3i & getBox() const {
                return _myVBox;
            }

            void marchAllSegments () {
                for (int i = 0; i < numSegments; ++i) {
                    _myMarchSegment[i] = 1;
                }
            }
            void clearMarchAllSegments () {
                for (int i = 0; i < numSegments; i ++) {
                    _myMarchSegment[i] = 0;
                }
            }
            void setMarchSegment(int theSegment) {
                _myMarchSegment[theSegment] = 1;
            }

            const asl::Vector3i & getDataDimension() const {
                return _myDimensions;
            }

            bool march() {
                AC_TRACE << "MarchingCubes::march()";
#ifdef DISABLE_LOGGING
                AC_WARNING << "MarchingCubes: Logging disabled at compile time. See MarchingCubes.h";
#endif
                int j, i, k;
                int myUpperCacheIndex = 0;  // will be switched on first iteration
                int iBoxStart = _myVBox[asl::Box3i::MIN][0];
                int iBoxEnd   = _myVBox[asl::Box3i::MAX][0];
                int jBoxStart = _myVBox[asl::Box3i::MIN][1];
                int jBoxEnd   = _myVBox[asl::Box3i::MAX][1];
                int kBoxStart = _myVBox[asl::Box3i::MIN][2];
                int kBoxEnd   = _myVBox[asl::Box3i::MAX][2];
                if (_myCloseAtClippingBoxFlag) {
                    jBoxStart--;
                    kBoxStart--;
                    jBoxEnd++;
                    kBoxEnd++;
                }

                AC_TRACE << "Data dimension: X(" << _myDimensions[0] << "), Y("
                    << _myDimensions[1] << "), Z(" << _myDimensions[2] << ")\n";
                AC_TRACE << "Box           : X[" << iBoxStart << ", "
                    << iBoxEnd << "], Y[" << jBoxStart;
                AC_TRACE << ", " << jBoxEnd << "], Z[" << kBoxStart << ", " << kBoxEnd << "]\n";

                int myVoxelsPerSlize = _myDimensions[0] * _myDimensions[1];
                _myCache[0] = IJCachePtr(new IJCache(_myDimensions[0], _myDimensions[1]));
                _myCache[1] = IJCachePtr(new IJCache(_myDimensions[0], _myDimensions[1]));
                _myKCache = KCachePtr(new KCache(_myDimensions[0], _myDimensions[1]));

                fillOffsetTable();

                bool myContinueFlag;
                float myProgress;
                for(k = kBoxStart; k < kBoxEnd; k++) {
                    myProgress = float(k - kBoxStart) / float(kBoxEnd - kBoxStart);
                    myContinueFlag = _myVoxelData->notifyProgress(myProgress, _myOutputPolicy.getDescription());
                    if ( ! myContinueFlag ) {
                        break;
                    }
                    // switch cache
                    myUpperCacheIndex = 1 - myUpperCacheIndex;
                    _myCache[myUpperCacheIndex]->reset();
                    _myKCache->reset(-1);
                    int kCubeIndex = 0;
                    if (k < _myVBox[asl::Box3i::MIN][2]) {
                        kCubeIndex |= BIT_0 | BIT_1 | BIT_2 | BIT_3;
                    }
                    if (k >= _myVBox[asl::Box3i::MAX][2]) {
                        kCubeIndex |= BIT_4 | BIT_5 | BIT_6 | BIT_7;
                    }
                    for(j = jBoxStart; j < jBoxEnd; j++) {
                        int jCubeIndex = 0;
                        if (j < _myVBox[asl::Box3i::MIN][1]) {
                            jCubeIndex |= BIT_0 | BIT_3 | BIT_4 | BIT_7;
                        }
                        if (j >= _myVBox[asl::Box3i::MAX][1]) {
                            jCubeIndex |= BIT_1 | BIT_2 | BIT_5 | BIT_6;
                        }
                        // add empty value at iBoxStart-1
                        if (_myCloseAtClippingBoxFlag) {
                            int myCubeIndex = kCubeIndex | jCubeIndex
                                              | BIT_0 | BIT_1 | BIT_4 | BIT_5;
                            getCubeIndex(iBoxStart-1, j, k, myCubeIndex);
                            if((myCubeIndex > 0) && (myCubeIndex < 255)) {
                                _mySegmentizer.fillThresholdCube(iBoxStart-1, j, k);
                                fillVoxelCube(iBoxStart-1, j, k, _myCurrent);
                                asl::Vector3i myVoxel(iBoxStart-1, j, k);
                                triangulateVoxel(myCubeIndex, myVoxel, *_myCache[1-myUpperCacheIndex],
                                        *_myCache[myUpperCacheIndex], *_myKCache);
                            }
                        }
                        for(i = iBoxStart; i < iBoxEnd; i++) {
                            // [TS] This makes MC behave different from before. But still I
                            // believe it is more correct than before. We cant triangulate
                            // the rightmost voxels. Visual results are kindof strange. We
                            // could make it check against the myCubeIndex so we at least close
                            // everything we can... but that would still be strange...
                            int myCubeIndex = kCubeIndex | jCubeIndex;
                            getCubeIndex(i, j, k, myCubeIndex);
                            if((myCubeIndex > 0) && (myCubeIndex < 255)) {
                                _mySegmentizer.fillThresholdCube(i, j, k);
                                fillVoxelCube(i, j, k, _myCurrent);
                                asl::Vector3i myVoxel(i, j, k);
                                triangulateVoxel(myCubeIndex, myVoxel, *_myCache[1-myUpperCacheIndex],
                                        *_myCache[myUpperCacheIndex], *_myKCache);
                            }
                        }
                        // add empty value at iBoxEnd
                        if (_myCloseAtClippingBoxFlag) {
                            int myCubeIndex = kCubeIndex | jCubeIndex
                                | BIT_2 | BIT_3 | BIT_6 | BIT_7;
                            getCubeIndex(iBoxEnd, j, k, myCubeIndex);
                            if((myCubeIndex > 0) && (myCubeIndex < 255)) {
                                _mySegmentizer.fillThresholdCube(iBoxEnd, j, k);
                                fillVoxelCube(iBoxEnd, j, k, _myCurrent);
                                asl::Vector3i myVoxel(iBoxEnd, j, k);
                                triangulateVoxel(myCubeIndex, myVoxel, *_myCache[1-myUpperCacheIndex],
                                        *_myCache[myUpperCacheIndex], *_myKCache);
                            }
                        }
                    }
                }
                if ( myContinueFlag ) {
                    _myVoxelData->notifyProgress(1.0, "Setting up model...");
                }
                _myOutputPolicy.done();
                return myContinueFlag;
            }

        private:
            enum BitNames {
                BIT_0 = 1,
                BIT_1 = 2,
                BIT_2 = 4,
                BIT_3 = 8,
                BIT_4 = 16,
                BIT_5 = 32,
                BIT_6 = 64,
                BIT_7 = 128
            };



            void checkBox(asl::Box3i & theBox) {
                std::string myBoxIsBrokenMsg(std::string("MarchingCubes::CheckBox: Wrong box size! ") +
                                "(Dimension: min: 2x2x2, max: " + asl::as_string(_myDimensions[0]) + "x" +
                                asl::as_string(_myDimensions[1]) + "x" + asl::as_string( _myDimensions[2]) + ")"+
                                asl::as_string(theBox));
                if (theBox.isEmpty()) {
                    throw MarchingCubesException(myBoxIsBrokenMsg, PLUS_FILE_LINE);
                }
                for(int i = 0; i < 3; i++) {
                    if (theBox[asl::Box3i::MIN][i] < 0) {
                        theBox[asl::Box3i::MIN][i] = 0;
                    }
                    if (theBox[asl::Box3i::MAX][i] >= _myDimensions[i]) {
                        theBox[asl::Box3i::MAX][i] = _myDimensions[i]-1;
                    }
                }
            }

            inline void triangulateVoxel(int theCubeIndex, const asl::Vector3i & theMarchPos,
                    IJCache & theLowerCache, IJCache & theUpperCache, KCache & theKCache)
            {
                std::vector<int> myEdgeTable;
                myEdgeTable.resize(12, -1);
                myEdgeTable[1] = -2;
                int iMarch = theMarchPos[0];
                int jMarch = theMarchPos[1];
                int kMarch = theMarchPos[2];
                int myPositionIndex;
                const MCLookup::CubeCase & myCubeCase = _myMCLookup.cubeCases[theCubeIndex];

                DB(AC_TRACE << "triangulateVoxel(" << theCubeIndex << ", " << jMarch << "," << iMarch << "," << kMarch << ")");
                const std::vector<int> & myEdges = myCubeCase.edges;
                int myEdgeCount = myEdges.size();
                for (int i = 0; i < myEdgeCount; ++i) {
                    DB(AC_TRACE << "     edge case :" << myEdges[i]);
                    myPositionIndex = -1;
#ifndef MC_DONT_SHARE_VERTICES
                    switch (myEdges[i]) {
                        case 0:
                            myPositionIndex = theLowerCache.get(iMarch, jMarch).j;
                            break;

                        case 1:
                            myPositionIndex = theLowerCache.get(iMarch, jMarch+1).i;
                            break;

                        case 2:
                            myPositionIndex = theLowerCache.get(iMarch+1, jMarch).j;
                            break;

                        case 3:
                            myPositionIndex = theLowerCache.get(iMarch, jMarch).i;
                            break;

                        case 4:
                            myPositionIndex = theUpperCache.get(iMarch, jMarch).j;
                            break;

                        case 5:
                            break;

                        case 6:
                            break;

                        case 7:
                            myPositionIndex = theUpperCache.get(iMarch, jMarch).i;
                            break;

                        case 8:
                            myPositionIndex = theKCache.get(iMarch, jMarch);
                            break;

                        case 9:
                            myPositionIndex = theKCache.get(iMarch, jMarch+1);
                            break;

                        case 10:
                            break;

                        case 11:
                            myPositionIndex = theKCache.get(iMarch+1, jMarch);
                            break;
                        default:
                            break;
                    }
#endif
                    if (myPositionIndex < 0) {
                        myPositionIndex = _myOutputPolicy.onVertex(*this, myEdges[i], theMarchPos);
                    }
                    myEdgeTable[myEdges[i]] = myPositionIndex;
                }
                theLowerCache.get(iMarch, jMarch+1).i = myEdgeTable[1];
                theLowerCache.get(iMarch+1, jMarch).j = myEdgeTable[2];
                theUpperCache.get(iMarch, jMarch).j = myEdgeTable[4];
                theUpperCache.get(iMarch+1, jMarch).j = myEdgeTable[6];
                theUpperCache.get(iMarch, jMarch).i = myEdgeTable[7];
                theUpperCache.get(iMarch, jMarch+1).i = myEdgeTable[5];
                theKCache.get(iMarch, jMarch+1) = myEdgeTable[9];
                theKCache.get(iMarch+1, jMarch+1) = myEdgeTable[10];
                theKCache.get(iMarch+1, jMarch) = myEdgeTable[11];

                _myOutputPolicy.onHalfEdges(myCubeCase, myEdgeTable);
            }

            inline void
            computeGradient(unsigned theCubeIndex, const asl::Vector3i & thePosition,
                asl::Vector3f & theGradient, bool theUpperFlag) const
            {
                asl::Vector3i myDoublePos = 2*_myCubeTable[theCubeIndex];

                for (int i = 0; i < 3; ++i) {
                    int myIndex = _myAdjacents[theCubeIndex][i];
                    asl::Vector3i myOffset = myDoublePos - _myCubeTable[myIndex];
                    theGradient[i] = _mySegmentizer.interpolateNormal(_myCurrent[myIndex], myIndex,
                            clampedAt(thePosition + myOffset), thePosition + myOffset, theUpperFlag) / _myVoxelSize[i];
                    if (_myCubeTable[myIndex][i]) {
                        theGradient[i] = -theGradient[i];
                    }
                }
                theGradient.normalize();
            }

            inline void computeVertexPosition (int n, const asl::Vector3i & theMarchPos,
                            asl::Point3f & thePosition, asl::Vector3f * theNormal = 0) const
            {
                int iMarch = theMarchPos[0];
                int jMarch = theMarchPos[1];
                int kMarch = theMarchPos[2];
                float li = 0.5f;
                asl::Vector3f myGradient1, myGradient2;
                bool myInsideOutFlag;
                //int CUBEPAIRS[][] = {{0,1}, {1,2}, {3,2}, {0,3}, {4,5}, {5,6}, {7,6}, {4,7}, {0,4}, {1,5}, {2,6}, {3,7}};
                switch(n) {
                    case 0:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 0,1, myInsideOutFlag);

                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];
                        if (theNormal){
                            computeGradient(0, theMarchPos, myGradient1, myInsideOutFlag);
                            computeGradient(1, theMarchPos, myGradient2, myInsideOutFlag);
                        }
                        break;

                    case 1:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 1,2, myInsideOutFlag);

                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if (theNormal) {
                            computeGradient(1, theMarchPos, myGradient1, myInsideOutFlag);
                            computeGradient(2, theMarchPos, myGradient2, myInsideOutFlag);
                        }
                        break;

                    case 2:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 3,2, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(theNormal){
                            computeGradient(3, theMarchPos, myGradient1, myInsideOutFlag);
                            computeGradient(2, theMarchPos, myGradient2, myInsideOutFlag);
                        }
                        break;

                    case 3:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 0,3, myInsideOutFlag);
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(3, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(0, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    case 4:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 4,5, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(5, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(4, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    case 5:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 5,6, myInsideOutFlag);
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(6, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(5, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    case 6:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 7,6, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch+ li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(6, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(7, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    case 7:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 4,7, myInsideOutFlag);
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(4, theMarchPos, myGradient1, myInsideOutFlag);
                            computeGradient(7, theMarchPos, myGradient2, myInsideOutFlag);
                        }
                        break;

                    case 8:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 0,4, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(4, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(0, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    case 9:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 1,5, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(5, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(1, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    case 10:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 2,6, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(6, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(2, theMarchPos, myGradient1, myInsideOutFlag);
                        }

                        break;

                    case 11:
                        li = _mySegmentizer.interpolatePosition(_myCurrent, 3,7, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            computeGradient(7, theMarchPos, myGradient2, myInsideOutFlag);
                            computeGradient(3, theMarchPos, myGradient1, myInsideOutFlag);
                        }
                        break;

                    default:
                        break;
                }
                if (theNormal) {
                    if (myInsideOutFlag) {
                        *theNormal = (myGradient1 + li * (myGradient2 - myGradient1));
                    } else {
                        *theNormal = -1.0 * (myGradient1 + li * (myGradient2 - myGradient1));
                    }
                }
            }

            /**
             * Gets a (downsampled) value from the Voxel data.
             */
            inline
            VoxelT
            at(const asl::Vector3i & thePosition) const {
                return at(thePosition[0], thePosition[1], thePosition[2]);
            }

            /**
             * Gets a (downsampled) value from the Voxel data.
             */
            inline
            VoxelT
            at(int x, int y, int z) const {
                if (_myDownSampleRate == 1) {
                    return _mySlices[z*_myDownSampleRate][(_myLineStride*y + x)*_myDownSampleRate];
                } else {
                    int myCount = 0;
                    float myValue = 0.0f;
                    for (int i = 0; i < _myDownSampleRate; ++i) {
                        for (int j = 0; j < _myDownSampleRate; ++j) {
                            for (int k = 0; k < _myDownSampleRate; ++k) {
                                myCount++;
                                myValue +=_mySlices[z*_myDownSampleRate+i][_myLineStride* (y*_myDownSampleRate+j)+
                                                    x*_myDownSampleRate+k];
                            }
                        }
                    }
                    return VoxelT(myValue / float(myCount));
                }
            }

            /**
             * Gets a value from the Voxel data by the downsampled sliceindex and offset in the slice
             */
            /*
            inline
            VoxelT
            at(const asl::Vector2i & theOffset) const {
                return _mySlices[theOffset[1]][theOffset[0]];
            }
            */
            inline
            VoxelT
            clampedAt(int x, int y, int z) const {
                return clampedAt(asl::Vector3i(x,y,z));
            }

            inline
            VoxelT
            clampedAt(const asl::Vector3i & thePosition) const {
                if ((thePosition[0] >= _myVBox[asl::Box3i::MIN][0]) && (thePosition[0] <= _myVBox[asl::Box3i::MAX][0])
                 && (thePosition[1] >= _myVBox[asl::Box3i::MIN][1]) && (thePosition[1] <= _myVBox[asl::Box3i::MAX][1])
                 && (thePosition[2] >= _myVBox[asl::Box3i::MIN][2]) && (thePosition[2] <= _myVBox[asl::Box3i::MAX][2]))
                {
                    return at(asl::asPoint(thePosition));
                } else {
                    return asl::NumericTraits<VoxelT>::min();
                }
            }
            /*
            // Fast but ugly method. Deprecate this if possible.
            inline VoxelT
            getValueByCubeCorner(int iMarch, int jMarch, int kMarch, int theCubeCorner) {
                switch (theCubeCorner) {
                case 0:
                    return at(iMarch, jMarch, kMarch);
                case 1:
                    return at(iMarch, jMarch+1, kMarch);
                case 2:
                    return at(iMarch+1, jMarch+1, kMarch);
                case 3:
                    return at(iMarch+1, jMarch, kMarch);
                case 4:
                    return at(iMarch, jMarch, kMarch+1);
                case 5:
                    return at(iMarch, jMarch+1, kMarch+1);
                case 6:
                    return at(iMarch+1, jMarch+1, kMarch+1);
                case 7:
                    return at(iMarch+1, jMarch, kMarch+1);
                default:
                    throw MarchingCubesException(std::string("Illegal CubeIndex: ") + asl::as_string(theCubeCorner), PLUS_FILE_LINE);
                }
            }
            */

            inline void
            fillVoxelCube(int theI, int theJ, int theK, std::vector<VoxelT> & theVoxelCube) {
                theVoxelCube.clear();
                theVoxelCube.reserve(8);
                theVoxelCube.push_back(clampedAt(theI, theJ, theK));
                theVoxelCube.push_back(clampedAt(theI, theJ+1, theK));
                theVoxelCube.push_back(clampedAt(theI+1, theJ+1, theK));
                theVoxelCube.push_back(clampedAt(theI+1, theJ, theK));
                theVoxelCube.push_back(clampedAt(theI, theJ, theK+1));
                theVoxelCube.push_back(clampedAt(theI, theJ+1, theK+1));
                theVoxelCube.push_back(clampedAt(theI+1, theJ+1, theK+1));
                theVoxelCube.push_back(clampedAt(theI+1, theJ, theK+1));
            }

            void
            fillOffsetTable() {
                // 1st component : byte offset within slice (i.e. x+y*stride),
                // 2nd component : slice offset within slice-stack
                _myOffsetTable[0] = asl::Vector2i(0,0);  // [0,0,0]
                _myOffsetTable[1] = asl::Vector2i(_myLineStride * _myDownSampleRate, 0);  // [0,1,0]
                _myOffsetTable[2] = asl::Vector2i((1+_myLineStride) * _myDownSampleRate, 0);  // [1,1,0]
                _myOffsetTable[3] = asl::Vector2i(_myDownSampleRate, 0); // [1,0,0]
                _myOffsetTable[4] = asl::Vector2i(0, _myDownSampleRate);  // [0,0,1]
                _myOffsetTable[5] = asl::Vector2i(_myLineStride * _myDownSampleRate, _myDownSampleRate);  // [0,1,1]
                _myOffsetTable[6] = asl::Vector2i((1+_myLineStride) * _myDownSampleRate, _myDownSampleRate);  // [1,1,1]
                _myOffsetTable[7] = asl::Vector2i(_myDownSampleRate, _myDownSampleRate); // [1,0,1]
                _myCubeTable[0] = asl::Vector3i(0,0,0);
                _myCubeTable[1] = asl::Vector3i(0,1,0);
                _myCubeTable[2] = asl::Vector3i(1,1,0);
                _myCubeTable[3] = asl::Vector3i(1,0,0);
                _myCubeTable[4] = asl::Vector3i(0,0,1);
                _myCubeTable[5] = asl::Vector3i(0,1,1);
                _myCubeTable[6] = asl::Vector3i(1,1,1);
                _myCubeTable[7] = asl::Vector3i(1,0,1);
                _myBitMaskTable[0] = BIT_0;
                _myBitMaskTable[1] = BIT_1;
                _myBitMaskTable[2] = BIT_2;
                _myBitMaskTable[3] = BIT_3;
                _myBitMaskTable[4] = BIT_4;
                _myBitMaskTable[5] = BIT_5;
                _myBitMaskTable[6] = BIT_6;
                _myBitMaskTable[7] = BIT_7;
                unsigned int ADJACENTS[8][3] = {{3, 1, 4}, {2, 0, 5}, {1, 3, 6}, {0, 2, 7}, {7, 5, 0}, {6, 4, 1}, {5, 7, 2}, {4, 6, 3}};
                for (int i = 0; i < 8 * 3; ++i) {
                    _myAdjacents[i/3][i%3] = ADJACENTS[i/3][i%3];
                }
            }

            inline void
            getCubeIndex(int theI, int theJ, int theK, int & theBitMask) const {
                if( ((theBitMask & BIT_0) == 0) && _mySegmentizer.isOutside(theI, theJ, theK, at(theI, theJ, theK))) {
                    theBitMask |= BIT_0;
                }
                if( ((theBitMask & BIT_1) == 0) && _mySegmentizer.isOutside(theI, theJ+1, theK, at(theI, theJ+1, theK))) {
                    theBitMask |= BIT_1;
                }
                if( ((theBitMask & BIT_2) == 0) && _mySegmentizer.isOutside(theI+1, theJ+1, theK, at(theI+1, theJ+1, theK))) {
                    theBitMask |= BIT_2;
                }
                if( ((theBitMask & BIT_3) == 0) && _mySegmentizer.isOutside(theI+1, theJ, theK, at(theI+1, theJ, theK))) {
                    theBitMask |= BIT_3;
                }
                if( ((theBitMask & BIT_4) == 0) && _mySegmentizer.isOutside(theI, theJ, theK+1, at(theI, theJ, theK+1))) {
                    theBitMask |= BIT_4;
                }
                if( ((theBitMask & BIT_5) == 0) && _mySegmentizer.isOutside(theI, theJ+1, theK+1, at(theI, theJ+1, theK+1))) {
                    theBitMask |= BIT_5;
                }
                if( ((theBitMask & BIT_6) == 0) && _mySegmentizer.isOutside(theI+1, theJ+1, theK+1, at(theI+1, theJ+1, theK+1))) {
                    theBitMask |= BIT_6;
                }
                if( ((theBitMask & BIT_7) == 0) && _mySegmentizer.isOutside(theI+1, theJ, theK+1, at(theI+1, theJ, theK+1))) {
                    theBitMask |= BIT_7;
                }
                return;
            }


            //====================================================================
            // Private Members
            //====================================================================

            CTScan * _myVoxelData;
            asl::Vector3i _myDimensions;
            asl::Vector3f _myVoxelSize;
            asl::Box3i _myVBox;

            IJCachePtr   _myCache[2];
            KCachePtr    _myKCache;
            std::vector<VoxelT> _myCurrent;
            int  _myMarchSegment[numSegments];
            MCLookup _myMCLookup;

            const int _myDownSampleRate;
            bool _myCloseAtClippingBoxFlag;
            int _myLineStride;
            int _mySliceOffset;

            OutputPolicy       & _myOutputPolicy;
            SegmentationPolicy & _mySegmentizer;

            std::vector<const VoxelT*>   _mySlices;
            std::vector<const unsigned char*> _myStencils;
            asl::Vector2i                _myOffsetTable[8];
            unsigned                     _myBitMaskTable[8];
            asl::Vector3i                _myCubeTable[8];
            /// stores adjacency information for each of the 8 voxels in the cube
            /// first coordinate is the voxel id in the cube, the second coordiate
            /// is the axis (i,j,k).
            /// i.E.: _myAdjacents[3][1] tells us the neighbor voxel of voxel 3 in j direction
            unsigned int                 _myAdjacents[8][3];

    };

} // end of namespace y60

// cleanup defines
#undef DB
#undef DISABLE_LOGGING

#endif // Y60_MARCHING_CUBES_INCLUDED
