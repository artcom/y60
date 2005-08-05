/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Value.cpp,v $
//
//   $Revision: 1.7 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Value.h"
#include "Nodes.h"
#include <asl/Logger.h>

#define DB(x) // x
#define DB2(x) // x

using namespace std;
using namespace dom;
using namespace asl;

void
StringValue::bumpVersion() {
    Node * myNode = getNodePtr();
    DB(AC_TRACE << "StringValue::bumpVersion():"<<endl);
    if (myNode) {
        myNode->bumpVersion();
    }
}
void
dom::NodeIDRegistry::registerID(const DOMString & theIDAttributeName, const DOMString & theIDAttributeValue, dom::Node * theElement) {
    DB(AC_TRACE << "Node::registerID: registering "<<theIDAttributeName<<"='"<<theIDAttributeValue<<"' at node "<<(void*)this<<endl);
    IDMap & myMap = _myIDMaps[theIDAttributeName];
    IDMap::iterator myEntry = myMap.find(theIDAttributeValue);
    if (myEntry != myMap.end()) {
        throw Node::DuplicateIDValue(string("An Element with ID attribute '")+theIDAttributeName+"' of value '"+theIDAttributeValue+"' is already registered at the document", PLUS_FILE_LINE);
    }
    myMap[theIDAttributeValue] = theElement;
}

void
dom::NodeIDRegistry::unregisterID(const DOMString & theIDAttributeName, const DOMString & theIDAttributeValue) {
    DB(AC_TRACE << "Node::unregisterID: unregistering "<<theIDAttributeName<<"='"<<theIDAttributeValue<<"' at node "<<(void*)this<<endl);
    IDMaps::iterator myMap = _myIDMaps.find(theIDAttributeName);
    if (myMap == _myIDMaps.end()) {
        throw Node::IDValueNotRegistered(string("Internal Error: No Element with ID attribute '")+theIDAttributeName+"' of any value, especially '"+theIDAttributeValue+"' is registered at the document", PLUS_FILE_LINE);
    }
    IDMap::iterator myEntry = myMap->second.find(theIDAttributeValue);
    if (myEntry == myMap->second.end()) {
        throw Node::IDValueNotRegistered(string("Internal Error: No Element with ID attribute '")+theIDAttributeName+"' with value '"+theIDAttributeValue+"' is registered at the document", PLUS_FILE_LINE);
    }
    myMap->second.erase(myEntry);
    if (!myMap->second.size()) {
        _myIDMaps.erase(myMap);
    }
}
const NodePtr
NodeIDRegistry::getElementById(const DOMString & theId, const DOMString & theIdAttribute) const {
    IDMaps::const_iterator myMap = _myIDMaps.find(theIdAttribute);
    if (myMap != _myIDMaps.end()) {
        IDMap::const_iterator myEntry = myMap->second.find(theId);
        if (myEntry != myMap->second.end()) {
            return myEntry->second->self().lock();
        }
    }
    return NodePtr(0);
}

NodePtr
NodeIDRegistry::getElementById(const DOMString & theId, const DOMString & theIdAttribute) {
    IDMaps::iterator myMap = _myIDMaps.find(theIdAttribute);
    if (myMap != _myIDMaps.end()) {
        IDMap::iterator myEntry = myMap->second.find(theId);
        if (myEntry != myMap->second.end()) {
            return myEntry->second->self().lock();
        }
    }
    return NodePtr(0);
}

void
IDValue::update() const {
    Node * myNode = const_cast<IDValue*>(this)->getNodePtr();
    DB(AC_TRACE << "IDValue::update():"<<endl);
    if (myNode) {
        const DOMString & myCurrentValue = getStringDirect(); // do not make a onSetValue() callback
        if (_myOldValue != myCurrentValue) {
            unregisterID();
            registerID(myCurrentValue);
       }
    }
}

