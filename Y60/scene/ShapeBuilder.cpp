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
//   $Id: ShapeBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: ShapeBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//
//=============================================================================

#include "ShapeBuilder.h"
#include "ElementBuilder.h"
#include "Shape.h"
#include <y60/NodeValueNames.h>
#include <y60/DataTypes.h>

#include <asl/Dashboard.h>

#define DB(x) //x


namespace y60 {
    ShapeBuilder::ShapeBuilder(const std::string & theName,
                               bool theTestAlmostEqualFlag) :
        BuilderBase(SHAPE_NODE_NAME),
        _myTestAlmostEqualFlag(theTestAlmostEqualFlag),
        _myFrontFacingFlag(true),
        _myBackFacingFlag(false)
    {
        dom::NodePtr myNode = getNode();

        // Create neccessary container nodes
        (*myNode)(VERTEX_DATA_NAME);
        (*myNode)(PRIMITIVE_LIST_NAME);

        if (!myNode->hasFacade()) {
            myNode->appendAttribute(NAME_ATTRIB, theName);
        } else {
            ShapePtr myShape = getNode()->getFacade<Shape>();
            myShape->set<NameTag>(theName);
        }

        updateRenderStyle();
    }

    ShapeBuilder::ShapeBuilder(dom::NodePtr theShapeNode) :
        _myTestAlmostEqualFlag(false),
        _myFrontFacingFlag(false),
        _myBackFacingFlag(false)
    {
        setNode(theShapeNode);
        dom::NodePtr myVertexDataNode = theShapeNode->childNode(VERTEX_DATA_NAME);
        VectorOfString myRenderStyles =
            theShapeNode->getAttributeValue<VectorOfString>(RENDER_STYLE_ATTRIB);
        for (unsigned i = 0; i < myRenderStyles.size(); ++i) {
            if (myRenderStyles[i] == FRONTFACING_STYLE) {
                _myFrontFacingFlag = true;
            }
            if (myRenderStyles[i] == BACKFACING_STYLE) {
                _myBackFacingFlag = true;
            }
        }
        for (unsigned i = 0; i < myVertexDataNode->childNodesLength(); ++i) {
            dom::NodePtr myCurrentBin = myVertexDataNode->childNode(i);
            std::string myName = myCurrentBin->getAttributeString(NAME_ATTRIB);
            dom::NodePtr myDataNode = myCurrentBin->childNode(0);
            _myVertexDataBins.insert(make_pair(myName, myDataNode));
        }
    }

    ShapeBuilder::~ShapeBuilder() {
    }

    int
    ShapeBuilder::appendElements(const ElementBuilder & theElements) {
        MAKE_SCOPE_TIMER(ShapeBuilder_appendPrimitives);
        dom::NodePtr myPrimitivesNode = getNode()->childNode(PRIMITIVE_LIST_NAME);
        myPrimitivesNode->appendChild(theElements.getNode());
        return myPrimitivesNode->childNodesLength() - 1;
    }

    void
    ShapeBuilder::setBackFacing(bool isBackFacing) {
        _myBackFacingFlag = isBackFacing;
        updateRenderStyle();
    }

    void
    ShapeBuilder::setFrontFacing(bool isFrontFacing) {
        _myFrontFacingFlag = isFrontFacing;
        updateRenderStyle();
    }

    void
    ShapeBuilder::updateRenderStyle() {
        VectorOfString myRenderStyles;
        if (_myFrontFacingFlag) {
            myRenderStyles.push_back(FRONTFACING_STYLE);
        }
        if (_myBackFacingFlag) {
            myRenderStyles.push_back(BACKFACING_STYLE);
        }
        dom::NodePtr myAttribute = getNode()->getAttribute(RENDER_STYLE_ATTRIB);
        dom::DOMString myValue = asl::as_string(myRenderStyles);
        if (myAttribute) {
            myAttribute->nodeValue(myValue);
        } else {
            getNode()->appendAttribute(RENDER_STYLE_ATTRIB, myValue);
        }
    }
}
