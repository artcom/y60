//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_y60_ShapeBuiler_h_
#define _ac_y60_ShapeBuiler_h_

#include "BuilderBase.h"
#include <y60/NodeNames.h>
#include <y60/TypeTraits.h>
#include <y60/iostream_functions.h>

#include <dom/Nodes.h>
#include <asl/linearAlgebra.h>
#include <asl/Ptr.h>

#include <vector>
#include <string>
#include <map>

namespace y60 {

    class ElementBuilder;
    class CameraBuilder;

    class ShapeBuilder : public BuilderBase {
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