void
IDValue::reparent() const {
    Node * myNode = const_cast<IDValue*>(this)->getNodePtr();
    DB(AC_TRACE << "IDValue::reparent():"<<endl);
    if (myNode) {
        unregisterID();
        const DOMString & myCurrentValue = getStringDirect(); // do not make a onSetValue() callback
        registerID(myCurrentValue);
        myNode->bumpVersion();
    }
}


void
IDValue::setNodePtr(Node * theNode) {
    if (_myRegistry && getNodePtr() != theNode) {
        unregisterID();
    }
    StringValue::setNodePtr(theNode);
    update();
}

void
IDValue::registerID(const DOMString & theCurrentValue) const {
    if (theCurrentValue.size()) {
        Node * myNode = const_cast<IDValue*>(this)->getNodePtr();
        if (myNode->parentNode()) {
            DB(AC_TRACE << "IDValue::registerID(): registerID node="<<(void*)myNode<<","<<myNode->nodeName()<<"='"<<theCurrentValue<<"'"<<endl);
            NodeIDRegistryPtr myIDRegistry = myNode->getIDRegistry();
            myIDRegistry->registerID(myNode->nodeName(), theCurrentValue, &(*(myNode->parentNode())));
            _myOldValue = theCurrentValue;
            _myRegistry = myIDRegistry;
        } else {
            DB(AC_TRACE << "IDValue::registerID(): did not register ID '"<<theCurrentValue<<"', no parent"<<endl);
        }
    } else {
        AC_WARNING << "IDValue::registerID(): could not register ID '"<<theCurrentValue<<"', empty id"<<endl;
    }
}

void
IDValue::unregisterID() const {
    Node * myNode = const_cast<IDValue*>(this)->getNodePtr();
    if (_myRegistry && _myOldValue.size()) {
        Node * myNode = const_cast<IDValue*>(this)->getNodePtr();
        DB(AC_TRACE << "IDValue::unregister(): unregisterID node="<<(void*)myNode<<","<<myNode->nodeName()<<"='"<<_myOldValue<<"'"<<endl);
        _myRegistry.lock()->unregisterID(myNode->nodeName(), _myOldValue);
        _myOldValue.resize(0);
    }
    _myRegistry = NodeIDRegistryPtr(0);
}

IDValue::~IDValue() {
    DB(AC_TRACE << "IDValue::~IDValue():"<<endl);
    unregisterID();
}

ValueFactory::ValueFactory() {
}

ValuePtr
ValueFactory::createValue(const DOMString & theType, Node * theNode) const {
	ValuePtr myPrototype = findPrototype(theType);
	if (myPrototype) {
		DB(AC_TRACE << "ValueFactory::createValue('"<<theType<<"')"<<" returns value"<<std::endl;)
		ValuePtr myValue = myPrototype->clone(theNode);
        myValue->setSelf(myValue);
        return myValue;
	}
	DB(AC_TRACE << "ValueFactory::createValue('"<<theType<<"')"<<" returns 0"<<std::endl;)
	return ValuePtr(0);
}

ValuePtr
ValueFactory::createValue(const DOMString & theType, const DOMString & theValue, Node * theNode) const {
	ValuePtr myPrototype = findPrototype(theType);
	if (myPrototype) {
		DB(AC_TRACE << "ValueFactory::createValue('"<<theType<<"',(String Value))"<<" returns value"<<std::endl;)
		ValuePtr myValue = myPrototype->create(theValue, theNode);
        myValue->setSelf(myValue);
        return myValue;
	}
	DB(AC_TRACE << "ValueFactory::createValue('"<<theType<<"',(String Value))"<<" returns 0"<<std::endl;)
	return ValuePtr(0);
}

