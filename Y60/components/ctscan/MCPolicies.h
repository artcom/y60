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

#ifndef Y60_CTSCAN_MCPOLICIES_INCLUDED
#define Y60_CTSCAN_MCPOLICIES_INCLUDED

#include <y60/Scene.h>
#include <y60/ShapeBuilder.h>
#include <y60/ElementBuilder.h>

namespace y60 {

    template <class VoxelT, class OutputPolicy> class MarchingCubes;

    template <class VoxelT>
    class CountPolygonPolicy {
        typedef MarchingCubes<VoxelT, CountPolygonPolicy<VoxelT> > MyMarcher;
        public:
            CountPolygonPolicy() : _myVertexCount(0), _myHalfEdgeCount(0) {};

            int onVertex(const MyMarcher & theMarchingCubes,
                        int n, const asl::Vector3i & theMarchPos) 
            {
	    	    return ++_myVertexCount;
		    }

            bool ignoreHalfEdges() const { 
                return true;
            }
            void onHalfEdges(const MCLookup::CubeCase & theCubeCase, const std::vector<int> & theEdgeTable) {
                _myHalfEdgeCount += theCubeCase.faces.size();
            }
            std::string getDescription() const {
                return std::string("estimating");
            }
            unsigned getTriangleCount() const {
                return _myHalfEdgeCount / 3;
            }
            unsigned getHalfEdgeCount() const {
                return _myHalfEdgeCount;
            }
            unsigned getVertexCount() const {
                return _myVertexCount;
            }
            void done() const {
                AC_INFO << "done.";
            }

        private:
            unsigned int _myVertexCount;
            unsigned int _myHalfEdgeCount;
    };

    template <class VoxelT>
    class ExportShapePolicy {
        typedef std::pair<int, int> EdgeId;
        typedef std::map<EdgeId, int> EdgeCache;
        typedef MarchingCubes<VoxelT, ExportShapePolicy<VoxelT> > MyMarcher;
        public:
            ExportShapePolicy(SceneBuilderPtr theSceneBuilder, 
                              const std::string & theShapeName,
                              const std::string & theMaterialId,
                              unsigned theNumVertices, unsigned theNumTriangles,
                              bool theVertexNormalFlag ) :
            _myVertexColorFlag(true),
            _myNormals(0),
            _myVertices(0),
            _myIndices(0),
            _myHalfEdges(0),
    		_myVertexCount(theNumVertices),
			_myHalfEdgeCount(theNumTriangles*3),
            _myHalfEdgeFlag(true),
            _myInvertNormalsFlag(false),
            _myVertexNormalFlag(theVertexNormalFlag)
            {
                _myShapeNode = prepareShapeBuilder(theSceneBuilder, theShapeName, theMaterialId);
            };

            int onVertex(const MyMarcher & theMarchingCubes,
                        int n, const asl::Vector3i & theMarchPos) 
            {
                asl::Point3f myVertexPosition;
                asl::Vector3f myVertexNormal;
                theMarchingCubes.computeVertexPosition(n, theMarchPos, myVertexPosition, 
                                _myVertexNormalFlag ? &myVertexNormal : 0 );
                if (_myInvertNormalsFlag) {
                    myVertexNormal = -myVertexNormal;
                }
                asl::Vector3f myVertex(myVertexPosition[0], - myVertexPosition[1], myVertexPosition[2]);
                AC_TRACE << "added vertex position " << myVertex;
                _myVertices->push_back(myVertex);
                if (_myVertexNormalFlag) {
                    AC_TRACE << "added vertex normal " << myVertexNormal;
                    _myNormals->push_back(normalized(asl::Vector3f(myVertexNormal[0], - myVertexNormal[1], myVertexNormal[2])));
                }
                AC_TRACE << "new size " << _myVertices->size();
                return _myVertices->size()-1;
            }
            bool ignoreHalfEdges() const { 
                return false;
            }
            void genHalfEdges(bool theFlag) { 
                _myHalfEdgeFlag = theFlag; 
            }
            std::string getDescription() const {
                return std::string("polygonizing");
            }
            // 1: calc vertex normals using gradients, 0: no normals (flat shading)
            void invertNormal(bool theFlag) { _myInvertNormalsFlag = theFlag; }

            unsigned getVertexCount() const {
                return _myIndices->size();
            }
            unsigned getHalfEdgeCount() const {
                return _myHalfEdges->size();
            }
            void onHalfEdges(const MCLookup::CubeCase & theCubeCase, const std::vector<int> & theEdgeTable) {
                int myFirstFaceIndex = getVertexCount();
                for (int i = 0; i < theCubeCase.faces.size(); ++i) {
                    int myCornerIndex = theCubeCase.faces[i];
                    int myNextCornerIndex = theCubeCase.faces[i - (i % 3) + ((i+1) % 3)];
                    int myIndex = theEdgeTable[myCornerIndex];
                    if (_myHalfEdgeFlag) {
                        int myNextIndex = theEdgeTable[myNextCornerIndex];
                        const MCLookup::HalfEdgeNeighbor & myNeighbor = theCubeCase.neighbors[i];
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
                                    int myValue = getHalfEdgeCount();
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
            dom::NodePtr getShapeNode() const {
                return _myShapeNode;
            }
            void done() {
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
            }
        private:
            ShapeBuilderPtr   _myShapeBuilder;
            ElementBuilderPtr _myElementBuilder;
            dom::NodePtr _myShapeNode;
            dom::NodePtr _myVertexNode;
            dom::NodePtr _myNormalNode;
            dom::NodePtr _myIndexNode;
            dom::NodePtr _myHalfEdgeNode;
            dom::NodePtr _myColorIndexNode;
            std::vector<asl::Vector3f> * _myVertices;
            std::vector<signed int>    * _myHalfEdges;
            std::vector<asl::Vector3f> * _myNormals;            
            std::vector<unsigned int>  * _myIndices;
            bool _myHalfEdgeFlag;
            int  _myHalfEdgeCount;
			int  _myVertexCount;
            bool _myVertexColorFlag;
            bool _myVertexNormalFlag;
            bool _myInvertNormalsFlag;
            EdgeCache                    _myHalfEdgeCache;

            dom::NodePtr prepareShapeBuilder(SceneBuilderPtr theSceneBuilder, 
                                             const std::string & theShapeName,
                                             const std::string & theMaterialId) 
            { 
                _myShapeBuilder = ShapeBuilderPtr( new ShapeBuilder(theShapeName) );
                theSceneBuilder->appendShape( * _myShapeBuilder );

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

    };
}

#endif
