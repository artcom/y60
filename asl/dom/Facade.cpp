/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: Facade.cpp,v $
//
//   $Revision: 1.6 $
//
//   Description:
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "Facade.h"

#include "Nodes.h"

#define DB(x) x

using namespace std;

namespace dom {

    Facade::Facade(Node & theNode)
        : TypedNamedNodeMap(dom::Node::ATTRIBUTE_NODE, 0), _myNode(theNode)
        //, _hasRegisteredDependencies(false), _hasOutdatedDependencies(false)
    {}

    void
        Facade::setSelf(FacadePtr theSelf) {
            if (!theSelf) {
                AC_ERROR << "Facade::setSelf: null self pointer passed";
                throw InvalidNullPointerPassed(JUST_FILE_LINE);
            }
            _mySelf = theSelf;
            registerDependenciesRegistrators();

            // [CH]: This should not be neccessary. It causes all images to be loaded twice. If anybody
            // has strange effects, please talk to me :-)
            //markAllDirty();
    }

    void
        Facade::markAllDirty() {
            DB(AC_TRACE << "Facade::markAllDirty: this ="<<(void*)this);

            Node & myNode = getNode();
            for (size_type i=0; i < myNode.attributes().size(); ++i) {
                myNode.attributes()[i].nodeValueWrapperPtr()->setDirty();
            }
            for (size_type i=0; i < size(); ++i) {
                item(i)->nodeValueWrapperPtr()->setDirty();
            }
    }

    void
        Facade::markPrecursorDependenciesOutdated() {
            for (size_type i=0; i < size(); ++i) {
                item(i)->nodeValueWrapperPtr()->markPrecursorDependenciesOutdated();
            }
    }

    Facade *
        FacadeFactory::createFacade(const DOMString & theType, Node & theNode,
        const DOMString & theParentNodeName) const
    {
        DB(AC_TRACE << "FacadeFactory::createFacade('"<<theType<<"','"<<theParentNodeName<<"')"; )
            FacadePtr myPrototype = findPrototype(FacadeKey(theType, theParentNodeName));
        if (myPrototype) {
            DB(AC_TRACE << "FacadeFactory::createFacade('" << theType << ", "
                << theParentNodeName<<"') returns value";)
                return myPrototype->createNew(theNode);
        }
        DB(AC_TRACE << "FacadeFactory::createFacade('"<<theType<<"') returns 0";)
            return 0;
    }

    void
        FacadeFactory::registerPrototype(const DOMString & theType, FacadePtr thePrototype,
        const DOMString & theParentNodeName)
    {
        DB(AC_TRACE << "FacadeFactory::registerPrototype('" << theType << ", "
            << theParentNodeName<<"')" << endl;)
            _myPrototypes[FacadeKey(theType,theParentNodeName)] = FacadePtr(thePrototype->createNew(Node::Prototype));
    }

    const FacadePtr
        FacadeFactory::findPrototype(const FacadeKey & thePrototypeKey) const {
            ProtoMap::const_iterator myPrototype = _myPrototypes.find(thePrototypeKey);
            if (myPrototype != _myPrototypes.end()) {
                return myPrototype->second;
            }
            return FacadePtr();
    }

    /******************************************************************
    * Parent Facade
    \*****************************************************************/

    void
        Facade::registerChildName(const std::string & theChildName) {
            if (hasRegisteredChild(theChildName)) {
                throw Facade::DuplicateChildName(theChildName,PLUS_FILE_LINE);
            }
            _myChildNames.insert(theChildName);
    }
    bool
        Facade::hasRegisteredChild(const std::string & theChildName) const {
            return _myChildNames.find(theChildName) != _myChildNames.end();
    }
    const NodePtr
        Facade::getChildNode(const DOMString & theName) const {
            if (hasRegisteredChild(theName)) {
                return Facade::ensureChild(_myNode, theName);
            }
            return NodePtr();
    }
    NodePtr
        Facade::getChildNode(const DOMString & theName) {
            if (hasRegisteredChild(theName)) {
                return Facade::ensureChild(_myNode, theName);
            }
            return NodePtr();
    }


    Facade::PropertyMap &
        Facade::getProperties() const {
            ensureProperties();
            return _myPropertyNodes;
    }

    NodePtr
        Facade::getProperty(const std::string & theName) const {
            ensureProperties();
            PropertyMap::iterator it = _myPropertyNodes.find(theName);
            if (it != _myPropertyNodes.end()) {
                return it->second;
            } else {
                return NodePtr();
            }
    }

    void
        Facade::setNode( Node & theNode) {
            _myNode = theNode;
    }
    NodePtr
        Facade::ensureChild(const Node & theNode, const DOMString & theName) {
            theNode.getFacade()->ensureProperties();
            NodePtr myChild = theNode.childNode(theName);
            if (!myChild) {
                myChild = createChild(theNode, theName);
            }
            return myChild;
    }
    NodePtr
        Facade::createChild(const Node & theNode, const DOMString & theName) {
            try {
                return const_cast<Node&>(theNode).appendChild(NodePtr(new Element(theName)) );
            } catch (asl::Exception & ex) {
                throw Facade::Exception(std::string("Could not add child '") +theName +
                    "' to node:\n" + asl::as_string(theNode) + asl::as_string(ex), PLUS_FILE_LINE);
            }
    }
}
