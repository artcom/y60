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
*/

// own header
#include "ShapeBuilder.h"

#include "ElementBuilder.h"
#include "Shape.h"
#include <y60/base/NodeValueNames.h>
#include <y60/base/DataTypes.h>

#include <asl/base/Dashboard.h>

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
            myNode->getFacade<Shape>()->set<RenderStylesTag>(myDefaultStyle);
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
            RenderStyles myStyle = getNode()->getFacade<Shape>()->get<RenderStylesTag>();
            myStyle.set(BACK, isBackFacing);
            getNode()->getFacade<Shape>()->set<RenderStylesTag>(myStyle);
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
            RenderStyles myStyle = getNode()->getFacade<Shape>()->get<RenderStylesTag>();
            myStyle.set(FRONT, isFrontFacing);
            getNode()->getFacade<Shape>()->set<RenderStylesTag>(myStyle);
        }
    }
}
