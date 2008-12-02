//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: ElementBuilder.cpp,v 1.2 2005/04/13 09:41:41 christian Exp $
//   $RCSfile: ElementBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/04/13 09:41:41 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "ElementBuilder.h"
#include <y60/base/DataTypes.h>

#include <asl/base/Dashboard.h>
#include <asl/dom/Nodes.h>

using namespace asl;

namespace y60 {
    ElementBuilder::ElementBuilder(const std::string & thePrimitiveType,
                                   const std::string & theMaterial)
        : BuilderBase(ELEMENTS_NODE_NAME),
          _myMaterialId(theMaterial),
          _myPrimitiveType(thePrimitiveType)
    {
        getNode()->appendAttribute(PRIMITIVE_TYPE_ATTRIB, _myPrimitiveType);
        getNode()->appendAttribute(MATERIAL_REF_ATTRIB, theMaterial);

        _myVertexIndexBins.resize(MAX_VERTEX_DATA_ROLE);
    }

    dom::NodePtr
    ElementBuilder::createIndex(const std::string & theVertexDataName,
                                VertexDataRole theRole,
                                unsigned theSize)
    {
        dom::NodePtr myIndexNode = getNode()->appendChild(dom::Element(VERTEX_INDICES_NAME));
        myIndexNode->appendAttribute(VERTEX_DATA_ATTRIB, theVertexDataName);
        myIndexNode->appendAttribute(VERTEX_DATA_ROLE_ATTRIB, getStringFromEnum(theRole, VertexDataRoleString));

        IndicesValueTypePtr myIndicesValue(new IndicesValueType(0));
        dom::NodePtr myTextChild = myIndexNode->appendChild(dom::Text(myIndicesValue));
        dom::Node::WritableValue<VectorOfUnsignedInt> myIndicesLock(myTextChild);
        VectorOfUnsignedInt & myIndices = myIndicesLock.get();
        myIndices.reserve(theSize);
        _myVertexIndexBins[theRole].vertexdata = theVertexDataName;
        _myVertexIndexBins[theRole].role       = theRole;
        _myVertexIndexBins[theRole].indices    = myTextChild;
        return myTextChild;
    }

    void
    ElementBuilder::appendIndex(VertexDataRole theRole, unsigned theIndex) {
        MAKE_SCOPE_TIMER(ElementBuilder_appendIndex);

        if (_myVertexIndexBins[theRole].indices) {
            //_myVertexIndexBins[theRole].indices->push_back(theIndex);
            dom::Node::WritableValue<VectorOfUnsignedInt> myIndicesLock(_myVertexIndexBins[theRole].indices);
            myIndicesLock.get().push_back(theIndex);
        } else {
            throw SomException(std::string("Cannot append vertex index for ") +
                getStringFromEnum(theRole, VertexDataRoleString) +
                " before the bin has been created.", "ElementBuilder::appendIndex");
        }
    }

    void 
    ElementBuilder::copyIndexBin(const VertexDataRole & theSourceRole,
                                 const VertexDataRole & theTargetRole,
                                 const std::string & theTargetVertexDataName)
    {
        const VectorOfUnsignedInt & mySourceIndices
            = _myVertexIndexBins[theSourceRole].indices->nodeValueRef<VectorOfUnsignedInt>();

        createIndex(theTargetVertexDataName, theTargetRole, mySourceIndices.size());

        dom::Node::WritableValue<VectorOfUnsignedInt>
            myTargetIndicesLock(_myVertexIndexBins[theTargetRole].indices);

        myTargetIndicesLock.get() = mySourceIndices;
    }

    void 
    ElementBuilder::appendHalfEdge(int theTwin) {
        dom::NodePtr myHalfEdgeNode = getNode()->childNode(HALFEDGES_NODE_NAME);        
        dom::Node::WritableValue<VectorOfSignedInt> myHalfEdgesLock(myHalfEdgeNode->childNode(0));
        VectorOfSignedInt & myHalfEdges = myHalfEdgesLock.get();
        myHalfEdges.push_back(theTwin);
        if (theTwin >= 0) {
            myHalfEdges[theTwin] = myHalfEdges.size()-1;
        }
    }

