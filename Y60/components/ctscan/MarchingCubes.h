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
//
//    $RCSfile: MarchingCubes.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
//=============================================================================

#ifndef Y60_MARCHING_CUBES_INCLUDED
#define Y60_MARCHING_CUBES_INCLUDED

#include "MCLookup.h"
#include "CTScan.h"

#include <y60/Scene.h>
#include <y60/ShapeBuilder.h>
#include <y60/ElementBuilder.h>
#include <asl/Box.h>
#include <asl/Logger.h>

#include <algorithm>
#include <stdio.h>
#include <asl/Assure.h>
#include "VertexCache.h"

//#define MC_DONT_SHARE_VERTICES

//#define DEBUG_INDEXTABLES
//#define DUPECHECKER

typedef unsigned char VmDataValue;

namespace y60 {

    DEFINE_EXCEPTION(MarchingCubesException, asl::Exception);

    const int numSegments = 16;
    const int MAX_VOXEL_SIZE = 64;

    template <class VoxelT>
    class MarchingCubes {
        public:
            typedef std::pair<int, int> EdgeId;
            typedef std::map<EdgeId, int> EdgeCache;

            MarchingCubes(int theDownSampleRate, SceneBuilderPtr theSceneBuilder, CTScan * theVoxelData) :
                _myVertexNormalFlag(false),
                _myVertexColorFlag(true),
                _myInvertNormalsFlag(false),
                _myHalfEdgeFlag(true),
                _mySceneBuilder(theSceneBuilder),
                _myVoxelData(theVoxelData),
                _myVertexNode(0),
                _myHalfEdgeNode(0),
                _myNormalNode(0),
                _myVertices(0),
                _myIndices(0),
                _myHalfEdges(0),
                _myColorIndexNode(0),
                _myNormals(0),
				_myVertexCount(0),
				_myHalfEdgeCount(0),
                _myDownSampleRate(theDownSampleRate),
                _myLineStride(0),
                _myMCLookup()                
            {
                if ( ! _myVoxelData) {
                    throw MarchingCubesException("CTScan ptr is zero.", PLUS_FILE_LINE);
                }

                _myLineStride = theVoxelData->getVoxelDimensions()[0];
                
                _myDimensions[0] = theVoxelData->getVoxelDimensions()[0] / _myDownSampleRate;
                _myDimensions[1] = theVoxelData->getVoxelDimensions()[1] / _myDownSampleRate;
                _myDimensions[2] = theVoxelData->getVoxelDimensions()[2] / _myDownSampleRate;
                
                _myVoxelSize = theVoxelData->getVoxelAspect()*_myDownSampleRate;

                _myVBox.makeEmpty();
                _myVBox.extendBy(asl::Vector3i(0, 0, 0));
                _myVBox.extendBy(asl::Vector3i(_myDimensions[0] - 1,
                                               _myDimensions[1] - 1,
                                               _myDimensions[2] - 1));

                for (int i=0; i < theVoxelData->getVoxelDimensions()[2]; ++i) {
                    _mySlices.push_back(theVoxelData->CTScan::getSlicePtr<VoxelT>(i));
                }
                const SegmentationBitmap & myStencils = theVoxelData->getStencil();
                for (int i = 0; i < myStencils.size(); ++i) {
                    _myStencils.push_back(myStencils[i]->pixels().begin());
                }
                marchAllSegments();
            }
            ~MarchingCubes() {}

            void setThreshold(asl::Vector2<VoxelT> theThreshold) {
                _myThreshold = theThreshold;
            }

