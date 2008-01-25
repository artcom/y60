/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
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

namespace dom {
asl::Unsigned32 dom::UniqueId::_myCounter(0);

std::ostream & operator<<(std::ostream& os, const UniqueId & uid) {
    os << uid._myCount << "@" << uid._ptrValue;
    return os;
}
}
void
StringValue::bumpVersion() {
    Node * myNode = getNodePtr();
    DB(AC_TRACE << "StringValue::bumpVersion(): this="<<(void*)this<<" , vtname="<<name());
    if (myNode) {
        DB(AC_TRACE << "StringValue::bumpVersion(): before node="<<(void*)myNode<<" , version="<<myNode->nodeVersion());
        myNode->bumpVersion();
        DB(AC_TRACE << "StringValue::bumpVersion(): after node="<<(void*)myNode<<" , version="<<myNode->nodeVersion());
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

NodeOffsetCatalog &
NodeIDRegistry::getOffsetCatalog() {
    if (!_myOffsets) {
        _myOffsets = asl::Ptr<NodeOffsetCatalog>(new NodeOffsetCatalog);
    }
    return *_myOffsets;
}

const NodeOffsetCatalog &
NodeIDRegistry::getOffsetCatalog() const {
    if (!_myOffsets) {
        _myOffsets = asl::Ptr<NodeOffsetCatalog>(new NodeOffsetCatalog);
    }
    return *_myOffsets;
}

NodeOffsetCatalog::NodeOffsetCatalog(const Node & theRootNode) {
    extractFrom(theRootNode);
}

void
NodeOffsetCatalog::extractFrom(const Node & theRootNode) {
    clear();
    const NodeIDRegistry & theRegistry = *theRootNode.getIDRegistry();
    for (NodeIDRegistry::IDMaps::const_iterator it = theRegistry._myIDMaps.begin();
            it != theRegistry._myIDMaps.end(); ++it) {
        _myIDMaps[it->first] = IDMap();
        IDMap & myCatalog =_myIDMaps[it->first]; 
        for (NodeIDRegistry::IDMap::const_iterator mit = it->second.begin();
                mit != it->second.end(); ++mit) 
        {
            myCatalog[mit->first] = mit->second->getSavePosition();
        }
    }
    theRootNode.collectOffsets(*this);
}

void
NodeOffsetCatalog::binarize(asl::WriteableStream & theDest) const {
    theDest.appendUnsigned32(CatalogMagic);
    theDest.appendUnsigned(_myIDMaps.size());
    for (IDMaps::const_iterator it = _myIDMaps.begin(); it != _myIDMaps.end(); ++it) {
        theDest.appendUnsigned32(CatalogEntriesMagic);
        theDest.appendCountedString(it->first);
        theDest.appendUnsigned(it->second.size());
        for (IDMap::const_iterator mit = it->second.begin();
                mit != it->second.end(); ++mit) 
        {
            theDest.appendCountedString(mit->first);
            theDest.appendUnsigned(mit->second);
        }
    }
    theDest.appendUnsigned32(UIDCatalogMagic);
    binarizePODT(_myNodeOffsets, theDest);
    binarizePODT(_myNodeEndOffsets, theDest);
    binarizePODT(_myParentIndex, theDest);
#if 0
    theDest.appendUnsigned(_myUIDMap.size());
    for (UIDMap::const_iterator mit = _myUIDMap.begin(); mit != _myUIDMap.end(); ++mit) 
    {
        mit->first.append(theDest);
        theDest.appendUnsigned(mit->second);
    }
#endif
    theDest.appendUnsigned32(CatalogEndMagic);
 }

asl::AC_SIZE_TYPE
NodeOffsetCatalog::debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
    _myIDMaps = IDMaps();

    asl::Unsigned32 myMagic = 0;
    asl::AC_SIZE_TYPE theOldPos = thePos;
    thePos = theSource.readUnsigned32(myMagic, thePos);
    if (myMagic != CatalogMagic) {
        std::string myError = "Bad magic reading ID Catalog, thePos=";
        myError += asl::as_string(thePos);
        myError += ", myMagic=";
        myError += asl::as_string((void*)myMagic);
        throw FormatCorrupted(myError, PLUS_FILE_LINE, theOldPos);
    }
    asl::Unsigned64 myCatalogCount;
    thePos = theSource.readUnsigned(myCatalogCount, thePos);

    for (int i = 0; i < myCatalogCount; ++i) {
        asl::Unsigned32 myEntriesMagic = 0;
        thePos = theSource.readUnsigned32(myEntriesMagic, thePos);
        if (myEntriesMagic != CatalogEntriesMagic) {
            throw FormatCorrupted("Bad magic reading ID Catalog entry",PLUS_FILE_LINE);
        }
        DOMString myIDName;
        thePos = theSource.readCountedString(myIDName, thePos);
        if (myIDName.size() == 0) {
            throw FormatCorrupted("empty ID name reading ID catalog",PLUS_FILE_LINE);
        }
        _myIDMaps[myIDName] = IDMap();
        IDMap & myCatalog =_myIDMaps[myIDName]; 

        asl::Unsigned64 myCatalogSize;
        thePos = theSource.readUnsigned(myCatalogSize, thePos);
        for (asl::Unsigned64 j = 0; j < myCatalogSize; ++j) {
            DOMString myName;
            thePos = theSource.readCountedString(myName, thePos);
            if (myName.size() == 0) {
                throw FormatCorrupted("empty catalog entry key",PLUS_FILE_LINE);
            }
            asl::Unsigned64 myOffset;
            thePos = theSource.readUnsigned(myOffset, thePos);

            myCatalog[myName] = myOffset;
        }
    }

    asl::Unsigned32 myUIDCatalogMagic = 0;
    thePos = theSource.readUnsigned32(myUIDCatalogMagic, thePos);
    if (myUIDCatalogMagic != UIDCatalogMagic) {
        throw FormatCorrupted("Bad magic reading UID Catalog",PLUS_FILE_LINE);
    }
    thePos = debinarizePODT(_myNodeOffsets, theSource, thePos);
    thePos = debinarizePODT(_myNodeEndOffsets, theSource, thePos);
    thePos = debinarizePODT(_myParentIndex, theSource, thePos);
#if 0
    asl::Unsigned64 myUIDCatalogSize;
    thePos = theSource.readUnsigned(myUIDCatalogSize, thePos);
    for (asl::Unsigned64 j = 0; j < myUIDCatalogSize; ++j) {
        UniqueId myUID(theSource,thePos);
        asl::Unsigned64 myOffset;
        thePos = theSource.readUnsigned(myOffset, thePos);
        _myUIDMap[myUID] = myOffset;
    }
#endif
    asl::Unsigned32 myCatalogEndMagic = 0;
    thePos = theSource.readUnsigned32(myCatalogEndMagic, thePos);
    if (myCatalogEndMagic != CatalogEndMagic) {
        throw FormatCorrupted("Bad magic reading catalog end",PLUS_FILE_LINE);
    }
    //debug();
    return thePos;
}

bool 
NodeOffsetCatalog::getElementOffsetById(const DOMString & theId, const DOMString & theIdAttribute,  asl::Unsigned64 & theOffset) const {
    IDMaps::const_iterator myMap = _myIDMaps.find(theIdAttribute);
    if (myMap != _myIDMaps.end()) {
        IDMap::const_iterator myEntry = myMap->second.find(theId);
        if (myEntry != myMap->second.end()) {
            return theOffset = myEntry->second;
            return true;
        }
    }
    return false;
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
    DB(AC_TRACE << "IDValue::reparent(): this="<<(void*)this<<" , vtname="<<name());
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

void
dom::NodeIDRefRegistry::registerIDRef(const DOMString & theIDRefAttributeName, const DOMString & theIDRefAttributeValue, dom::Node * theElement) {
    DB(AC_TRACE << "Node::registerIDRef: registering "<<theIDRefAttributeName<<"='"<<theIDRefAttributeValue<<"' at node "<<(void*)this<<endl);
    IDRefMap & myMap = _myIDRefMaps[theIDRefAttributeName];
    IDRefMap::iterator myFirstEntry = myMap.lower_bound(theIDRefAttributeValue);
    IDRefMap::iterator myLastEntry = myMap.upper_bound(theIDRefAttributeValue);
    for (IDRefMap::iterator myIt = myFirstEntry; myIt != myLastEntry; ++myIt) {
        if (myIt->second == theElement) {
            throw Node::DuplicateIDRefValue(string("The Element at "+as_string((void*)theElement)+" with IDRef attribute '")+theIDRefAttributeName+"' of value '"+theIDRefAttributeValue+"' is already registered at the document", PLUS_FILE_LINE);
        }
    }
    myMap.insert(make_pair(theIDRefAttributeValue,theElement));
}

void
dom::NodeIDRefRegistry::unregisterIDRef(const DOMString & theIDRefAttributeName, const DOMString & theIDRefAttributeValue, Node * theElement) {
    DB(AC_TRACE << "Node::unregisterIDRef: unregistering "<<theIDRefAttributeName<<"='"<<theIDRefAttributeValue<<"' at node "<<(void*)this<<endl);
    IDRefMaps::iterator myMapIt = _myIDRefMaps.find(theIDRefAttributeName);
    if (myMapIt == _myIDRefMaps.end()) {
        throw Node::IDRefValueNotRegistered(string("Internal Error: No Element with IDRef attribute '")+theIDRefAttributeName+"' of any value, especially '"+theIDRefAttributeValue+"' is registered at the document", PLUS_FILE_LINE);
    }
    IDRefMap & myMap = myMapIt->second;
    IDRefMap::iterator myFirstEntry = myMap.lower_bound(theIDRefAttributeValue);
    IDRefMap::iterator myLastEntry = myMap.upper_bound(theIDRefAttributeValue);
    for (IDRefMap::iterator myIt = myFirstEntry; myIt != myLastEntry; ++myIt) {
        if (myIt->second == theElement) {
            myMap.erase(myIt);
            if (!myMap.size()) {
                _myIDRefMaps.erase(myMapIt);
            }
            return;
        }
    } 
    throw Node::IDRefValueNotRegistered(string("Internal Error: The Element at "+as_string((void*)theElement)+" with IDRef attribute '")+theIDRefAttributeName+"' of value '"+theIDRefAttributeValue+"' is not registered at the document", PLUS_FILE_LINE);
}
#if 0
void
NodeIDRefRegistry::getElementsReferencingId(const DOMString & theId, const DOMString & theIdAttribute, std::vector<NodePtr const> & theResult) const {
    IDRefMaps::const_iterator myMapIt = _myIDRefMaps.find(theIdAttribute);
    if (myMapIt == _myIDRefMaps.end()) {
        return;
    }
    const IDRefMap & myMap = myMapIt->second;
    IDRefMap::const_iterator myFirstEntry = myMap.lower_bound(theId);
    IDRefMap::const_iterator myLastEntry = myMap.upper_bound(theId);
    for (IDRefMap::const_iterator myIt = myFirstEntry; myIt != myLastEntry; ++myIt) {
        theResult.push_back(myIt->second->self().lock());
    }
}   
#endif

void
NodeIDRefRegistry::getElementsReferencingId(const DOMString & theId, const DOMString & theIdAttribute, std::vector<NodePtr> & theResult) {
    std::vector<NodePtr> myResult;
    IDRefMaps::iterator myMapIt = _myIDRefMaps.find(theIdAttribute);
    if (myMapIt == _myIDRefMaps.end()) {
        return;
    }
    IDRefMap & myMap = myMapIt->second;
    IDRefMap::iterator myFirstEntry = myMap.lower_bound(theId);
    IDRefMap::iterator myLastEntry = myMap.upper_bound(theId);
    for (IDRefMap::iterator myIt = myFirstEntry; myIt != myLastEntry; ++myIt) {
        theResult.push_back(myIt->second->self().lock());
    }
}
// TODO: put in common base class of IDValue; duplicate code
void
IDRefValue::update() const {
    Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
    DB(AC_TRACE << "IDRefValue::update():"<<endl);
    if (myNode) {
        const DOMString & myCurrentValue = getStringDirect(); // do not make a onSetValue() callback
        if (_myOldValue != myCurrentValue) {
            unregisterIDRef();
            registerIDRef(myCurrentValue);
       }
    }
}

// TODO: put in common base class of IDValue; duplicate code
void
IDRefValue::reparent() const {
    Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
    DB(AC_TRACE << "IDRefValue::reparent(): this="<<(void*)this<<" , vtname="<<name());
    if (myNode) {
        unregisterIDRef();
        const DOMString & myCurrentValue = getStringDirect(); // do not make a onSetValue() callback
        registerIDRef(myCurrentValue);
        myNode->bumpVersion();
    }
}

// TODO: put in common base class of IDValue; duplicate code
void
IDRefValue::setNodePtr(Node * theNode) {
    if (_myRegistry && getNodePtr() != theNode) {
        unregisterIDRef();
    }
    StringValue::setNodePtr(theNode);
    update();
}

// TODO: put in common base class of IDValue; duplicate code
void
IDRefValue::registerIDRef(const DOMString & theCurrentValue) const {
    if (theCurrentValue.size()) {
        Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
        if (myNode->parentNode()) {
            DB(AC_TRACE << "IDRefValue::registerIDRef(): registerIDRef node="<<(void*)myNode<<","<<myNode->nodeName()<<"='"<<theCurrentValue<<"'"<<endl);
            NodeIDRefRegistryPtr myIDRefRegistry = myNode->getIDRefRegistry();
            myIDRefRegistry->registerIDRef(myNode->nodeName(), theCurrentValue, &(*(myNode->parentNode())));
            _myOldValue = theCurrentValue;
            _myRegistry = myIDRefRegistry;
        } else {
            DB(AC_TRACE << "IDRefValue::registerIDRef(): did not register IDRef '"<<theCurrentValue<<"', no parent"<<endl);
        }
    } else {
        Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
        if (myNode) {
            AC_DEBUG << "IDRefValue::registerIDRef(): could not register empty IDRef, attr= '"<<myNode->nodeName()
                       <<"', element="<< (myNode->parentNode() ? myNode->parentNode()->nodeName() : std::string("Unknown-No Parent")) <<endl;
        }else {
            AC_DEBUG << "IDRefValue::registerIDRef(): could not register IDRef, empty id and no node associated with value"<<endl;
        }
    }
}

// TODO: put in common base class of IDValue; almost duplicate code (3rd param for unregisterIDRef call)
void
IDRefValue::unregisterIDRef() const {
    Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
    if (myNode && myNode->parentNode() && _myRegistry && _myOldValue.size()) {
        Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
        DB(AC_TRACE << "IDRefValue::unregister(): unregisterIDRef node="<<(void*)myNode<<","<<myNode->nodeName()<<"='"<<_myOldValue<<"'"<<endl);
        _myRegistry.lock()->unregisterIDRef(myNode->nodeName(), _myOldValue, &(*(myNode->parentNode())));
        _myOldValue.resize(0);
    }
    _myRegistry = NodeIDRefRegistryPtr(0);
}

IDRefValue::~IDRefValue() {
    DB(AC_TRACE << "IDRefValue::~IDRefValue():"<<endl);
    unregisterIDRef();
}

NodePtr
IDRefValue::getReferencedElement() const {
    Node * myNode = const_cast<IDRefValue*>(this)->getNodePtr();
    DB(AC_TRACE << "IDRefValue::getReferencedNode():"<<endl);
    if (myNode) {
        return myNode->getElementById(getString(), myNode->nodeName());
    }
    AC_WARNING << "IDRefValue::getReferencedElement: Value is not attached to a node.";
    return NodePtr(0);
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

	theFactory.registerPrototype("long", ValuePtr(new SimpleValue<asl::Signed64>(0, 0))); // keep this arch independent
	theFactory.registerPrototype("int", ValuePtr(new SimpleValue<int>(0, 0)));
	theFactory.registerPrototype("short", ValuePtr(new SimpleValue<short>(0, 0)));
	theFactory.registerPrototype("byte", ValuePtr(new SimpleValue<char>(0, 0)));

	theFactory.registerPrototype("unsignedLong", ValuePtr(new SimpleValue<asl::Unsigned64>(0, 0))); // keep this arch independent
	theFactory.registerPrototype("unsignedInt", ValuePtr(new SimpleValue<unsigned int>(0, 0)));
	theFactory.registerPrototype("unsignedShort", ValuePtr(new SimpleValue<unsigned short>(0, 0)));
	theFactory.registerPrototype("unsignedByte", ValuePtr(new SimpleValue<unsigned char>(0, 0)));

	theFactory.registerPrototype("hexBinary", ValuePtr(new VectorValue<asl::Block>(0)));

	theFactory.registerPrototype("ID", ValuePtr(new IDValue(0)));

    ValuePtr myStringValue(new StringValue(0));    

	//theFactory.registerPrototype("IDREF", ValuePtr(new IDRefValue(0)));
    theFactory.registerPrototype("IDREF", myStringValue);
	theFactory.registerPrototype("string", myStringValue);
	theFactory.registerPrototype("normalizedString", myStringValue);  // TODO: make special type for this
	theFactory.registerPrototype("token", myStringValue);    // TODO: make special type for this
	theFactory.registerPrototype("language", myStringValue); // TODO: make special type for this
	theFactory.registerPrototype("Name", myStringValue);     // TODO: make special type for this
	theFactory.registerPrototype("NCName", myStringValue);   // TODO: make special type for this
	theFactory.registerPrototype("NMTOKEN", myStringValue);  // TODO: make special type for this
	theFactory.registerPrototype("ENTITY", myStringValue);   // TODO: make special type for this
}

