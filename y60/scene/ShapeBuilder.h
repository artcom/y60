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

#ifndef _ac_y60_ShapeBuiler_h_
#define _ac_y60_ShapeBuiler_h_

#include "y60_scene_settings.h"

#include "BuilderBase.h"
#include <y60/base/NodeNames.h>
#include <y60/base/TypeTraits.h>
#include <y60/base/iostream_functions.h>

#include <asl/dom/Nodes.h>
#include <asl/math/linearAlgebra.h>
#include <asl/base/Ptr.h>

#include <vector>
#include <string>
#include <map>

namespace y60 {

    class ElementBuilder;
    class CameraBuilder;

    class Y60_SCENE_DECL ShapeBuilder : public BuilderBase {
    public:
        ShapeBuilder(const std::string & theName,
                     bool theTestAlmostEqualFlag = false);
        ShapeBuilder(dom::NodePtr theShapeNode);
        virtual ~ShapeBuilder();

        int appendElements(const ElementBuilder & theElements);

        void setBackFacing(bool isBackFacing);
        void setFrontFacing(bool isFrontFacing);

        template <class T>
        unsigned appendVertexData(const std::string & theName, const T & theData);

        template <class T>
        dom::NodePtr createVertexDataBin(const std::string & theName, unsigned theSize = 0,
                                         const VertexBufferUsage theUsage = y60::getDefaultVertexBufferUsage());

        template <class T>
        unsigned getVertexDataSize(const std::string & theName);
    private:
        typedef std::map<std::string, dom::NodePtr> VertexDataMap;

        VertexDataMap   _myVertexDataBins;

        bool _myTestAlmostEqualFlag;

    };

    template <class T>
    dom::NodePtr
    ShapeBuilder::createVertexDataBin(const std::string & theName, unsigned theSize, const VertexBufferUsage theUsage) {
        dom::NodePtr myVertexDataNode = getNode()->childNode(VERTEX_DATA_NAME);

        dom::NodePtr myDataBinNode = myVertexDataNode->appendChild(dom::Element(VectorTypeNameTrait<T>::name()));
        myDataBinNode->appendAttribute(NAME_ATTRIB, theName);
        myDataBinNode->appendAttribute(VERTEX_BUFFER_USAGE_ATTRIB, theUsage);
        myDataBinNode->appendChild(dom::Text(dom::ValuePtr(new dom::VectorValue<std::vector<T>, dom::MakeResizeableVector>(0))));

        dom::NodePtr myTextChild = myDataBinNode->childNode("#text");
        std::vector<T> * myData = myTextChild->dom::Node::nodeValuePtrOpen<std::vector<T> >();
        myData->reserve(theSize);
        myTextChild->dom::Node::nodeValuePtrClose<std::vector<T> >();

        _myVertexDataBins[theName] = myTextChild;
        return myTextChild;
    }

    template <class T>
    unsigned
    ShapeBuilder::appendVertexData(const std::string & theName, const T & theData) {
        VertexDataMap::iterator it = _myVertexDataBins.find(theName);
        if (it != _myVertexDataBins.end()) {
            std::vector<T> * myData = it->second->dom::Node::nodeValuePtrOpen<std::vector<T> >();
            unsigned myNewIndex = 0;
            if (_myTestAlmostEqualFlag) {
                for (; myNewIndex < myData->size(); ++myNewIndex) {
                    if (almostEqual((*myData)[myNewIndex], theData)) {
                        break;
                    }
                }
                if (myNewIndex == myData->size()) {
                    myData->push_back(theData);
                }
#if 0
                else {
                    cerr << "found almostEqual data at " << myNewIndex << endl;
                }
#endif
            }
            else {
                myNewIndex = myData->size();
                myData->push_back(theData);
            }
            it->second->dom::Node::nodeValuePtrClose<std::vector<T> >();
            return myNewIndex;
        } else {
            throw SomException(std::string("Cannot append vertex data to ") + theName +
                " before the bin has been created.",
                "ShapeBuilder::appendVertexData");
        }
    }

    template <class T>
    unsigned
    ShapeBuilder::getVertexDataSize(const std::string & theName) {
        VertexDataMap::iterator it = _myVertexDataBins.find(theName);
        if (it != _myVertexDataBins.end()) {
            return it->second->dom::Node::nodeValueRef<std::vector<T> >().size();
        } else {
            return 0;
        }
    }

    typedef asl::Ptr<ShapeBuilder> ShapeBuilderPtr;
}


#endif