            const asl::Vector2<VoxelT> & getThreshold() const {
                return _myThreshold;
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

            // 1: calc vertex normals using gradients, 0: no normals (flat shading)
            void calcNormals(bool theFlag) { _myVertexNormalFlag = theFlag; }
            void invertNormal(bool theFlag) { _myInvertNormalsFlag = theFlag; }
            void setColors(bool theFlag) { _myVertexColorFlag = theFlag; }
            void genHalfEdges(bool theFlag) { _myHalfEdgeFlag = theFlag; }

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
            const asl::Vector3f & getVoxelSize() const {
                return _myVoxelSize;
            }            
            void estimate(unsigned int * theVertexCount=0, unsigned int * theTriangleCount=0) {
                _myHalfEdgeCount = 0;
				_myVertexCount = 0;
                _myUsedSegs = 0;
				// to dry run to count the trangles and vertices
                AC_INFO << "starting dry run";
				march(true);
                if (theVertexCount) {
                    *theVertexCount = _myVertexCount;
                }
                if (theTriangleCount) {
                    *theTriangleCount = _myHalfEdgeCount/3;
                }
            }
            void reserveBuffers(unsigned int theVertexCount, unsigned int theTriangleCount) {
                _myVertexCount = theVertexCount;
                _myHalfEdgeCount = theTriangleCount*3;
            }

            dom::NodePtr apply(const std::string & theShapeName, const std::string & theMaterialId) {
                dom::NodePtr myShapeNode = prepareShapeBuilder(theShapeName, theMaterialId);
                AC_INFO << "starting real march";
                march(false);
                _myHalfEdgeCache.clear();                
                if(_myVertexColorFlag) {
                    dom::Node::WritableValue<VectorOfUnsignedInt> myColorIndexLock(_myColorIndexNode);
                    VectorOfUnsignedInt & myColorIndex = myColorIndexLock.get();
                    myColorIndex.resize(_myHalfEdgeCount, 0);
                }
                if(_myVertexNormalFlag) {
                    _myElementBuilder->copyIndexBin(POSITIONS, NORMALS, NORMAL_ROLE);
                }
                unlockDataBins();
                AC_INFO << "done";
                return myShapeNode;
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
            dom::NodePtr prepareShapeBuilder(const std::string & theShapeName,
                                             const std::string & theMaterialId) 
            {
                _myShapeBuilder = ShapeBuilderPtr( new ShapeBuilder(theShapeName) );
                _mySceneBuilder->appendShape( * _myShapeBuilder );

                _myElementBuilder = ElementBuilderPtr(
                        new ElementBuilder(PRIMITIVE_TYPE_TRIANGLES, theMaterialId) );
                
                _myShapeBuilder->appendElements( * _myElementBuilder );

                // UH: the ShapeBuilder:: prefix is necessary for gcc to work
                _myVertexNode = _myShapeBuilder->ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE);
                _myVertices = _myVertexNode->dom::Node::nodeValuePtrOpen<std::vector<asl::Vector3f> >();
                _myIndexNode = _myElementBuilder->createIndex(POSITION_ROLE, POSITIONS);
                _myIndices = _myIndexNode->dom::Node::nodeValuePtrOpen<std::vector<unsigned int> >();
                _myHalfEdgeNode = _myElementBuilder->createHalfEdges();
                _myHalfEdges = _myHalfEdgeNode->dom::Node::nodeValuePtrOpen<std::vector<signed int> >();
                if (_myVertexColorFlag) {
                    _myColorIndexNode = _myElementBuilder->createIndex(COLOR_ROLE, COLORS);
                }
                AC_INFO << "reserving Vertex  :" << _myVertexCount;
                AC_INFO << "reserving HalfEdge:" << _myHalfEdgeCount;
				_myVertices->reserve(_myVertexCount);
                _myHalfEdges->reserve(_myHalfEdgeCount);
                _myIndices->reserve(_myHalfEdgeCount);

                if (_myVertexNormalFlag) {
                    _myNormalNode = _myShapeBuilder->ShapeBuilder::createVertexDataBin<asl::Vector3f>(NORMAL_ROLE);
                    _myNormals = _myNormalNode->dom::Node::nodeValuePtrOpen<std::vector<asl::Vector3f> >();
					_myNormals->reserve(_myVertexCount);
                } else {
                    _myNormals = 0;
                }

                if (_myVertexColorFlag) {
                    dom::NodePtr myColorNode = _myShapeBuilder->ShapeBuilder::createVertexDataBin<asl::Vector4f>(COLOR_ROLE);
                    std::vector<asl::Vector4f> * myColors = myColorNode->dom::Node::nodeValuePtrOpen<std::vector<asl::Vector4f> >();
                    myColors->reserve(6);                    
                    myColors->push_back(asl::Vector4f(189.0f/255.0f,191.0f/255.0f,136.0f/255.0f,1.0f));
                    myColors->push_back(asl::Vector4f(1,0,0,1));
                    myColors->push_back(asl::Vector4f(0,1,0,1));
                    myColors->push_back(asl::Vector4f(0,0,1,1));
                    myColors->push_back(asl::Vector4f(0,1,1,1));
                    myColors->push_back(asl::Vector4f(1,1,0,1));
                    myColorNode->dom::Node::nodeValuePtrClose<std::vector<asl::Vector4f> >();
                    myColorNode = dom::NodePtr(0);
                }

                return _myShapeBuilder->getNode();
            }

            //inline bool
            //isInInterval(const VoxelT & theValue, const asl::Vector2<VoxelT> & theInterval) {
            //    return (theValue > theInterval[0]) && (theValue < theInterval[1]);
            //}

            int 
            outputVertex(int n, const asl::Vector3i & theMarchPos) {
                asl::Point3f myVertexPosition;
                asl::Vector3f myVertexNormal;

                int iMarch = theMarchPos[0];
                int jMarch = theMarchPos[1];
                int kMarch = theMarchPos[2];
                float li = 0.5f;
                asl::Vector3f g0, g1, g2, g3, g4, g5, g6, g7;
                int myThresholdIndex = 0;
                switch(n) {
                    case 0:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[0], _myCurrent[1]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[0]) / (float)(_myCurrent[1] - _myCurrent[0]);
                        myVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        myVertexPosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];
                        if (_myVertexNormalFlag){
                            calcGradient(float(_myCurrent[3]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch)), float(_myCurrent[1]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch)), float(_myCurrent[4]) - clampedAt(asl::Vector3i(iMarch, jMarch, kMarch-1)), g0);
                            calcGradient(float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch))) - _myCurrent[0], float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch-1)), g1);
                            myVertexNormal = -1.0 * (g0 + li * (g1 - g0));
                        }
                        break;

                    case 1:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[1], _myCurrent[2]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[1]) / (float)(_myCurrent[2] - _myCurrent[1]);
                        myVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if (_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch))) - _myCurrent[0], float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch-1)), g1);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch))) - _myCurrent[1], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch))) - _myCurrent[3], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch-1)), g2);
                            myVertexNormal = -1.0 * (g1 + li * (g2 - g1));
                        }
                        break;

                    case 2:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[3], _myCurrent[2]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[3]) / (float)(_myCurrent[2] - _myCurrent[3]);
                        myVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        myVertexPosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(_myVertexNormalFlag){
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch))) - _myCurrent[1], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch))) - _myCurrent[3], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch-1)), g2);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch))) - _myCurrent[0], float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch)), float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch-1)), g3);
                            myVertexNormal = -1.0 * (g3 + li * (g2 - g3));
                        }
                        break;

                    case 3:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[0], _myCurrent[3]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[0]) / (float)(_myCurrent[3] - _myCurrent[0]);
                        myVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[3]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch)), float(_myCurrent[1]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch)), float(_myCurrent[4]) - clampedAt(asl::Vector3i(iMarch, jMarch, kMarch-1)), g0);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch))) - _myCurrent[0], float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch)), float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch-1)), g3);
                            myVertexNormal = -1.0 * (g0 + li * (g3 - g0));
                        }
                        break;

                    case 4:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[4], _myCurrent[5]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[4]) / (float)(_myCurrent[5] - _myCurrent[4]);
                        myVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        myVertexPosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch+1)), float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch, kMarch+2))) - _myCurrent[0], g4);
                            calcGradient(float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch+1))) - _myCurrent[4], float(clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch+2))) - _myCurrent[1], g5);
                            myVertexNormal = -1.0 * (g4 + li * (g5 - g4));
                        }
                        break;

                    case 5:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[5], _myCurrent[6]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[5]) / (float)(_myCurrent[6] - _myCurrent[5]);
                        myVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch+1))) - _myCurrent[4], float(clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch+2))) - _myCurrent[1], g5);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch+1))) - _myCurrent[5], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch+1))) - _myCurrent[7], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch+2))) - _myCurrent[2], g6);
                            myVertexNormal = -1.0 * (g5 + li * (g6 - g5));
                        }
                        break;

                    case 6:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[7], _myCurrent[6]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[7]) / (float)(_myCurrent[6] - _myCurrent[7]);
                        myVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        myVertexPosition[1] = ((float)jMarch+ li) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch+1))) - _myCurrent[5], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch+1))) - float(_myCurrent[7]), float(clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch+2))) - _myCurrent[2], g6);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch+1))) - _myCurrent[4], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch+2))) - _myCurrent[3], g7);
                            myVertexNormal = -1.0 * (g7 + li * (g6 - g7));
                        }
                        break;

                    case 7:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[4], _myCurrent[7]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[4]) / (float)(_myCurrent[7] - _myCurrent[4]);
                        myVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        myVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch+1)), float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch, kMarch+2))) - _myCurrent[0], g4);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch+1))) - _myCurrent[4], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch+2))) - _myCurrent[3], g7);
                            myVertexNormal = -1.0 * (g4 + li * (g7 - g4));
                        }
                        break;

                    case 8:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[0], _myCurrent[4]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[0]) / (float)(_myCurrent[4] - _myCurrent[0]);
                        myVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        myVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[3]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch)), float(_myCurrent[1]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch)), float(_myCurrent[4]) - clampedAt(asl::Vector3i(iMarch, jMarch, kMarch-1)), g0);
                            calcGradient(float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch-1, jMarch, kMarch+1)), float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch, kMarch+2))) - _myCurrent[0], g4);
                            myVertexNormal = -1.0 * (g0 + li * (g4 - g0));
                        }
                        break;

                    case 9:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[1], _myCurrent[5]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[1]) / (float)(_myCurrent[5] - _myCurrent[1]);
                        myVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        myVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch))) - _myCurrent[0], float(_myCurrent[5]) - clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch-1)), g1);
                            calcGradient(float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch-1, jMarch+1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch, jMarch+2, kMarch+1))) - _myCurrent[4], float(clampedAt(asl::Vector3i(iMarch, jMarch+1, kMarch+2))) - _myCurrent[1], g5);
                            myVertexNormal = -1.0 * (g1 + li * (g5 - g1));
                        }
                        break;

                    case 10:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[2], _myCurrent[6]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[2]) / (float)(_myCurrent[6] - _myCurrent[2]);
                        myVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        myVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch))) - _myCurrent[1], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch))) - _myCurrent[3], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch-1)), g2);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch+1, kMarch+1))) - _myCurrent[5], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+2, kMarch+1))) - _myCurrent[7], float(clampedAt(asl::Vector3i(iMarch+1, jMarch+1, kMarch+2))) - _myCurrent[2], g6);
                            myVertexNormal = -1.0 * (g2 + li * (g6 - g2));
                        }

                        break;

                    case 11:
                        myThresholdIndex = findThresholdBoundary(_myCurrent[3], _myCurrent[7]);
                        li = (float)(_myThreshold[myThresholdIndex] - _myCurrent[3]) / (float)(_myCurrent[7] - _myCurrent[3]);
                        myVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        myVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        myVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch))) - _myCurrent[0], float(_myCurrent[2]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch)), float(_myCurrent[7]) - clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch-1)), g3);
                            calcGradient(float(clampedAt(asl::Vector3i(iMarch+2, jMarch, kMarch+1))) - _myCurrent[4], float(_myCurrent[6]) - clampedAt(asl::Vector3i(iMarch+1, jMarch-1, kMarch+1)), float(clampedAt(asl::Vector3i(iMarch+1, jMarch, kMarch+2))) - _myCurrent[3], g7);
                            myVertexNormal = -1.0 * (g3 + li * (g7 - g3));
                        }
                        break;

                    default:
                        break;
                }
                if (myThresholdIndex == 1) {
                    myVertexNormal = -myVertexNormal;
                }

                if (_myInvertNormalsFlag) {
                    myVertexNormal = -myVertexNormal;
                }
                asl::Vector3f myVertex(myVertexPosition[0], - myVertexPosition[1], myVertexPosition[2]);
                _myVertices->push_back(myVertex);
                if (_myVertexNormalFlag) {
                    _myNormals->push_back(normalized(asl::Vector3f(myVertexNormal[0], - myVertexNormal[1], myVertexNormal[2])));
                }
                return _myVertices->size()-1;
            }
			int
			countVertex(int n, const asl::Vector3i & theMarchPos) {
				return ++_myVertexCount;
			}

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

            inline void triangulateVoxel(int theCubeIndex, const asl::Vector3i & theMarchPos, bool theDryRun, IJCache & theLowerCache, IJCache & theUpperCache, KCache & theKCache) {
                std::vector<int> myEdgeTable;
                myEdgeTable.resize(12, -1);
                myEdgeTable[1] = -2;
                int iMarch = theMarchPos[0];
                int jMarch = theMarchPos[1];
                int kMarch = theMarchPos[2];
                int myPositionIndex;
                int (MarchingCubes<VoxelT>::*myOnVertex)(int n, const asl::Vector3i & theMarchPos) = 0;
                const MCLookup::CubeCase & myCubeCase = _myMCLookup.cubeCases[theCubeIndex]; 

				if (theDryRun) { 
					myOnVertex = &MarchingCubes<VoxelT>::countVertex;
				} else {
					myOnVertex = &MarchingCubes<VoxelT>::outputVertex;
				}
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
                        myPositionIndex = (this->*myOnVertex) (myEdges[i], theMarchPos);
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

				if (theDryRun) {
                    int myFaceCount = myCubeCase.faces.size();
					for (int i = 0; i < myFaceCount; ++i) {
						int myCornerIndex = myCubeCase.faces[i];
						++_myHalfEdgeCount;
					}
				} else {
					int myFirstFaceIndex = _myIndices->size();
					for (int i = 0; i < myCubeCase.faces.size(); ++i) {
						int myCornerIndex = myCubeCase.faces[i];
						int myNextCornerIndex = myCubeCase.faces[i - (i % 3) + ((i+1) % 3)];
						int myIndex = myEdgeTable[myCornerIndex];
                        if (_myHalfEdgeFlag) {
						    int myNextIndex = myEdgeTable[myNextCornerIndex];
                            const MCLookup::HalfEdgeNeighbor & myNeighbor = myCubeCase.neighbors[i];
						    int myHalfEdge = -1;
						    if (myNeighbor.type == MCLookup::INTERNAL) {
							    myHalfEdge = myNeighbor.internal_index + myFirstFaceIndex;
						    } else {
							    // external twin
							    if (myNeighbor.type == MCLookup::MAX_X ||
                                    myNeighbor.type == MCLookup::MAX_Y ||
                                    myNeighbor.type == MCLookup::MAX_Z)                             {
                                        // XXX
                                    if (1
                                        /*((myNeighbor.type == MCLookup::MAX_X) && i != iboxend) ||
                                        ((myNeighbor.type == MCLookup::MAX_Y) && j != jboxend) ||
                                        ((myNeighbor.type == MCLookup::MAX_Z) && k != kboxend)*/
                                        ) 
                                    { 
                                        // Add to the map
                                        EdgeId myKey(myIndex, myNextIndex);
                                        int myValue = _myHalfEdges->size();
                                        EdgeCache::value_type myItem(myKey, myValue);
                                        _myHalfEdgeCache.insert(myItem);
                                        AC_TRACE << "Inserting: (" << myKey.first << ", " << myKey.second << ")";
                                    }
							    } else {
								    // Remove from map
								    EdgeId myKey(myNextIndex, myIndex);
								    EdgeCache::iterator iter = _myHalfEdgeCache.find(myKey);
								    if (_myHalfEdgeCache.end() == iter) {
                                        // [TS] Disabled for Segmentation
									    //if (!(kMarch == _myVBox[asl::Box3i::MIN][2]) && !(iMarch == _myVBox[asl::Box3i::MIN][0]) && !(jMarch == _myVBox[asl::Box3i::MIN][1])) {
										   // AC_WARNING << "Not Found in Cache: (" << myKey.first << ", " << myKey.second << ") = (" << myCornerIndex << ", " << myNextCornerIndex << ") jMarch: " << jMarch << ", iMarch: " << iMarch << ", kMarch: " << kMarch;
									    //}
								    } else {
									    myHalfEdge = (*iter).second;
									    _myHalfEdgeCache.erase(iter);
									    AC_TRACE << "Removing: (" << myKey.first << ", " << myKey.second << ")";
									    _myHalfEdges->at(myHalfEdge) = _myHalfEdges->size(); // we will push the other edge soon
								    }
							    }
						    }
						    AC_TRACE << "added HalfEdge " << myHalfEdge << " as element#" << _myHalfEdges->size();
						    _myHalfEdges->push_back(myHalfEdge); // push the other edge
						    AC_TRACE << "   size is now " << _myHalfEdges->size();
                        }
						_myIndices->push_back(myIndex);
                        AC_TRACE << myIndex << (*_myVertices)[myIndex]; 
					}
				}
            }

            inline bool
            isOutside(int x, int y, int z) const {
                const VoxelT & myValue = at(x, y, z);
                return myValue < _myThreshold[0] || myValue > _myThreshold[1];
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

            asl::Vector3f gradientAt(int iMarch, int jMarch, int kMarch) {
                asl::Vector3f myResult;

            }

            void calcGradient(float dx, float dy, float dz, asl::Vector3f & g) {
                g[0] = dx / _myVoxelSize[0];
                g[1] = dy / _myVoxelSize[1];
                g[2] = dz / _myVoxelSize[2];

                g.normalize();
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
#ifdef UNROLLED_CUBEINDEX_LOOP
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
#else
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

            void march(bool theDryRun) {
                AC_TRACE << "MarchingCubes::march()";
                int j, i, k;
                int myCubeIndex;
                //int myCubeIndex;
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
                            theDryRun ? "estimating" : "polygonizing");
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
                                triangulateVoxel(myCubeIndex, myVoxel, theDryRun, *_myCache[1-myUpperCacheIndex], *_myCache[myUpperCacheIndex], *_myKCache);
                            }
                        }
                    }
                }

                AC_INFO << _myHalfEdgeCount/3 << " triangles, " 
                        << _myVertexCount << " vertices, "
                        << _myHalfEdgeCache.size() << " halfedges remaining in cache";
                _myHalfEdgeCache.clear();
            }

            void
            unlockDataBins() {
                _myVertexNode->dom::Node::nodeValuePtrClose<std::vector<asl::Vector3f> >();
                _myVertexNode = dom::NodePtr(0);
                _myVertices = 0;

                _myHalfEdgeNode->dom::Node::nodeValuePtrClose<std::vector<signed int> >();
                _myHalfEdgeNode = dom::NodePtr(0);
                _myHalfEdges = 0;

                _myIndexNode->dom::Node::nodeValuePtrClose<std::vector<unsigned int> >();
                _myIndexNode = dom::NodePtr(0);
                _myIndices = 0;

                if (_myVertexNormalFlag) {
                    _myNormalNode->dom::Node::nodeValuePtrClose<std::vector<asl::Vector3f> >();
                    _myNormalNode = dom::NodePtr(0);
                    _myNormals = 0;
                }
            }


            //====================================================================
            // Private Members
            //====================================================================

            CTScan * _myVoxelData;
            asl::Vector3i _myDimensions;
            asl::Box3i _myVBox;
            asl::Vector3i _myDimBox;
            asl::Vector3f _myVoxelSize;
            asl::Vector2<VoxelT> _myThreshold;

            IJCachePtr   _myCache[2];
            KCachePtr    _myKCache;
            std::vector<VoxelT> _myCurrent;
            int  _myMarchSegment[numSegments];
            bool _myVertexNormalFlag;
            bool _myHalfEdgeFlag;
            bool _myInvertNormalsFlag;
            bool _myVertexColorFlag;
            int  _myHalfEdgeCount;
			int  _myVertexCount;
            int  _myUsedSegs;
            MCLookup _myMCLookup;

            const int _myDownSampleRate;
            int _myLineStride;
            int _mySliceOffset;

            SceneBuilderPtr   _mySceneBuilder;
            ShapeBuilderPtr   _myShapeBuilder;
            ElementBuilderPtr _myElementBuilder;

            dom::NodePtr                 _myVertexNode;
            dom::NodePtr                 _myNormalNode;
            dom::NodePtr                 _myIndexNode;
            dom::NodePtr                 _myHalfEdgeNode;
            dom::NodePtr                 _myColorIndexNode;
            std::vector<const VoxelT*>   _mySlices;
            std::vector<const unsigned char*> _myStencils;
            std::vector<asl::Vector3f> * _myVertices;
            std::vector<signed int>    * _myHalfEdges;
            std::vector<asl::Vector3f> * _myNormals;            
            std::vector<unsigned int>  * _myIndices;
            EdgeCache                    _myHalfEdgeCache;
            asl::Vector2i                _myOffsetTable[8];
            unsigned                     _myBitMaskTable[8];
    };

} // end of namespace y60

// clean up our macros

#endif // Y60_MARCHING_CUBES_INCLUDED
