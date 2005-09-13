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

#include "MCLookup.h"
#include "MCPolicies.h"
#include "CTScan.h"
#include "VertexCache.h"

#include <asl/Box.h>
#include <asl/Logger.h>
#include <asl/Assure.h>

#include <algorithm>
#include <stdio.h>

//#define MC_DONT_SHARE_VERTICES

typedef unsigned char VmDataValue;

namespace y60 {

    DEFINE_EXCEPTION(MarchingCubesException, asl::Exception);
    
    const int numSegments = 16;

    template <class VoxelT, class OutputPolicy>
    class MarchingCubes {
        friend class CountPolygonPolicy<VoxelT>;
        friend class ExportShapePolicy<VoxelT>;
        public:

            MarchingCubes(int theDownSampleRate, OutputPolicy & theOutputPolicy, CTScan * theVoxelData) :
                _myOutputPolicy(theOutputPolicy),
                _myVoxelData(theVoxelData),
                _myDownSampleRate(theDownSampleRate),
                _myLineStride(0),
                _myMCLookup()                
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
                /*
                const SegmentationBitmap & myStencils = theVoxelData->getStencil();
                for (int i = 0; i < myStencils.size(); ++i) {
                    _myStencils.push_back(myStencils[i]->pixels().begin());
                }
                */
                marchAllSegments();
            }
            ~MarchingCubes() {}

            void setThreshold(asl::Vector2<VoxelT> theThreshold) {
                _myCurrentThresholds.resize(8, theThreshold);
            }