    dom::NodePtr
    ElementBuilder::createHalfEdges() {
        int myNumFaceVertices;
        if (_myPrimitiveType == "triangles") {
            myNumFaceVertices = 3;
        } else if (_myPrimitiveType == "quads") {
            myNumFaceVertices = 4;
        } else {
            AC_ERROR << "Can't generate halfedges for other elements than triangles and quads";
            return dom::NodePtr(0);
        }
        dom::NodePtr myHalfEdgesNode = getNode()->appendChild(dom::Element(HALFEDGES_NODE_NAME));

        HalfEdgesValueTypePtr myHalfEdgesValue(new HalfEdgesValueType(0));
        dom::NodePtr myTextChild = myHalfEdgesNode->appendChild(dom::Text(myHalfEdgesValue));
        return myTextChild;
    }

    int 
    ElementBuilder::getHalfEdgeSize() const {
        dom::NodePtr myHalfEdgeNode = getNode()->childNode(HALFEDGES_NODE_NAME);
        dom::Node::WritableValue<VectorOfSignedInt> myHalfEdgesLock(myHalfEdgeNode->childNode(0));
        return myHalfEdgesLock.get().size();

    }
    
    void ElementBuilder::generateHalfEdges() {
        int myNumFaceVertices;
        if (_myPrimitiveType == "triangles") {
            myNumFaceVertices = 3;
        } else if (_myPrimitiveType == "quads") {
            myNumFaceVertices = 4;
        } else {
            AC_ERROR << "Can't generate halfedges for other elements than triangles and quads";
            return;
        }

        typedef std::pair<int, int> EdgeId;
        typedef std::map<EdgeId, int> EdgeMap;


        EdgeMap myEdgeMap;

        dom::NodePtr myPositionsNode = getNode()->childNodeByAttribute(VERTEX_INDICES_NAME, 
            VERTEX_DATA_ROLE_ATTRIB, POSITION_ROLE);
        const VectorOfUnsignedInt & myPositions = 
            myPositionsNode->childNode(0)->nodeValueRef<VectorOfUnsignedInt>();

        dom::NodePtr myHalfEdgesNode = getNode()->appendChild(dom::Element(HALFEDGES_NODE_NAME));

        HalfEdgesValueTypePtr myHalfEdgesValue(new HalfEdgesValueType(0));
        dom::NodePtr myTextChild = myHalfEdgesNode->appendChild(dom::Text(myHalfEdgesValue));
        dom::Node::WritableValue<VectorOfSignedInt> myHalfEdgesLock(myTextChild);
        VectorOfSignedInt & myHalfEdges = myHalfEdgesLock.get();
        myHalfEdges.resize(myPositions.size(), -1);
        if (myHalfEdges.size() != myPositions.size()) {
            AC_ERROR << myHalfEdges.size() << " = myHalfEdges->size() != myPositions.size() = " << myPositions.size();
        }
        AC_INFO << "Generating HalfEdges for " << myPositions.size() << " vertices";
        for (VectorOfUnsignedInt::size_type i=0; i < myPositions.size(); ++i) {
            EdgeId myEdgeId;
            myEdgeId.first = myPositions[i];
            if (static_cast<int>(i % myNumFaceVertices) != (myNumFaceVertices-1)) {
                myEdgeId.second = myPositions[i+1];
            } else {
                myEdgeId.second = myPositions[i-myNumFaceVertices+1];
            }
            if (myEdgeId.first > myEdgeId.second) {
                myEdgeId = EdgeId(myEdgeId.second, myEdgeId.first);
            }
            EdgeMap::iterator myIterator = myEdgeMap.find(myEdgeId);
            int myValue = -1;
            if (myIterator != myEdgeMap.end()) {
                myValue = myIterator->second;
                AC_TRACE << myValue << " is a twin of " << i;
                myHalfEdges[myValue] = i;
                myHalfEdges[i] = myValue;
            } else {
                myEdgeMap.insert(EdgeMap::value_type(myEdgeId, i));
            }
            //myHalfEdgesLock.get()[i] = (myValue);
        }
        AC_INFO << "HalfEdges generation finished.";
    }
}