ValuePtr
ValueFactory::createValue(const DOMString & theType, const asl::ReadableBlock & theValue, Node * theNode) const {
	ValuePtr myPrototype = findPrototype(theType);
	if (myPrototype) {
		DB(AC_TRACE << "ValueFactory::createValue('"<<theType<<"',(Block Value))"<<" returns value"<<std::endl;)
		ValuePtr myValue = myPrototype->create(theValue, theNode);
        myValue->setSelf(myValue);
        return myValue;
	}
	DB(AC_TRACE << "ValueFactory::createValue('"<<theType<<"',(Block Value))"<<" returns 0"<<std::endl;)
	return ValuePtr(0);
}

void
ValueFactory::registerPrototype(const DOMString & theType, ValuePtr thePrototype) {
	DB(AC_TRACE << "ValueFactory::registerPrototype('"<<theType<<"',"<<(void*)&(*thePrototype)<<")"<<std::endl;)
	_myPrototypes[theType] = thePrototype->clone(0);
}

const ValuePtr
ValueFactory::findPrototype(const DOMString & theType) const {
	ProtoMap::const_iterator myPrototype = _myPrototypes.find(theType);
	if (myPrototype != _myPrototypes.end()) {
		return myPrototype->second;
	}
	return ValuePtr(0);
}
void
ValueFactory::dump() const {
	AC_PRINT << "$$$ Begin of Factory dump:" << std::endl;
	for (ProtoMap::const_iterator it = _myPrototypes.begin(); it!=_myPrototypes.end();++it) {
		AC_PRINT << it->first << " ";
	}
	AC_PRINT << "$$$ End of Factory dump:" << std::endl;
}


void dom::registerStandardTypes(ValueFactory & theFactory) {
	theFactory.registerPrototype("boolean", ValuePtr(new SimpleValue<bool>(false, 0)));

	theFactory.registerPrototype("float", ValuePtr(new SimpleValue<float>(0.0, 0)));
	theFactory.registerPrototype("double", ValuePtr(new SimpleValue<double>(0.0, 0)));

	theFactory.registerPrototype("decimal", ValuePtr(new SimpleValue<double>(0, 0))); // TODO: make special type for this

	theFactory.registerPrototype("long", ValuePtr(new SimpleValue<long>(0, 0)));
	theFactory.registerPrototype("int", ValuePtr(new SimpleValue<int>(0, 0)));
	theFactory.registerPrototype("short", ValuePtr(new SimpleValue<short>(0, 0)));
	theFactory.registerPrototype("byte", ValuePtr(new SimpleValue<char>(0, 0)));

	theFactory.registerPrototype("unsignedLong", ValuePtr(new SimpleValue<unsigned long>(0, 0)));
	theFactory.registerPrototype("unsignedInt", ValuePtr(new SimpleValue<unsigned int>(0, 0)));
	theFactory.registerPrototype("unsignedShort", ValuePtr(new SimpleValue<unsigned short>(0, 0)));
	theFactory.registerPrototype("unsignedByte", ValuePtr(new SimpleValue<unsigned char>(0, 0)));

	theFactory.registerPrototype("hexBinary", ValuePtr(new VectorValue<asl::Block>(0)));

	theFactory.registerPrototype("ID", ValuePtr(new IDValue(0)));

    ValuePtr myStringValue(new StringValue(0));
	theFactory.registerPrototype("string", myStringValue);
	theFactory.registerPrototype("normalizedString", myStringValue);  // TODO: make special type for this
	theFactory.registerPrototype("token", myStringValue);    // TODO: make special type for this
	theFactory.registerPrototype("language", myStringValue); // TODO: make special type for this
	theFactory.registerPrototype("Name", myStringValue);     // TODO: make special type for this
	theFactory.registerPrototype("NCName", myStringValue);   // TODO: make special type for this
	theFactory.registerPrototype("NMTOKEN", myStringValue);  // TODO: make special type for this
	theFactory.registerPrototype("IDREF", myStringValue);    // TODO: make special type for this
	theFactory.registerPrototype("ENTITY", myStringValue);   // TODO: make special type for this
}