            void setBox(const asl::Box3i & theBox) {
                asl::Box3i myTempBox(theBox.getMin()[0]/_myDownSampleRate,
                                     theBox.getMin()[1]/_myDownSampleRate,
                                     theBox.getMin()[2]/_myDownSampleRate,
                                     theBox.getMax()[0]/_myDownSampleRate,
                                     theBox.getMax()[1]/_myDownSampleRate,
                                     theBox.getMax()[2]/_myDownSampleRate);
                checkBox(myTempBox);
                _myVBox = myTempBox;
                asl::Vector3i mySize = _myVBox.getSize();
                _myDimBox[0]= mySize[0] + 1;
                _myDimBox[1]= mySize[1] + 1;
                _myDimBox[2]= mySize[2] + 1;
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
            const asl::Vector3i & getBoxDimension() const {
                return _myDimBox;
            }
            
            void march() {
                AC_TRACE << "MarchingCubes::march()";
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
                AC_TRACE << "Box dimension : X(" << _myDimBox[0] << "), Y(" 
                    << _myDimBox[1] << "), Z(" << _myDimBox[2] << ")\n";
                AC_TRACE << "Box           : X[" << iBoxStart << ", " 
                    << iBoxEnd << "], Y[" << jBoxStart;
                AC_TRACE << ", " << jBoxEnd << "], Z[" << kBoxStart << ", " << kBoxEnd << "]\n";

                int myVoxelsPerSlize = _myDimensions[0] * _myDimensions[1];
                _myCache[0] = IJCachePtr(new IJCache(_myDimensions[0], _myDimensions[1]));
                _myCache[1] = IJCachePtr(new IJCache(_myDimensions[0], _myDimensions[1]));
                _myKCache = KCachePtr(new KCache(_myDimensions[0], _myDimensions[1]));

                fillOffsetTable();

                for(k = kBoxStart; k < kBoxEnd; k++) {
                    _myVoxelData->notifyProgress(double(k - kBoxStart) / double(kBoxEnd - kBoxStart),
                            _myOutputPolicy.getDescription());
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
                                fillVoxelCube(i, j, k, _myCurrent);
                                asl::Vector3i myVoxel(i, j, k);
                                triangulateVoxel(myCubeIndex, myVoxel, *_myCache[1-myUpperCacheIndex], *_myCache[myUpperCacheIndex], *_myKCache);
                            }
                        }
                    }
                }
                _myOutputPolicy.done();
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

                AC_TRACE << "triangulateVoxel(" << theCubeIndex << ", " << jMarch << "," << iMarch << "," << kMarch << ")";
                const std::vector<int> & myEdges = myCubeCase.edges;
                int myEdgeCount = myEdges.size();
                for (int i = 0; i < myEdgeCount; ++i) {
                    AC_TRACE << "     edge case :" << myEdges[i];
                    myPositionIndex = -1;
#ifndef MC_DONT_SHARE_VERTICES
//                    int myReference;
//                    int myNewCache;
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

            inline void calcGradient(float dx, float dy, float dz, asl::Vector3f & g) const {
                g[0] = dx / _myVoxelSize[0];
                g[1] = dy / _myVoxelSize[1];
                g[2] = dz / _myVoxelSize[2];

                g.normalize();
            }

            inline float interpolatePosition(int theFirstIndex, int theSecondIndex, 
                        bool & theInsideOutFlag) const 
            {
                const VoxelT & firstValue = _myCurrent[theFirstIndex];
                const VoxelT & secondValue = _myCurrent[theSecondIndex];
                const asl::Vector2<VoxelT> & firstThreshold = _myCurrentThresholds[theFirstIndex];
                const asl::Vector2<VoxelT> & secondThreshold = _myCurrentThresholds[theSecondIndex];
                
                // XXX: TODO: interpolate between thresholds
                float li = (float)(firstThreshold[0] - firstValue) / (float)(secondValue - firstValue);

                if ((firstValue <= firstThreshold[0] && secondValue >= secondThreshold[0]) ||
                    (firstValue >= firstThreshold[0] && secondValue <= secondThreshold[0])) 
                {
                    theInsideOutFlag = false;
                    return li;
                } else {
                    if ((firstValue <= firstThreshold[1] && secondValue >= secondThreshold[1]) ||
                        (firstValue >= firstThreshold[1] && secondValue <= secondThreshold[1])) 
                    {
                        theInsideOutFlag = true;
                        return li;
                    } else {
                        throw MarchingCubesException(std::string("Threshold is neither crossed from top or bottom with first: " ) + 
                            as_string(int(firstValue)) + " and second: " + as_string(int(secondValue)), PLUS_FILE_LINE);
                    }
                }
            }            
            inline void computeVertexPosition (int n, const asl::Vector3i & theMarchPos, 
                            asl::Point3f & thePosition, asl::Vector3f * theNormal = 0) const 
            {
                int iMarch = theMarchPos[0];
                int jMarch = theMarchPos[1];
                int kMarch = theMarchPos[2];
                float li = 0.5f;
                asl::Vector3f g0, g1, g2, g3, g4, g5, g6, g7;
                bool myInsideOutFlag;
                switch(n) {
                    case 0:
                        li = interpolatePosition(0,1, myInsideOutFlag);
                        
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];
                        if (theNormal){
                            calcGradient(float(_myCurrent[3]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch)), float(_myCurrent[1]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch)), float(_myCurrent[4]) - clampedAt(asl::Vector3i(iMarch, jMarch, kMarch-1)), g0);
                            calcGradient(float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch))) - _myCurrent[0], float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch-1)), g1);
                            *theNormal = -1.0 * (g0 + li * (g1 - g0));
                        }
                        break;

                    case 1:
                        li = interpolatePosition(1,2, myInsideOutFlag);
                        
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if (theNormal) {
                            calcGradient(float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch))) - _myCurrent[0], float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch-1)), g1);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch))) - _myCurrent[1], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch))) - _myCurrent[3], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch-1)), g2);
                            *theNormal = -1.0 * (g1 + li * (g2 - g1));
                        }
                        break;

                    case 2:
                        li = interpolatePosition(3,2, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(theNormal){
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch))) - _myCurrent[1], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch))) - _myCurrent[3], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch-1)), g2);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch))) - _myCurrent[0], float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch)), float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch-1)), g3);
                            *theNormal = -1.0 * (g3 + li * (g2 - g3));
                        }
                        break;

                    case 3:
                        li = interpolatePosition(0,3, myInsideOutFlag);
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(_myCurrent[3]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch)), float(_myCurrent[1]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch)), float(_myCurrent[4]) - clampedAt(asl::Vector3i(iMarch, jMarch, kMarch-1)), g0);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch))) - _myCurrent[0], float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch)), float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch-1)), g3);
                            *theNormal = -1.0 * (g0 + li * (g3 - g0));
                        }
                        break;

                    case 4:
                        li = interpolatePosition(4,5, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch+1)), float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch, kMarch+2))) - _myCurrent[0], g4);
                            calcGradient(float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch+1))) - _myCurrent[4], float(clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch+2))) - _myCurrent[1], g5);
                            *theNormal = -1.0 * (g4 + li * (g5 - g4));
                        }
                        break;

                    case 5:
                        li = interpolatePosition(5,6, myInsideOutFlag);
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch+1))) - _myCurrent[4], float(clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch+2))) - _myCurrent[1], g5);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch+1))) - _myCurrent[5], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch+1))) - _myCurrent[7], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch+2))) - _myCurrent[2], g6);
                            *theNormal = -1.0 * (g5 + li * (g6 - g5));
                        }
                        break;

                    case 6:
                        li = interpolatePosition(7,6, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = ((float)jMarch+ li) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch+1))) - _myCurrent[5], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch+1))) - float(_myCurrent[7]), float(clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch+2))) - _myCurrent[2], g6);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch+1))) - _myCurrent[4], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch+2))) - _myCurrent[3], g7);
                            *theNormal = -1.0 * (g7 + li * (g6 - g7));
                        }
                        break;

                    case 7:
                        li = interpolatePosition(4,7, myInsideOutFlag);
                        thePosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch+1)), float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch, kMarch+2))) - _myCurrent[0], g4);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch+1))) - _myCurrent[4], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch+2))) - _myCurrent[3], g7);
                            *theNormal = -1.0 * (g4 + li * (g7 - g4));
                        }
                        break;

                    case 8:
                        li = interpolatePosition(0,4, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(_myCurrent[3]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch)), float(_myCurrent[1]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch)), float(_myCurrent[4]) - clampedAt(asl::Vector3i(iMarch, jMarch, kMarch-1)), g0);
                            calcGradient(float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch+1)), float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch, kMarch+2))) - _myCurrent[0], g4);
                            *theNormal = -1.0 * (g0 + li * (g4 - g0));
                        }
                        break;

                    case 9:
                        li = interpolatePosition(1,5, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch))) - _myCurrent[0], float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch-1)), g1);
                            calcGradient(float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch+1))) - _myCurrent[4], float(clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch+2))) - _myCurrent[1], g5);
                            *theNormal = -1.0 * (g1 + li * (g5 - g1));
                        }
                        break;

                    case 10:
                        li = interpolatePosition(2,6, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch))) - _myCurrent[1], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch))) - _myCurrent[3], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch-1)), g2);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch+1))) - _myCurrent[5], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch+1))) - _myCurrent[7], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch+2))) - _myCurrent[2], g6);
                            *theNormal = -1.0 * (g2 + li * (g6 - g2));
                        }

                        break;

                    case 11:
                        li = interpolatePosition(3,7, myInsideOutFlag);
                        thePosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        thePosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        thePosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(theNormal) {
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch))) - _myCurrent[0], float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch)), float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch-1)), g3);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch+1))) - _myCurrent[4], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch+2))) - _myCurrent[3], g7);
                            *theNormal = -1.0 * (g3 + li * (g7 - g3));
                        }
                        break;

                    default:
                        break;
                }
                if (myInsideOutFlag && theNormal) {
                    *theNormal = -(*theNormal);
                }

            }

            inline bool
            isOutside(int x, int y, int z) const {
                const VoxelT & myValue = at(x, y, z);
                return myValue < _myCurrentThresholds[0][0] || myValue > _myCurrentThresholds[0][1];
            }

            inline int findThresholdBoundary(const VoxelT theFirstValue, const VoxelT theSecondValue) const {
                if ((theFirstValue <= _myThreshold[0] && theSecondValue >= _myThreshold[0]) ||
                    (theFirstValue >= _myThreshold[0] && theSecondValue <= _myThreshold[0])) 
                {
                    return 0;
                } else {
                    if ((theFirstValue <= _myThreshold[1] && theSecondValue >= _myThreshold[1]) ||
                        (theFirstValue >= _myThreshold[1] && theSecondValue <= _myThreshold[1])) 
                    {
                        return 1;
                    } else {
                        throw MarchingCubesException(std::string("Threshold is neither crossed from top or bottom with first: " ) + 
                            as_string(int(theFirstValue)) + " and second: " + as_string(int(theSecondValue)), PLUS_FILE_LINE);
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
            const VoxelT &
            clampedAt(const asl::Vector3i & thePosition) const {
                asl::Vector3i myClippedPosition = thePosition;
                for (int i = 0; i < 3; ++i) {
                    if (thePosition[i] < _myVBox[asl::Box3i::MIN][i]) {
                        myClippedPosition[i] = _myVBox[asl::Box3i::MIN][i];
                    }
                    if (thePosition[i] >= _myVBox[asl::Box3i::MAX][i]) {
                        myClippedPosition[i] = _myVBox[asl::Box3i::MAX][i]-1;
                    }
                }
                return at(myClippedPosition);
            }

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
                    throw MarchingCubesException(std::string("Illegal CubeIndex: ") + as_string(theCubeCorner), PLUS_FILE_LINE);
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
                _myOffsetTable[0] = asl::Vector2i(0,0);  // [0,0,0]
                _myOffsetTable[1] = asl::Vector2i(_myLineStride * _myDownSampleRate, 0);  // [0,1,0]
                _myOffsetTable[2] = asl::Vector2i((1+_myLineStride) * _myDownSampleRate, 0);  // [1,1,0]
                _myOffsetTable[3] = asl::Vector2i(_myDownSampleRate, 0); // [1,0,0]
                _myOffsetTable[4] = asl::Vector2i(0, _myDownSampleRate);  // [0,0,0]
                _myOffsetTable[5] = asl::Vector2i(_myLineStride * _myDownSampleRate, _myDownSampleRate);  // [0,1,0]
                _myOffsetTable[6] = asl::Vector2i((1+_myLineStride) * _myDownSampleRate, _myDownSampleRate);  // [1,1,0]
                _myOffsetTable[7] = asl::Vector2i(_myDownSampleRate, _myDownSampleRate); // [1,0,0]
                _myBitMaskTable[0] = BIT_0;
                _myBitMaskTable[1] = BIT_1;
                _myBitMaskTable[2] = BIT_2;
                _myBitMaskTable[3] = BIT_3;
                _myBitMaskTable[4] = BIT_4;
                _myBitMaskTable[5] = BIT_5;
                _myBitMaskTable[6] = BIT_6;
                _myBitMaskTable[7] = BIT_7;
            }

            inline int 
            getCubeIndex(int theI, int theJ, int theK) const {
                unsigned int myBitMask = 0;
                if(isOutside(theI, theJ, theK)) {
                    myBitMask |= BIT_0;
                }
                if(isOutside(theI, theJ+1, theK)) {
                    myBitMask |= BIT_1;
                }
                if(isOutside(theI+1, theJ+1, theK)) {
                    myBitMask |= BIT_2;
                }
                if(isOutside(theI+1, theJ, theK)) {
                    myBitMask |= BIT_3;
                }
                if(isOutside(theI, theJ, theK+1)) {
                    myBitMask |= BIT_4;
                }
                if(isOutside(theI, theJ+1, theK+1)) {
                    myBitMask |= BIT_5;
                }
                if(isOutside(theI+1, theJ+1, theK+1)) {
                    myBitMask |= BIT_6;
                }
                if(isOutside(theI+1, theJ, theK+1)) {
                    myBitMask |= BIT_7;
                }
#if 0
                asl::Vector2i myNeighbour;
                VoxelT myValue;
                int myDownSampledSlice = theK * _myDownSampleRate;
                int myDownSampledPosition = (theJ * _myLineStride + theI) * _myDownSampleRate;
                int mySlicePosition;
                int mySliceOffset;
                for (int i = 0; i < 8; ++i) {
                    mySlicePosition = myDownSampledSlice+_myOffsetTable[i][1];
                    mySliceOffset = myDownSampledPosition+_myOffsetTable[i][0];
                    if (! _myStencils.empty()) {
                        if (bool(_myStencils[mySlicePosition][mySliceOffset]) == false) {
                            return -1;
                        }
                    }
                    myValue = _mySlices[mySlicePosition][mySliceOffset]; 
                    if (myValue < _myThreshold[0] || myValue > _myThreshold[1]) {
                        myBitMask |= _myBitMaskTable[i];
                    }
                }
#endif
                return myBitMask;
            }


            //====================================================================
            // Private Members
            //====================================================================

            CTScan * _myVoxelData;
            asl::Vector3i _myDimensions;
            asl::Vector3f _myVoxelSize;
            asl::Box3i _myVBox;
            asl::Vector3i _myDimBox;
            // asl::Vector2<VoxelT> _myThreshold;

            IJCachePtr   _myCache[2];
            KCachePtr    _myKCache;
            std::vector<VoxelT> _myCurrent;
            std::vector<asl::Vector2<VoxelT> > _myCurrentThresholds;
            int  _myMarchSegment[numSegments];
            MCLookup _myMCLookup;

            const int _myDownSampleRate;
            int _myLineStride;
            int _mySliceOffset;

            OutputPolicy & _myOutputPolicy;

            std::vector<const VoxelT*>   _mySlices;
            std::vector<const unsigned char*> _myStencils;
            asl::Vector2i                _myOffsetTable[8];
            unsigned                     _myBitMaskTable[8];
    };

} // end of namespace y60

#endif // Y60_MARCHING_CUBES_INCLUDED
