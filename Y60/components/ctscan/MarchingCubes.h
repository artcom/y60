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

#include <asl/Box.h>
#include <asl/Logger.h>
#include <asl/Assure.h>
#include <asl/string_functions.h>

#include <algorithm>
#include <stdio.h>


#ifdef ASSURE_POLICY
#undef ASSURE_POLICY
#endif
#define ASSURE_POLICY AssurePolicy::Throw

#include <asl/Assure.h>

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

            MarchingCubes(int theDownSampleRate, OutputPolicy & theOutputPolicy,
                          SegmentationPolicy & theSegmentizer, CTScan * theVoxelData) :
                _myOutputPolicy(theOutputPolicy),
                _myVoxelData(theVoxelData),
                _myDownSampleRate(theDownSampleRate),
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
                int myCubeIndex;
                int myUpperCacheIndex = 0;  // will be switched on first iteration
                const int iBoxStart = _myVBox[asl::Box3i::MIN][0];
                const int iBoxEnd   = _myVBox[asl::Box3i::MAX][0];
                const int jBoxStart = _myVBox[asl::Box3i::MIN][1];
                const int jBoxEnd   = _myVBox[asl::Box3i::MAX][1];
                const int kBoxStart = _myVBox[asl::Box3i::MIN][2];
                const int kBoxEnd   = _myVBox[asl::Box3i::MAX][2];

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
                    for(j = jBoxStart; j < jBoxEnd; j++) {
                        for(i = iBoxStart; i < iBoxEnd; i++) {
                            // [TS] This makes MC behave different from before. But still I
                            // believe it is more correct than before. We cant triangulate
                            // the rightmost voxels. Visual results are kindof strange. We
                            // could make it check against the myCubeIndex so we at least close
                            // everything we can... but that would still be strange...
                            myCubeIndex = getCubeIndex(i, j, k);
                            if((myCubeIndex > 0) && (myCubeIndex < 255)) {
                                _mySegmentizer.fillThresholdCube(i, j, k);
                                fillVoxelCube(i, j, k, _myCurrent);
                                asl::Vector3i myVoxel(i, j, k);
                                triangulateVoxel(myCubeIndex, myVoxel, *_myCache[1-myUpperCacheIndex], *_myCache[myUpperCacheIndex], *_myKCache);
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

            inline void triangulateVoxel(int theCubeIndex, const asl::Vector3i & theMarchPos, IJCache & theLowerCache, IJCache & theUpperCache, KCache & theKCache) {
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
                    theGradient[i] = _mySegmentizer.interpolateNormal(_myCurrent[myIndex], myIndex, clampedAt(thePosition + myOffset), thePosition + myOffset, theUpperFlag) / _myVoxelSize[i];
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
            const VoxelT &
            at(const asl::Vector3i & thePosition) const {
                return at(thePosition[0], thePosition[1], thePosition[2]);
            }

            /**
             * Gets a (downsampled) value from the Voxel data. 
             */
            inline
            const VoxelT &
            at(int x, int y, int z) const {
                return at(asl::Vector2i((_myLineStride*y + x)*_myDownSampleRate, z*_myDownSampleRate));
            }

            /**
             * Gets a value from the Voxel data by the downsampled sliceindex and offset in the slice
             */
            inline
            const VoxelT &
            at(const asl::Vector2i & theOffset) const {
                return _mySlices[theOffset[1]][theOffset[0]];
            }

            inline
            const VoxelT 
            clampedAt(const asl::Vector3i & thePosition) const {
                asl::Point3i myClippedPosition = thePosition;
                if (_myVBox.contains(myClippedPosition)) {
                    return at(myClippedPosition);
                } else {
                    return asl::NumericTraits<VoxelT>::min();
                }
            }

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

            inline void
            fillVoxelCube(int theI, int theJ, int theK, std::vector<VoxelT> & theVoxelCube) {
                theVoxelCube.clear();
                theVoxelCube.reserve(8);
                int myDownSampledSlice = theK * _myDownSampleRate;
                int myDownSampledPosition = (theJ * _myLineStride + theI) * _myDownSampleRate;
                int mySlicePosition;
                int mySliceOffset;
                for (int i = 0; i < 8; ++i) {
                    mySlicePosition = myDownSampledSlice+_myOffsetTable[i][1];
                    mySliceOffset = myDownSampledPosition+_myOffsetTable[i][0];
                    theVoxelCube.push_back(_mySlices[mySlicePosition][mySliceOffset]); 
                    // This is a bit faster but less elegant:
                    // theVoxelCube.push_back(getValueByCubeCorner(iMarch, jMarch, kMarch, i));
                }
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

            inline int 
            getCubeIndex(int theI, int theJ, int theK) const {
                unsigned int myBitMask = 0;


                //const VoxelT & myValue = at(x, y, z);

                if(_mySegmentizer.isOutside(theI, theJ, theK, at(theI, theJ, theK))) {
                    myBitMask |= BIT_0;
                }
                if(_mySegmentizer.isOutside(theI, theJ+1, theK, at(theI, theJ+1, theK))) {
                    myBitMask |= BIT_1;
                }
                if(_mySegmentizer.isOutside(theI+1, theJ+1, theK, at(theI+1, theJ+1, theK))) {
                    myBitMask |= BIT_2;
                }
                if(_mySegmentizer.isOutside(theI+1, theJ, theK, at(theI+1, theJ, theK))) {
                    myBitMask |= BIT_3;
                }
                if(_mySegmentizer.isOutside(theI, theJ, theK+1, at(theI, theJ, theK+1))) {
                    myBitMask |= BIT_4;
                }
                if(_mySegmentizer.isOutside(theI, theJ+1, theK+1, at(theI, theJ+1, theK+1))) {
                    myBitMask |= BIT_5;
                }
                if(_mySegmentizer.isOutside(theI+1, theJ+1, theK+1, at(theI+1, theJ+1, theK+1))) {
                    myBitMask |= BIT_6;
                }
                if(_mySegmentizer.isOutside(theI+1, theJ, theK+1, at(theI+1, theJ, theK+1))) {
                    myBitMask |= BIT_7;
                }
                return myBitMask;
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
