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
                /*
                std::swap( myTempBox[asl::Box3i::MIN][0], myTempBox[asl::Box3i::MIN][1]);
                std::swap( myTempBox[asl::Box3i::MAX][0], myTempBox[asl::Box3i::MAX][1]);
                */
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
                // TODO find a better way to speed up things ....
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
            int 
            outputVertex(int n, int iMarch, int jMarch, int kMarch) {
                asl::Point3f myVertexPosition;
                asl::Vector3f myVertexNormal;
                calcVoxelIntersect(n, iMarch, jMarch, kMarch, myVertexPosition, myVertexNormal);

                asl::Vector3f myVertex(myVertexPosition[0], - myVertexPosition[1], myVertexPosition[2]);
                _myVertices->push_back(myVertex);
                if (_myVertexNormalFlag) {
                    _myNormals->push_back(normalized(asl::Vector3f(myVertexNormal[0], - myVertexNormal[1], myVertexNormal[2])));
                }
                return _myVertices->size()-1;
            }
			int
			countVertex(int n, int iMarch, int jMarch, int kMarch) {
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

            inline void triangulateVoxel(int cubeIndex, int iMarch, int jMarch, int kMarch, bool theDryRun) {
                int edgeTable[12], edge;
                int vertOffset;
                int offset;
                asl::Point3f  myVertexPosition;
                asl::Vector3f myVertexNormal;
				int (MarchingCubes<VoxelT>::*myOnVertex)(int n, int iMarch, int jMarch, int kMarch) = 0;

                const MCLookup::CubeCase & myCubeCase = _myMCLookup.cubeCases[cubeIndex]; 
                const int iBoxStart = _myVBox[asl::Box3i::MIN][0];
                const int iBoxEnd   = _myVBox[asl::Box3i::MAX][0];
                const int jBoxStart = _myVBox[asl::Box3i::MIN][1];
                const int jBoxEnd   = _myVBox[asl::Box3i::MAX][1];
                const int kBoxStart = _myVBox[asl::Box3i::MIN][2];
                const int kBoxEnd   = _myVBox[asl::Box3i::MAX][2];

				if (theDryRun) { 
					myOnVertex = &MarchingCubes<VoxelT>::countVertex;
				} else {
					myOnVertex = &MarchingCubes<VoxelT>::outputVertex;
				}
                AC_TRACE << "triangulateVoxel(" << cubeIndex << ", " << jMarch << "," << iMarch << "," << kMarch << ")";
                const std::vector<int> & myEdges = myCubeCase.edges;
                int myEdgeCount = myEdges.size();
                for (int i = 0; i < myEdgeCount; ++i) {
                    edge = myEdges[i];
                    AC_TRACE << "     edge case :" << edge;
#if 0
                    vertOffset = (this->*myOnVertex)  (edge, iMarch, jMarch, kMarch);
#else
                    switch (edge) {
                        case 0:
                            if(kMarch == kBoxStart) {
                                if (iMarch == iBoxStart) {
                                    vertOffset = (this->*myOnVertex) (0, iMarch, jMarch, kMarch);
                                } else {
                                    vertOffset = _myOld2Pt;
                                }
                            } else {
                                offset = _myDimensions[0] * jMarch + iMarch;
                                vertOffset = _myXEdge[offset];
                            }
                            break;

                        case 1:
                            if (kMarch == kBoxStart) {
                                vertOffset = (this->*myOnVertex)  (1, iMarch, jMarch, kMarch);
                            } else {
                                offset = _myDimensions[0] * (jMarch + 1) + iMarch;
                                vertOffset = _myYEdge[offset];
                            }
                            if (kMarch == kBoxStart) {
                                offset = _myDimensions[0] * (jMarch + 1) + iMarch;
                                _myYEdge[offset] = vertOffset;
                            }
                            break;

                        case 2:
                            if (kMarch == kBoxStart) {
                                vertOffset = (this->*myOnVertex)  (2, iMarch, jMarch, kMarch);
                            } else {
                                offset = _myDimensions[0] * jMarch + iMarch + 1;
                                vertOffset = _myXEdge[offset];
                            }
                            break;

                        case 3:
                            if((kMarch == kBoxStart) && (jMarch == jBoxStart)) { 
                                vertOffset = (this->*myOnVertex)  (3, iMarch, jMarch, kMarch);
                            } else {
                                offset = _myDimensions[0] * jMarch + iMarch;
                                vertOffset = _myYEdge[offset];
                            }
                            break;

                        case 4:
                            if(iMarch == iBoxStart) {
                                vertOffset = (this->*myOnVertex)  (4, iMarch, jMarch, kMarch);
                            } else {
                                vertOffset = _myOld6Pt;
                            }
                            offset = _myDimensions[0] * jMarch + iMarch;
                            _myXEdge[offset] = vertOffset;
                            break;

                        case 5:
                            vertOffset = (this->*myOnVertex)  (5, iMarch, jMarch, kMarch);
                            _myTopYEdge[iMarch] = vertOffset;
                            // Only write to _myYEdge for the last voxel because otherwise it
                            // has already been / will be written by 7. We shouldn't write here
                            // because we want to keep the old value as long as possible
                            if(jMarch == jBoxEnd - 1) {
                                offset = _myDimensions[0] * jMarch + iMarch;
                                _myYEdge[offset] = vertOffset;
                            } 
                            break;

                        case 6:
                            vertOffset = (this->*myOnVertex)  (6, iMarch, jMarch, kMarch);

                            if(iMarch == iBoxEnd - 1) {
                                offset = _myDimensions[0] * jMarch + iBoxEnd;
                                _myXEdge[offset] = vertOffset;
                            }
                            break;

                        case 7:
                            if(jMarch == jBoxStart) {
                                vertOffset = (this->*myOnVertex)  (7, iMarch, jMarch, kMarch);
                            } else {
                                vertOffset = _myTopYEdge[iMarch];
                            }
                            offset = _myDimensions[0] * jMarch + iMarch;
                            _myYEdge[offset] = vertOffset;
                            break;

                        case 8:
                            if(jMarch == jBoxStart) {
                                if (iMarch == iBoxStart) {
                                    vertOffset = (this->*myOnVertex)  (8, iMarch, jMarch, kMarch);
                                } else {
                                    vertOffset = _myOld11Pt;
                                }
                            } else {
                                vertOffset = _myZEdge[iMarch];
                            }
                            break;

                        case 9:
                            if(iMarch == iBoxStart) {
                                vertOffset = (this->*myOnVertex)  (9, iMarch, jMarch, kMarch);
                            } else {
                                vertOffset = _myOld10Pt;
                            }
                            _myZEdge[iMarch] = vertOffset;
                            break;

                        case 10:
                            vertOffset = (this->*myOnVertex)  (10, iMarch, jMarch, kMarch);

                            if(iMarch == iBoxEnd - 1) {
                                _myZEdge[iBoxEnd] = vertOffset;
                            }
                            break;

                        case 11:
                            if(jMarch == jBoxStart) {
                                vertOffset = (this->*myOnVertex)  (11, iMarch, jMarch, kMarch);
                            } else {
                                vertOffset = _myZEdge[iMarch + 1];
                            }
                            break;

                        default:
                            break;
                    }
#endif
                    edgeTable[edge] = vertOffset;
                }
                // update old stuff
                _myOld11Pt = edgeTable[11];
                _myOld2Pt = edgeTable[2];
                _myOld6Pt = edgeTable[6];
                _myOld10Pt = edgeTable[10];
				if (theDryRun) {
                    int myFaceCount = myCubeCase.faces.size();
					for (int i = 0; i < myFaceCount; ++i) {
						int myCornerIndex = myCubeCase.faces[i];
						++_myHalfEdgeCount;
					}
				} else {
					int myFirstFaceIndex = _myHalfEdges->size();
                    // AC_INFO << "-------------";
					for (int i = 0; i < myCubeCase.faces.size(); ++i) {
						int myCornerIndex = myCubeCase.faces[i];
						int myNextCornerIndex = myCubeCase.faces[i - (i % 3) + ((i+1) % 3)];
						int myIndex = edgeTable[myCornerIndex];
						int myNextIndex = edgeTable[myNextCornerIndex];
						// int myTwin = ourHalfEdgeData[cubeIndex][i];
                        const MCLookup::HalfEdgeNeighbor & myNeighbor = myCubeCase.neighbors[i];
						int myHalfEdge = -1;
						if (myNeighbor.type == MCLookup::INTERNAL) {
							myHalfEdge = myNeighbor.internal_index + myFirstFaceIndex;
						} else {
							// external twin
							if (myNeighbor.type == MCLookup::MAX_X ||
                                myNeighbor.type == MCLookup::MAX_Y ||
                                myNeighbor.type == MCLookup::MAX_Z) 
                            {
                                if (((myNeighbor.type == MCLookup::MAX_X) && (iMarch < iBoxEnd-1)) ||
                                    ((myNeighbor.type == MCLookup::MAX_Y) && (jMarch < jBoxEnd-1)) ||
                                    ((myNeighbor.type == MCLookup::MAX_Z) && (kMarch < kBoxEnd-1))) 
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
									if (!(kMarch == kBoxStart) && !(iMarch == jBoxStart) && !(jMarch == iBoxStart)) {
										AC_WARNING << "Not Found in Cache: (" << myKey.first << ", " << myKey.second << ") " << edge << " jMarch: " << jMarch << ", iMarch: " << iMarch << ", kMarch: " << kMarch << " Dumping cache.";
										//throw MarchingCubesException("Not found in cache.", PLUS_FILE_LINE);                                    
									}
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
						_myIndices->push_back(myIndex);
                        AC_TRACE << myIndex << (*_myVertices)[myIndex]; 
					}
				}
            }

            inline const asl::Vector2<VoxelT> &
            getVoxelThreshold(int x, int y, int z) const {
                return _myThreshold;
                //static asl::Vector2<VoxelT> ourOtherThreshold(VoxelT(100), VoxelT(150));
                //if (x > 64) {
                //    return _myThreshold;
                //} else {
                //    return ourOtherThreshold;
                //}
            }

            inline bool
            isOutside(int x, int y, int z) const {
                const VoxelT & myValue = at(x, y, z);
                const asl::Vector2<VoxelT> & myThreshold = getVoxelThreshold(x, y, z);
                return myValue < myThreshold[0] || myValue > myThreshold[1];
            }

            inline int findThresholdBoundary(const VoxelT theFirstValue, const VoxelT theSecondValue) const {
                if ((theFirstValue <= _myThreshold[0] && theSecondValue >= _myThreshold[0]) ||
                    (theFirstValue >= _myThreshold[0] && theSecondValue <= _myThreshold[0])) 
                {
                    return 0;
                } else {
                    //throw MarchingCubesException("Threshold is neither crossed from top or bottom", PLUS_FILE_LINE);
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

            void calcVoxelIntersect(int n, int iMarch, int jMarch, int kMarch, 
                    asl::Point3f & theVertexPosition, asl::Vector3f & theVertexNormal) 
            {
                float li = 0.5f;
                asl::Vector3f g0, g1, g2, g3, g4, g5, g6, g7;
                int myThresholdIndex = 0;
                asl::Vector2<VoxelT> myThreshold = _myThreshold;

                switch(n) {
                    case 0:
                        li = (float)(_myThreshold[0] - _myCurrent[0]) / (float)(_myCurrent[1] - _myCurrent[0]);
                        theVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        theVertexPosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if (_myVertexNormalFlag){
                            calcGradient(float(_myCurrent[3]) - _myJPost0Val, float(_myCurrent[1]) - _myIPost0Val, float(_myCurrent[4]) - _myKPost0Val, g0);
                            calcGradient(float(_myCurrent[2]) - _myJPost1Val, float(_myIPre1Val) - _myCurrent[0], float(_myCurrent[5]) - _myKPost1Val, g1);
                            theVertexNormal = -1.0 * (g0 + li * (g1 - g0));
                        }
                        break;

                    case 1:
                        li = (float)(_myThreshold[0] - _myCurrent[1]) / (float)(_myCurrent[2] - _myCurrent[1]);
                        theVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if (_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[2]) - _myJPost1Val, float(_myIPre1Val) - _myCurrent[0], float(_myCurrent[5]) - _myKPost1Val, g1);
                            calcGradient(float(_myJPre2Val) - _myCurrent[1], float(_myIPre2Val) - _myCurrent[3], float(_myCurrent[6]) - _myKPost2Val, g2);
                            theVertexNormal = -1.0 * (g1 + li * (g2 - g1));
                        }
                        break;

                    case 2:
                        li = (float)(_myThreshold[0] - _myCurrent[3]) / (float)(_myCurrent[2] - _myCurrent[3]);
                        theVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        theVertexPosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(_myVertexNormalFlag){
                            calcGradient(float(_myJPre2Val) - _myCurrent[1], float(_myIPre2Val) - _myCurrent[3], float(_myCurrent[6]) - _myKPost2Val, g2);
                            calcGradient(float(_myJPre3Val) - _myCurrent[0], float(_myCurrent[2]) - _myIPost3Val, float(_myCurrent[7]) - _myKPost3Val, g3);
                            theVertexNormal = -1.0 * (g3 + li * (g2 - g3));
                        }
                        break;

                    case 3:
                        li = (float)(_myThreshold[0] - _myCurrent[0]) / (float)(_myCurrent[3] - _myCurrent[0]);
                        theVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[3]) - _myJPost0Val, float(_myCurrent[1]) - _myIPost0Val, float(_myCurrent[4]) - _myKPost0Val, g0);
                            calcGradient(float(_myJPre3Val) - _myCurrent[0], float(_myCurrent[2]) - _myIPost3Val, float(_myCurrent[7]) - _myKPost3Val, g3);
                            theVertexNormal = -1.0 * (g0 + li * (g3 - g0));
                        }
                        break;

                    case 4:
                        li = (float)(_myThreshold[0] - _myCurrent[4]) / (float)(_myCurrent[5] - _myCurrent[4]);
                        theVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        theVertexPosition[1] = ((float)jMarch + li) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[7]) - _myJPost4Val, float(_myCurrent[5]) - _myIPost4Val, float(_myKPre4Val) - _myCurrent[0], g4);
                            calcGradient(float(_myCurrent[6]) - _myJPost5Val, float(_myIPre5Val) - _myCurrent[4], float(_myKPre5Val) - _myCurrent[1], g5);
                            theVertexNormal = -1.0 * (g4 + li * (g5 - g4));
                        }
                        break;

                    case 5:
                        li = (float)(_myThreshold[0] - _myCurrent[5]) / (float)(_myCurrent[6] - _myCurrent[5]);
                        theVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[6]) - _myJPost5Val, float(_myIPre5Val) - _myCurrent[4], float(_myKPre5Val) - _myCurrent[1], g5);
                            calcGradient(float(_myJPre6Val) - _myCurrent[5], float(_myIPre6Val) - _myCurrent[7], float(_myKPre6Val) - _myCurrent[2], g6);
                            theVertexNormal = -1.0 * (g5 + li * (g6 - g5));
                        }
                        break;

                    case 6:
                        li = (float)(_myThreshold[0] - _myCurrent[7]) / (float)(_myCurrent[6] - _myCurrent[7]);
                        theVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        theVertexPosition[1] = ((float)jMarch+ li) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myJPre6Val) - _myCurrent[5], float(_myIPre6Val) - float(_myCurrent[7]), float(_myKPre6Val) - _myCurrent[2], g6);
                            calcGradient(float(_myJPre7Val) - _myCurrent[4], float(_myCurrent[6]) - _myIPost7Val, float(_myKPre7Val) - _myCurrent[3], g7);
                            theVertexNormal = -1.0 * (g7 + li * (g6 - g7));
                        }
                        break;

                    case 7:
                        li = (float)(_myThreshold[0] - _myCurrent[4]) / (float)(_myCurrent[7] - _myCurrent[4]);
                        theVertexPosition[0] = ((float)iMarch + li) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        theVertexPosition[2] = (float)(kMarch + 1) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[7]) - _myJPost4Val, float(_myCurrent[5]) - _myIPost4Val, float(_myKPre4Val) - _myCurrent[0], g4);
                            calcGradient(float(_myJPre7Val) - _myCurrent[4], float(_myCurrent[6]) - _myIPost7Val, float(_myKPre7Val) - _myCurrent[3], g7);
                            theVertexNormal = -1.0 * (g4 + li * (g7 - g4));
                        }
                        break;

                    case 8:
                        li = (float)(_myThreshold[0] - _myCurrent[0]) / (float)(_myCurrent[4] - _myCurrent[0]);
                        theVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        theVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[3]) - _myJPost0Val, float(_myCurrent[1]) - _myIPost0Val, float(_myCurrent[4]) - _myKPost0Val, g0);
                            calcGradient(float(_myCurrent[7]) - _myJPost4Val, float(_myCurrent[5]) - _myIPost4Val, float(_myKPre4Val) - _myCurrent[0], g4);
                            theVertexNormal = -1.0 * (g0 + li * (g4 - g0));
                        }
                        break;

                    case 9:
                        li = (float)(_myThreshold[0] - _myCurrent[1]) / (float)(_myCurrent[5] - _myCurrent[1]);
                        theVertexPosition[0] = (float)(iMarch) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        theVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myCurrent[2]) - _myJPost1Val, float(_myIPre1Val) - _myCurrent[0], float(_myCurrent[5]) - _myKPost1Val, g1);
                            calcGradient(float(_myCurrent[6]) - _myJPost5Val, float(_myIPre5Val) - _myCurrent[4], float(_myKPre5Val) - _myCurrent[1], g5);
                            theVertexNormal = -1.0 * (g1 + li * (g5 - g1));
                        }
                        break;

                    case 10:
                        li = (float)(_myThreshold[0] - _myCurrent[2]) / (float)(_myCurrent[6] - _myCurrent[2]);
                        theVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch + 1) * _myVoxelSize[1];
                        theVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myJPre2Val) - _myCurrent[1], float(_myIPre2Val) - _myCurrent[3], float(_myCurrent[6]) - _myKPost2Val, g2);
                            calcGradient(float(_myJPre6Val) - _myCurrent[5], float(_myIPre6Val) - _myCurrent[7], float(_myKPre6Val) - _myCurrent[2], g6);
                            theVertexNormal = -1.0 * (g2 + li * (g6 - g2));
                        }

                        break;

                    case 11:
                        li = (float)(_myThreshold[0] - _myCurrent[3]) / (float)(_myCurrent[7] - _myCurrent[3]);
                        theVertexPosition[0] = (float)(iMarch + 1) * _myVoxelSize[0];
                        theVertexPosition[1] = (float)(jMarch) * _myVoxelSize[1];
                        theVertexPosition[2] = ((float)kMarch + li) * _myVoxelSize[2];

                        if(_myVertexNormalFlag) {
                            calcGradient(float(_myJPre3Val) - _myCurrent[0], float(_myCurrent[2]) - _myIPost3Val, float(_myCurrent[7]) - _myKPost3Val, g3);
                            calcGradient(float(_myJPre7Val) - _myCurrent[4], float(_myCurrent[6]) - _myIPost7Val, float(_myKPre7Val) - _myCurrent[3], g7);
                            theVertexNormal = -1.0 * (g3 + li * (g7 - g3));
                        }
                        break;

                    default:
                        break;
                }
                if (myThresholdIndex == 1) {
                    theVertexNormal = -theVertexNormal;
                }

                if (_myInvertNormalsFlag) {
                    theVertexNormal = -theVertexNormal;
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
             * @warn x is j, y is i and z is k
             */
            inline
            const VoxelT &
            at(int x, int y, int z) const {
                return _mySlices[z*_myDownSampleRate][_myLineStride*y*_myDownSampleRate + x*_myDownSampleRate];
            }

            inline
            const VoxelT &
            clippedAt(const asl::Vector3i & thePosition) const {
                asl::Vector3i myClippedPosition = thePosition;
                for (int i = 0; i < 3; ++i) {
                    if (thePosition[i] < 0) {
                        myClippedPosition[i] = 0;
                    }
                    if (thePosition[i] >= _myVBox[asl::Box3i::MAX][i]) {
                        myClippedPosition[i] = _myVBox[asl::Box3i::MAX][i]-1;
                    }
                }
                return at(myClippedPosition[1], myClippedPosition[0], myClippedPosition[2]);
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

            inline VoxelT
            getThresholdByCubeCorner(int iMarch, int jMarch, int kMarch, int theCubeCorner) {
                switch (theCubeCorner) {
                case 0:
                    return getVoxelThreshold(iMarch, _jMarch, kMarch);
                case 1:
                    return getVoxelThreshold(iMarch, _jMarch+1, kMarch);
                case 2:
                    return getVoxelThreshold(iMarch+1, _jMarch+1, kMarch);
                case 3:
                    return getVoxelThreshold(iMarch+1, _jMarch, kMarch);
                case 4:
                    return getVoxelThreshold(iMarch, _jMarch, kMarch+1);
                case 5:
                    return getVoxelThreshold(iMarch, _jMarch+1, kMarch+1);
                case 6:
                    return getVoxelThreshold(iMarch+1, _jMarch+1, kMarch+1);
                case 7:
                    return getVoxelThreshold(iMarch+1, _jMarch, kMarch+1);
                default:
                    throw MarchingCubesException(std::string("Illegal CubeIndex: ") + as_string(theCubeCorner), PLUS_FILE_LINE);
                }
            }


            inline void
            fillVoxelCube(int iMarch, int jMarch, int kMarch, std::vector<VoxelT> & theVoxelCube) {
                theVoxelCube.clear();
                theVoxelCube.reserve(8);
                for (int i = 0; i < 8; ++i) {
                    theVoxelCube.push_back(getValueByCubeCorner(iMarch, jMarch, kMarch, i));
                }
            }

            void march(bool theDryRun) {
                AC_TRACE << "MarchingCubes::march()";
                int j, i, k;
                int cubeIndex;
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

                _myXEdge.resize(_myDimensions[0] * _myDimensions[1], -1);
                _myYEdge.resize(_myDimensions[0] * _myDimensions[1], -1);
                _myTopYEdge.resize(_myDimensions[0], -1);
                _myZEdge.resize(_myDimensions[0], -1);

                int postSlice, lowSlice, highSlice, preSlice;
                // asl::Ptr<VoxelSampler<VoxelT> > postSlice, lowSlice, highSlice, preSlice;
                if (kBoxStart > 0) {
                    postSlice = kBoxStart - 1;
                }
                lowSlice = kBoxStart;
                if(kBoxStart == 0) {
                    postSlice = lowSlice;
                }
                highSlice = kBoxStart+1;
                preSlice = kBoxStart+2;

                for(k = kBoxStart; k < kBoxEnd; k++) {
                    _myVoxelData->notifyProgress(double(k - kBoxStart) / double(kBoxEnd - kBoxStart),
                            theDryRun ? "estimating" : "polygonizing");

                    for(j = jBoxStart; j < jBoxEnd; j++) {
                        _myOld2Pt = _myOld6Pt = _myOld11Pt = _myOld10Pt = -1;
                        for(i = iBoxStart; i < iBoxEnd; i++) {

                            cubeIndex = 0;
                            if(isOutside(i, j, lowSlice)) {
                                cubeIndex |= BIT_0;
                            }
                            if(isOutside(i, j+1, lowSlice)) {
                                cubeIndex |= BIT_1;
                            }
                            if(isOutside(i+1, j+1, lowSlice)) {
                                cubeIndex |= BIT_2;
                            }
                            if(isOutside(i+1, j, lowSlice)) {
                                cubeIndex |= BIT_3;
                            }
                            if(isOutside(i, j, highSlice)) {
                                cubeIndex |= BIT_4;
                            }
                            if(isOutside(i, j+1, highSlice)) {
                                cubeIndex |= BIT_5;
                            }
                            if(isOutside(i+1, j+1, highSlice)) {
                                cubeIndex |= BIT_6;
                            }
                            if(isOutside(i+1, j, highSlice)) {
                                cubeIndex |= BIT_7;
                            }

                            if((cubeIndex > 0) && (cubeIndex < 255)) {
                                fillVoxelCube(i, j, k, _myCurrent);                                
                                _myIPost0Val = j > jBoxStart ? 
                                    at(i,j-1,lowSlice) : _myCurrent[0];
                                _myJPost0Val = i > iBoxStart ?
                                    at(i-1,j,lowSlice) : _myCurrent[0];
                                _myKPost0Val = at(i,j,postSlice);

                                _myIPost3Val = j > jBoxStart ? 
                                    at(i+1,j-1,lowSlice) : _myCurrent[3];
                                _myJPre3Val = i < iBoxEnd - 1 ? 
                                    at(i+2,j,lowSlice) : _myCurrent[3];
                                _myKPost3Val = at(i+1,j,postSlice);

                                _myIPre1Val = j < jBoxEnd - 1 ? 
                                    at(i,j+2,lowSlice) : _myCurrent[1];
                                _myJPost1Val = i > iBoxStart ? 
                                    at(i-1,j+1,lowSlice) : _myCurrent[1];
                                _myKPost1Val = at(i,j+1,postSlice);
                                
                                _myIPost4Val = j > jBoxStart ? 
                                    at(i,j-1,highSlice) : _myCurrent[4];
                                _myJPost4Val = i > iBoxStart ? 
                                    at(i-1,j,highSlice) : _myCurrent[4];
                                _myKPre4Val = at(i,j,preSlice);

                                _myJPre6Val  = i < iBoxEnd - 1 ? 
                                    at(i+2,j+1,highSlice) : _myCurrent[6];
                                _myIPre6Val  = j < jBoxEnd - 1 ? 
                                    at(i+1,j+2,highSlice) : _myCurrent[6];
                                _myKPre6Val = at(i+1,j+1,preSlice);

                                _myJPost5Val = i > iBoxStart ? 
                                    at(i-1,j+1,highSlice) : _myCurrent[5];
                                _myIPre5Val  = j < jBoxEnd - 1 ? 
                                    at(i,j+2,highSlice) : _myCurrent[5];
                                _myKPre5Val = at(i,j+1,preSlice);

                                _myIPost7Val = j > jBoxStart ? 
                                    at(i+1,j-1,highSlice) : _myCurrent[7];
                                _myJPre7Val  = i < iBoxEnd - 1 ? 
                                    at(i+2,j,highSlice) : _myCurrent[7];
                                _myKPre7Val = at(i+1,j,preSlice);

                                _myIPre2Val  = j < jBoxEnd - 1 ? 
                                    at(i+1,j+2,lowSlice) : _myCurrent[2];
                                _myJPre2Val  = i < iBoxEnd - 1 ? 
                                    at(i+2,j+1,lowSlice) : _myCurrent[2];
                                _myKPost2Val = at(i+1,j+1,postSlice);
                                triangulateVoxel(cubeIndex, i, j, k, theDryRun);
                            }
                        }
                    }
                    postSlice = lowSlice;
                    lowSlice = highSlice;
                    highSlice = preSlice;

                    if(k < kBoxEnd - 2) {
                        preSlice = k+3;
                    } else if (k < kBoxEnd - 1) {
                        preSlice = k+2;
                    } else if (k < kBoxEnd) {
                        preSlice = k+1;
                    }
                }

                AC_INFO << _myHalfEdgeCount/3 << " triangles, " 
                        << _myVertexCount << " vertices, "
                        << _myHalfEdgeCache.size() << " halfedges remaining in cache";
                _myHalfEdgeCache.clear();
                _myXEdge.resize(0);
                _myYEdge.resize(0);
                _myTopYEdge.resize(0);
                _myZEdge.resize(0);
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

            // TODO change to std::vector ...
            std::vector<int> _myXEdge;
            std::vector<int> _myYEdge;
            std::vector<int> _myTopYEdge;
            std::vector<int> _myZEdge;
            int   _myOld6Pt, _myOld10Pt, _myOld11Pt, _myOld2Pt; // old vertex indices in j order
            std::vector<VoxelT> _myCurrent;
            VoxelT _myIPost0Val, _myJPost0Val, _myKPost0Val, _myIPre1Val, _myJPost1Val, _myKPost1Val;
            VoxelT _myIPre2Val, _myJPre2Val, _myKPost2Val, _myIPost3Val, _myJPre3Val, _myKPost3Val;
            VoxelT _myIPost4Val, _myJPost4Val, _myKPre4Val, _myIPre5Val, _myJPost5Val,_myKPre5Val;
            VoxelT _myIPre6Val, _myJPre6Val, _myKPre6Val, _myIPost7Val, _myJPre7Val, _myKPre7Val;
            int  _myMarchSegment[numSegments];
            bool _myVertexNormalFlag;
            bool _myInvertNormalsFlag;
            bool _myVertexColorFlag;
            int  _myHalfEdgeCount;
			int  _myVertexCount;
            int  _myUsedSegs;
            MCLookup _myMCLookup;

            const int _myDownSampleRate;
            int _myLineStride;

            SceneBuilderPtr   _mySceneBuilder;
            ShapeBuilderPtr   _myShapeBuilder;
            ElementBuilderPtr _myElementBuilder;

            dom::NodePtr                 _myVertexNode;
            dom::NodePtr                 _myNormalNode;
            dom::NodePtr                 _myIndexNode;
            dom::NodePtr                 _myHalfEdgeNode;
            dom::NodePtr                 _myColorIndexNode;
            std::vector<const VoxelT*>   _mySlices;
            std::vector<asl::Vector3f> * _myVertices;
            std::vector<signed int>    * _myHalfEdges;
            std::vector<asl::Vector3f> * _myNormals;            
            std::vector<unsigned int>  * _myIndices;            
            EdgeCache                    _myHalfEdgeCache;
    };

} // end of namespace y60

// clean up our macros

#endif // Y60_MARCHING_CUBES_INCLUDED
