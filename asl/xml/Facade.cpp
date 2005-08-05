/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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
        markAllDirty();
    }
 
    void 
    Facade::markAllDirty() {
        DB(AC_TRACE << "Facade::markAllDirty: this ="<<(void*)this); 

        Node & myNode = getNode();
        for (int i=0; i < myNode.attributes().size(); ++i) {
            myNode.attributes()[i].nodeValueWrapperPtr()->setDirty();
        }
        for (int i=0; i < size(); ++i) {
            item(i)->nodeValueWrapperPtr()->setDirty();
        }
    }

    void 
    Facade::markPrecursorDependenciesOutdated() {
        for (int i=0; i < size(); ++i) {
            item(i)->nodeValueWrapperPtr()->markPrecursorDependenciesOutdated();
        }
    }
                
    Facade *
    FacadeFactory::createFacade(const DOMString & theType, Node & theNode) const {
    	FacadePtr myPrototype = findPrototype(theType); 
    	if (myPrototype) {
    		DB(AC_TRACE << "FacadeFactory::createValue('"<<theType<<"') returns value"<<endl;)
            return myPrototype->createNew(theNode);
    	}
    	DB(AC_TRACE << "FacadeFactory::createValue('"<<theType<<"',) returns 0"<<endl;)
    	return 0;
    }	
    
	void
	FacadeFactory::registerPrototype(const DOMString & theType, FacadePtr thePrototype) {
    	DB(AC_TRACE << "FacadeFactory::registerPrototype('" << theType << "')" << endl;)
            _myPrototypes[theType] = FacadePtr(thePrototype->createNew(Node::Prototype));	    
	}
	
    const FacadePtr
    FacadeFactory::findPrototype(const DOMString & theType) const {
    	ProtoMap::const_iterator myPrototype = _myPrototypes.find(theType);
    	if (myPrototype != _myPrototypes.end()) {
    		return myPrototype->second;
    	}
    	return FacadePtr(0);
    }	
}
