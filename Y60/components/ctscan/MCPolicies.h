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

#include <asl/Vector234.h>
#include <dom/Value.h>
#include <y60/Scene.h>
#include <y60/ShapeBuilder.h>
#include <y60/ElementBuilder.h>

#include <vector>

namespace y60 {

    template <class VoxelT, class OutputPolicy, class SegmentationPolicy> class MarchingCubes;

    template <class VoxelT, class SegmentationPolicy>
    class CountPolygonPolicy {
        typedef MarchingCubes<VoxelT,
                              CountPolygonPolicy<VoxelT, SegmentationPolicy>,
                              SegmentationPolicy > MyMarcher;
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

    template <class VoxelT, class SegmentationPolicy>
    class ExportShapePolicy {
        typedef std::pair<int, int> EdgeId;
        typedef std::map<EdgeId, int> EdgeCache;
        typedef MarchingCubes<VoxelT,
                              ExportShapePolicy<VoxelT, SegmentationPolicy>,
                              SegmentationPolicy > MyMarcher;
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
                DB(AC_TRACE << "added vertex position " << myVertex);
                _myVertices->push_back(myVertex);
                if (_myVertexNormalFlag) {
                    DB(AC_TRACE << "added vertex normal " << myVertexNormal);
                    _myNormals->push_back(normalized(asl::Vector3f(myVertexNormal[0], - myVertexNormal[1], myVertexNormal[2])));
                }
                DB(AC_TRACE << "new size " << _myVertices->size());
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
                                    myNeighbor.type == MCLookup::MAX_Z)
                            {
                                // Add to the map
                                EdgeId myKey(myIndex, myNextIndex);
                                int myValue = getHalfEdgeCount();
                                EdgeCache::value_type myItem(myKey, myValue);
                                _myHalfEdgeCache.insert(myItem);
                                DB(AC_TRACE << "Inserting: (" << myKey.first << ", " << myKey.second << ")");
                            } else {
                                // Remove from map
                                EdgeId myKey(myNextIndex, myIndex);
                                EdgeCache::iterator iter = _myHalfEdgeCache.find(myKey);
                                if (_myHalfEdgeCache.end() != iter) {
                                    myHalfEdge = (*iter).second;
                                    _myHalfEdgeCache.erase(iter);
                                    DB(AC_TRACE << "Removing: (" << myKey.first << ", " << myKey.second << ")");
                                    _myHalfEdges->at(myHalfEdge) = _myHalfEdges->size(); // we will push the other edge soon
                                }
                            }
                        }
                        DB(AC_TRACE << "added HalfEdge " << myHalfEdge << " as element#" << _myHalfEdges->size());
                        _myHalfEdges->push_back(myHalfEdge); // push the other edge
                        DB(AC_TRACE << "   size is now " << _myHalfEdges->size());
                    }
                    _myIndices->push_back(myIndex);
                    DB(AC_TRACE << myIndex << (*_myVertices)[myIndex]); 
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
                DB(AC_INFO << "reserving Vertex  :" << _myVertexCount);
                DB(AC_INFO << "reserving HalfEdge:" << _myHalfEdgeCount);
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

    DEFINE_EXCEPTION(MCPolicyException, asl::Exception);

    template <class VoxelT>
    class GlobalThresholdSegmentationPolicy {
        public:
            GlobalThresholdSegmentationPolicy(const VoxelT & theLower, const VoxelT & theUpper) :
                _myThresholds( theLower, theUpper ) {}
        
            GlobalThresholdSegmentationPolicy(const asl::Vector2<VoxelT> & theThresholds) :
                _myThresholds( theThresholds ) {}
        
            inline bool
            isOutside(int x, int y, int z, VoxelT theValue) const {
                return theValue < _myThresholds[0] || theValue > _myThresholds[1];
            }

            inline float interpolatePosition(const std::vector<VoxelT> & theVoxelCube,
                         int theFirstIndex, int theSecondIndex, bool & theInsideOutFlag) const 
            {
                const VoxelT & firstValue = theVoxelCube[theFirstIndex];
                const VoxelT & secondValue = theVoxelCube[theSecondIndex];
                

                if ((firstValue <= _myThresholds[0] && secondValue >= _myThresholds[0]) ||
                    (firstValue >= _myThresholds[0] && secondValue <= _myThresholds[0])) 
                {
                    float li = (float)(_myThresholds[0] - firstValue) / (float)(secondValue - firstValue);
                    theInsideOutFlag = false;
                    return li;
                } else {
                    if ((firstValue <= _myThresholds[1] && secondValue >= _myThresholds[1]) ||
                        (firstValue >= _myThresholds[1] && secondValue <= _myThresholds[1])) 
                    {
                        float li = (float)(_myThresholds[1] - firstValue) / (float)(secondValue - firstValue);
                        theInsideOutFlag = true;
                        return li;
                    } else {
                        throw MCPolicyException(std::string("Threshold is neither crossed from top or bottom with first: " ) + 
                            as_string(int(firstValue)) + " and second: " + as_string(int(secondValue)), PLUS_FILE_LINE);
                    }
                }
            }       
            inline float interpolateNormal(const VoxelT & theFirstValue, int theFirstIndex, const VoxelT & theSecondValue, 
                const asl::Vector3i & theSecondPosition, bool theUpperFlag) const
            {
                return float(theSecondValue - theFirstValue);
                //if (!theUpperFlag) {
                //    return float(theSecondValue - theFirstValue);
                //    //return float((_myThresholds[0] - theFirstValue) - (_myThresholds[0] - theSecondValue));
                //} else {
                //    return float(theSecondValue - theFirstValue);
                //    //return float((_myThresholds[1] - theFirstValue) - (_myThresholds[1] - theSecondValue));
                //}
            }

            inline void fillThresholdCube(int theI, int theJ, int theK) {
            }
        private:
            asl::Vector2<VoxelT> _myThresholds;
    };

    template <class VoxelT>
    class PerVoxelThresholdSegmentationPolicy {
        public:
            PerVoxelThresholdSegmentationPolicy(dom::NodePtr thePaletteNode, dom::NodePtr theMeasurement,
                                                int theDownsampleRate) : 
                _myThresholdCube(8),
                _myThresholdPalette(256),
                _myDownSampleRate( theDownsampleRate)
            
            {
                for (unsigned i = 0; i < thePaletteNode->childNodesLength("thresholdcolor"); ++i) {
                    dom::NodePtr myPaletteItem = thePaletteNode->childNode("thresholdcolor", i);
                    unsigned char myIndex = static_cast<unsigned char>(
                            myPaletteItem->dom::Node::getAttributeValue<int>("index"));
                    asl::Vector2f myThresholds = myPaletteItem->dom::Node::getAttributeValue<asl::Vector2f>("threshold");
                    _myThresholdPalette[myIndex] = asl::Vector2<VoxelT>( VoxelT(myThresholds[0]), VoxelT(myThresholds[1]));
                }
                dom::NodePtr myRasterList = theMeasurement->childNode("rasters", 0);
                for (unsigned i = 0; i < myRasterList->childNodesLength("rasterofgray"); ++i) {
                    dom::NodePtr myRasterNode = myRasterList->childNode("rasterofgray", i)->childNode(0);
                    _mySegmentationBitmaps.push_back(dynamic_cast_Ptr<dom::ResizeableRaster>(myRasterNode->nodeValueWrapperPtr()));
                }
                asl::Box3f myBoundingBox = theMeasurement->dom::Node::getAttributeValue<asl::Box3f>("boundingbox");
                _myBoundingBox[asl::Box3i::MIN][0] = int( myBoundingBox[asl::Box3f::MIN][0] );
                _myBoundingBox[asl::Box3i::MIN][1] = int( myBoundingBox[asl::Box3f::MIN][1] );
                _myBoundingBox[asl::Box3i::MIN][2] = int( myBoundingBox[asl::Box3f::MIN][2] );
                _myBoundingBox[asl::Box3i::MAX][0] = int( myBoundingBox[asl::Box3f::MAX][0] );
                _myBoundingBox[asl::Box3i::MAX][1] = int( myBoundingBox[asl::Box3f::MAX][1] );
                _myBoundingBox[asl::Box3i::MAX][2] = int( myBoundingBox[asl::Box3f::MAX][2] );
            }

            inline bool
            isOutside(int x, int y, int z, VoxelT theValue) const {
                const asl::Vector2<VoxelT> & myThreshold = getThreshold(x, y, z);
                return theValue < myThreshold[0] || theValue > myThreshold[1];
            }

            inline float interpolateNormal(const VoxelT & theFirstValue, int theFirstIndex, const VoxelT & theSecondValue, 
                const asl::Vector3i & theSecondPosition, bool theUpperFlag) const
            {
                const asl::Vector2<VoxelT> & myFirstThreshold  = _myThresholdCube[theFirstIndex];
                const asl::Vector2<VoxelT> & mySecondThreshold = getThreshold(theSecondPosition[0], theSecondPosition[1], theSecondPosition[2]);
                if (!theUpperFlag) {
                    return float((theSecondValue - mySecondThreshold[0]) - (theFirstValue - myFirstThreshold[0]));
                } else {
                    return float((theSecondValue - mySecondThreshold[1]) - (theFirstValue - myFirstThreshold[1]));
                }
            }

            inline float 
            interpolatePosition(const std::vector<VoxelT> & theVoxelCube,
                int theFirstIndex, int theSecondIndex, bool & theInsideOutFlag) const 
            {
                const VoxelT & firstValue = theVoxelCube[theFirstIndex];
                const VoxelT & secondValue = theVoxelCube[theSecondIndex];
                
                const asl::Vector2<VoxelT> & firstThreshold  = _myThresholdCube[theFirstIndex];
                const asl::Vector2<VoxelT> & secondThreshold = _myThresholdCube[theSecondIndex];
                if ((firstValue <= firstThreshold[0] && secondValue >= secondThreshold[0]) ||
                    (firstValue >= firstThreshold[0] && secondValue <= secondThreshold[0])) 
                {
                    float li = (float)(firstValue-firstThreshold[0]) / 
                            (float)((secondThreshold[0]- firstThreshold[0]) - (secondValue - firstValue));
                    theInsideOutFlag = false;
                    return li;
                } else {
                    if ((firstValue <= firstThreshold[1] && secondValue >= secondThreshold[1]) ||
                        (firstValue >= firstThreshold[1] && secondValue <= secondThreshold[1])) 
                    {
                        float li = (float)(firstValue-firstThreshold[1]) / 
                            (float)((secondThreshold[1]- firstThreshold[1]) - (secondValue - firstValue));
                        theInsideOutFlag = true;
                        return li;
                    } else {
                        throw MCPolicyException(std::string("Threshold is neither crossed from top or bottom with first: " ) + 
                            as_string(int(firstValue)) + " and second: " + as_string(int(secondValue)), PLUS_FILE_LINE);
                    }
                }
            }

            inline void fillThresholdCube(int theI, int theJ, int theK) {
                _myThresholdCube.clear();
                _myThresholdCube.reserve(8);

                _myThresholdCube.push_back( getThreshold(theI + 0, theJ + 0, theK + 0)); 
                _myThresholdCube.push_back( getThreshold(theI + 0, theJ + 1, theK + 0)); 
                _myThresholdCube.push_back( getThreshold(theI + 1, theJ + 1, theK + 0)); 
                _myThresholdCube.push_back( getThreshold(theI + 1, theJ + 0, theK + 0)); 
                _myThresholdCube.push_back( getThreshold(theI + 0, theJ + 0, theK + 1)); 
                _myThresholdCube.push_back( getThreshold(theI + 0, theJ + 1, theK + 1)); 
                _myThresholdCube.push_back( getThreshold(theI + 1, theJ + 1, theK + 1)); 
                _myThresholdCube.push_back( getThreshold(theI + 1, theJ + 0, theK + 1)); 
            }
        private:
            inline
            const asl::Vector2<VoxelT> &
            getThreshold(int theX, int theY, int theZ) const {
                unsigned char myIndex = 0;
                int x = theX * _myDownSampleRate;
                int y = theY * _myDownSampleRate;
                int z = theZ * _myDownSampleRate;
                
                if ( ( x >= _myBoundingBox[asl::Box3i::MIN][0] ) &&  ( x < _myBoundingBox[asl::Box3i::MAX][0]) &&
                     ( y >= _myBoundingBox[asl::Box3i::MIN][1] ) &&  ( y < _myBoundingBox[asl::Box3i::MAX][1]) &&
                     ( z >= _myBoundingBox[asl::Box3i::MIN][2] ) &&  ( z < _myBoundingBox[asl::Box3i::MAX][2]) )
                {
                    x -= _myBoundingBox[asl::Box3i::MIN][0];
                    y -= _myBoundingBox[asl::Box3i::MIN][1];
                    z -= _myBoundingBox[asl::Box3i::MIN][2];
                    // XXX Make this beautiful and fast
                    asl::Vector4f myPixel =  _mySegmentationBitmaps[z]->getPixel(x,y); 
                    myIndex = (unsigned char) myPixel[0];
                }
                return _myThresholdPalette[myIndex]; 
            }

            std::vector<asl::Vector2<VoxelT> >    _myThresholdPalette;
            std::vector<dom::ResizeableRasterPtr> _mySegmentationBitmaps;
            std::vector<asl::Vector2<VoxelT> >    _myThresholdCube;
            asl::Box3i                            _myBoundingBox;
            int                                   _myDownSampleRate;
    };
}


#endif
