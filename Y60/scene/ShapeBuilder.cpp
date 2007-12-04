//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ShapeBuilder.h"
#include "ElementBuilder.h"
#include "Shape.h"
#include <y60/NodeValueNames.h>
#include <y60/DataTypes.h>

#include <asl/Dashboard.h>

#define DB(x) //x

using namespace asl;
using namespace dom;

namespace y60 {
    ShapeBuilder::ShapeBuilder(const std::string & theName,
                               bool theTestAlmostEqualFlag) :
        BuilderBase(SHAPE_NODE_NAME),
        _myTestAlmostEqualFlag(theTestAlmostEqualFlag)
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
        RenderStyles myDefaultStyle(BIT(FRONT));
        if (!myNode->hasFacade()) {
	        myNode->appendAttribute(RENDER_STYLE_ATTRIB, as_string(myDefaultStyle));
        } else {
            myNode->getFacade<Shape>()->set<RenderStyleTag>(myDefaultStyle);
        }
        /*
	    myNode->appendAttribute(RENDER_STYLE_ATTRIB, as_string(RenderStyles()));

        setFrontFacing(true);
        setBackFacing(true);
        */
    }

    ShapeBuilder::ShapeBuilder(dom::NodePtr theShapeNode) :
        _myTestAlmostEqualFlag(false)
    {
        setNode(theShapeNode);
        dom::NodePtr myVertexDataNode = theShapeNode->childNode(VERTEX_DATA_NAME);
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
        if (!getNode()->hasFacade()) {
            // hard way - no facades
            RenderStyles myStyle = as<RenderStyles>(getNode()->getAttributeString(RENDER_STYLE_ATTRIB));
            myStyle.set(BACK, isBackFacing);
            getNode()->getAttribute(RENDER_STYLE_ATTRIB)->nodeValue(as_string(myStyle));
        } else {
            // easy way - with facades
            RenderStyles myStyle = getNode()->getFacade<Shape>()->get<RenderStyleTag>();
            myStyle.set(BACK, isBackFacing);
            getNode()->getFacade<Shape>()->set<RenderStyleTag>(myStyle);
        }
    }

    void
    ShapeBuilder::setFrontFacing(bool isFrontFacing) {
        if (!getNode()->hasFacade()) {
            // hard way - no facades
            RenderStyles myStyle = as<RenderStyles>(getNode()->getAttributeString(RENDER_STYLE_ATTRIB));
            myStyle.set(FRONT, isFrontFacing);
            getNode()->getAttribute(RENDER_STYLE_ATTRIB)->nodeValue(as_string(myStyle));
        } else {
            // easy way - with facades
            RenderStyles myStyle = getNode()->getFacade<Shape>()->get<RenderStyleTag>();
            myStyle.set(FRONT, isFrontFacing);
            getNode()->getFacade<Shape>()->set<RenderStyleTag>(myStyle);
        }
    }
}
