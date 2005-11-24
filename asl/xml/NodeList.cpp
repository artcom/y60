/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: NodeList.cpp,v $
//
//   $Revision: 1.17 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "typedefs.h"
#include "NodeList.h"
#include "Nodes.h"

#include <vector>
#include <map>

#define DB(x) //  x
#define DB2(x) //  x

using namespace std;
using namespace dom;

dom::NodePtr
dom::NodeList::removeItem(int i) {
	if (i>=0 && i<size()) {
		NodePtr myResult = item(i);
		_myNodes.erase(_myNodes.begin()+i);
        if (_myShell) {
		    myResult->reparent(0, 0);
            _myShell->bumpVersion();
        }
		return myResult;
	}
	return NodePtr(0);
}

void 
dom::NodeList::clear() {
	for (int i = 0; i < size();++i) {
	    if (_myNodes[i]->parentNode() == _myShell) {
	        _myNodes[i]->reparent(0, 0);
	    }
	}
    resize(0);
    if (_myShell) {
        _myShell->bumpVersion();
    }
}

dom::NodeList::NodeList(Node * theShell):_myShell(theShell) {
    
};

dom::NodeList::~NodeList() {
	for (int i = 0; i < size();++i) {
	    if (_myNodes[i]->parentNode() == _myShell) {
	        _myNodes[i]->reparent(0, 0);
	    }
	}
};

void
dom::NodeList::setVersion(unsigned long long theVersion) {
    for (int i = 0; i < size();++i) {
        _myNodes[i]->setVersion(theVersion);
    }
}

dom::NodeList::NodeList(const NodeList & theOtherList, Node * theShell) : _myShell(theShell){
	_myNodes = theOtherList._myNodes;
}

int dom::NodeList::findIndex(const Node * theNode) const {
	for (int i=0; i < length();++i) {
		if (&(*item(i)) == theNode) {
			return i;
		}
	}
	return -1;
};

NodePtr
dom::NodeList::nextSibling(const Node * theNode) {
	int myIndex = findIndex(theNode);
	if (myIndex >= 0 && myIndex+1 < length()) {
		return item(myIndex+1);
	}
	return NodePtr(0);
}

const NodePtr
dom::NodeList::nextSibling(const Node * theNode) const {
	int myIndex = findIndex(theNode);
	if (myIndex >= 0 && myIndex+1 < length()) {
		return item(myIndex+1);
	}
	return NodePtr(0);
}
NodePtr
dom::NodeList::previousSibling(const Node * theNode) {
	int myIndex = findIndex(theNode);
	if (myIndex >= 1 && myIndex < length()) {
		return item(myIndex-1);
	}
	return NodePtr(0);
}

const NodePtr
dom::NodeList::previousSibling(const Node * theNode) const {
	int myIndex = findIndex(theNode);
	if (myIndex >= 1 && myIndex < length()) {
		return item(myIndex-1);
	}
	return NodePtr(0);
}

Node & dom::NodeList::operator[](int i) {
    return *item(i);
}
const Node & dom::NodeList::operator[](int i) const {
    return *item(i);
}
NodePtr dom::NodeList::item(int i) {
    if (i < 0 || i >= length()) {
        throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
    }
    return _myNodes[i];
}
const NodePtr dom::NodeList::item(int i) const {
    if (i < 0 || i >= length()) {
        throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
    }
    return _myNodes[i];
}
int dom::NodeList::length() const {
    return _myNodes.size();
}
int dom::NodeList::size() const {
    return _myNodes.size();
}

void 
dom::NodeList::binarize(asl::WriteableStream & theDest, Dictionaries & theDict, unsigned long long theIncludeVersion) const {
	theDest.appendUnsigned(size());
	for (int i = 0; i < size();++i) {
		_myNodes[i]->binarize(theDest, &theDict, theIncludeVersion);
	}
}

#ifdef PATCH_STATISTIC
#define PS(x) x
#else
#define PS(x)
#endif

asl::AC_SIZE_TYPE 
dom::NodeList::debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos, Dictionaries & theDict, bool thePatchFlag) {
    DB(AC_TRACE << "dom::NodeList::debinarize theSource.size() = " << theSource.size() << ", thePos = " << thePos << endl);
    asl::AC_SIZE_TYPE mySize;
	thePos = theSource.readUnsigned(mySize,thePos);
    DB(AC_TRACE << "dom::NodeList::debinarize count = " << mySize << endl);
    if (!thePatchFlag) {
	    for (unsigned long n=0; n < mySize; ++n) {
		    NodePtr newNode(new Node);
            if (_myShell) {
 	            newNode->reparent(_myShell, _myShell);
            }
            bool myUnmodifiedProxyFlag = false;
		    thePos = newNode->debinarize(theSource, thePos, &theDict, thePatchFlag, myUnmodifiedProxyFlag);
            newNode->self(newNode);
		    _myNodes.push_back(newNode);
            PS(++theDict._myPatchStat.newNodes);
	    }
    } else {
        // patch mode
        unsigned long di = 0; // destination index
	    for (unsigned long si=0; si < mySize; ) { // src index
            unsigned long theNewPos = thePos;
            DB(AC_TRACE << "NodeList::debinarize si = " << si << ", length = " << length() << endl);
            bool myUnmodifiedProxyFlag = false;
            if (di < length()) {
                // try to match existing node
		        theNewPos = item(di)->debinarize(theSource, thePos, &theDict, thePatchFlag, myUnmodifiedProxyFlag); 
            }
            if (theNewPos == thePos) {
                // unique id did not match
                if (myUnmodifiedProxyFlag) {
                    // new node is marked as unmodified so it can not be new
                    // therefore we delete the current src node
                    removeItem(di);
                } else {
                    // a new node is in the patch
                    NodePtr newNode(new Node);
                    if (_myShell) {
                        newNode->reparent(_myShell, _myShell);
                    }
                    thePos = newNode->debinarize(theSource, thePos, &theDict, false, myUnmodifiedProxyFlag);
                    newNode->self(newNode);
                    if (di < length()) {
                        insert(di,newNode);
                        ++si;
                        ++di;
                    } else {
                        append(newNode);
                        ++si;
                        ++di;
                    }
                    PS(++theDict._myPatchStat.newNodes);
                }
            } else {
                // unique id matches
                thePos = theNewPos;
                ++si;
                ++di;
            }
        }
        // remove superfluous trailing nodes
	    while (length() > mySize) {
            removeItem(length() - 1);
            PS(++theDict._myPatchStat.deletedNodes);
       }
    }
	return thePos;
}

NodePtr
dom::NodeList::append(NodePtr theNewNode) {
    _myNodes.push_back(theNewNode);
    if (_myShell) {
        theNewNode->reparent(_myShell, _myShell);
    }
    theNewNode->self(theNewNode);
    return theNewNode;
}

NodePtr
dom::NodeList::appendWhileParsing(NodePtr theNewNode) {
    _myNodes.push_back(theNewNode);
    theNewNode->self(theNewNode);
    return theNewNode;
}

void dom::NodeList::insert(int theIndex, NodePtr theNewNode) {
    if (theIndex < 0 || theIndex >= length()) {
        throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
    }
    _myNodes.insert(_myNodes.begin()+theIndex, theNewNode);
    if (_myShell) {
        theNewNode->reparent(_myShell, _myShell);
    }
    theNewNode->self(theNewNode);
}

void dom::NodeList::setItem(int theIndex, NodePtr theNewItem) {
    _myNodes[theIndex] = theNewItem;
    if (_myShell) {
	    theNewItem->reparent(_myShell, _myShell);
    }
    theNewItem->self(theNewItem);
}
void dom::NodeList::resize(asl::AC_SIZE_TYPE newSize) {
    _myNodes.resize(newSize);
}

void 
dom::NodeList::reparent(Node * theNewParent, Node * theTopNewParent) {
    _myShell = theNewParent;
    for (int i = 0; i < _myNodes.size(); ++i) {
		_myNodes[i]->reparent(theNewParent, theTopNewParent);
    }
}



NamedNodeMap::NamedNodeMap(Node * theShell) : NodeList(theShell) {}
NamedNodeMap::NamedNodeMap(const NamedNodeMap & other, Node * theShell) : NodeList(other, theShell) {}
Node & NamedNodeMap::operator[](int i) {
    return NodeList::operator[](i);
}
const Node & NamedNodeMap::operator[](int i) const {
    return NodeList::operator[](i);
}
Node & NamedNodeMap::operator[](const DOMString & name) {
    DB(AC_TRACE << "operator[" << name << "]" << std::endl);
    return *getNamedItem(name);
}
const Node & NamedNodeMap::operator[](const DOMString & name) const {
    return *getNamedItem(name);
}
NodePtr NamedNodeMap::getNamedItem(const DOMString & name) {
	int i = findNthNodeNamed(name,0,*this);
	if (i<size()) return item(i);
	return NodePtr(0);
}
const NodePtr NamedNodeMap::getNamedItem(const DOMString & name) const {
	int i = findNthNodeNamed(name,0,*this);
	if (i<size()) return item(i);
	return NodePtr(0);
}

NodePtr NamedNodeMap::setNamedItem(NodePtr node) {
	int i = findNthNodeNamed(node->nodeName(),0,*this);
	if (i<size()) {
		setItem(i, node);
	} else {
		append(node);
	}
	return node;
}

int
dom::NamedNodeMap::countNodesNamed(const String& name, const dom::NodeList & nodes) {
    int counter = 0;
    for (int i = 0; i < nodes.size(); ++i) {
		if (name == nodes[i].nodeName() && nodes[i].nodeType() != dom::Node::DOCUMENT_TYPE_NODE) {
            ++counter;
        }
    }
    return counter;
}

int
dom::NamedNodeMap::findNthNodeNamed(const String& name, int n, const dom::NodeList & nodes)
{
    int counter = 0;
    for (int i = 0; i < nodes.size(); ++i) {
        if (name == nodes[i].nodeName() && nodes[i].nodeType() != dom::Node::DOCUMENT_TYPE_NODE) {
            if (counter == n) {
                return i;
            }
            ++counter;
        }
    }
    return nodes.size();
}



NodePtr
dom::NamedNodeMap::append(NodePtr theNewNode) {
	int i = findNthNodeNamed(theNewNode->nodeName(),0,*this);
	if (i<size()) {
		std::string errorMessage;
		errorMessage += "attribute with name '";
		errorMessage += theNewNode->nodeName();
		errorMessage += "' is already used in this map";
		throw DomException(errorMessage,PLUS_FILE_LINE,DomException::INUSE_ATTRIBUTE_ERR);
	}
    return NodeList::append(theNewNode);
}

NodePtr
dom::NamedNodeMap::appendWhileParsing(NodePtr theNewNode) {
	int i = findNthNodeNamed(theNewNode->nodeName(),0,*this);
	if (i<size()) {
		std::string errorMessage;
		errorMessage += "attribute with name '";
		errorMessage += theNewNode->nodeName();
		errorMessage += "' is already used in this map";
		throw DomException(errorMessage,PLUS_FILE_LINE,DomException::INUSE_ATTRIBUTE_ERR);
	}
    return NodeList::appendWhileParsing(theNewNode);
}

void dom::NamedNodeMap::setItem(int theIndex, NodePtr theNewItem) {
	int i = findNthNodeNamed(theNewItem->nodeName(),0,*this);
	if (i<size() && i != theIndex) {
		std::string errorMessage;
		errorMessage += "attribute with name '";
		errorMessage += theNewItem->nodeName();
		errorMessage += "' is already used in this map";
		throw DomException(errorMessage,PLUS_FILE_LINE,DomException::INUSE_ATTRIBUTE_ERR);
	}
    NodeList::setItem(theIndex, theNewItem);
}

void dom::NamedNodeMap::insert(int theIndex, NodePtr theNewItem) {
	int i = findNthNodeNamed(theNewItem->nodeName(),0,*this);
	if (i<size() && i != theIndex) {
		std::string errorMessage;
		errorMessage += "attribute with name '";
		errorMessage += theNewItem->nodeName();
		errorMessage += "' is already used in this map";
		throw DomException(errorMessage,PLUS_FILE_LINE,DomException::INUSE_ATTRIBUTE_ERR);
	}
    NodeList::insert(theIndex, theNewItem);
}

void dom::TypedNamedNodeMap::checkType(NodePtr theNewNode) {
	if (allowedType && theNewNode->nodeType()!= allowedType) {
		std::string errorMessage;
		errorMessage = "putting a node of type ";
		errorMessage += NodeTypeName[theNewNode->nodeType()];
		errorMessage += " with name '";
		errorMessage += theNewNode->nodeName();
		errorMessage += "' into a NamedNodeMap for nodes of type ";
		errorMessage += NodeTypeName[allowedType];
		errorMessage += " is not allowed";
		throw DomException(errorMessage,PLUS_FILE_LINE,DomException::HIERARCHY_REQUEST_ERR);
	}
}

dom::NodePtr dom::TypedNamedNodeMap::setNamedItem(dom::NodePtr theNode) {
    checkType(theNode);
	return dom::NamedNodeMap::setNamedItem(theNode);
}

NodePtr
dom::TypedNamedNodeMap::append(NodePtr theNewNode) {
    checkType(theNewNode);
    return NamedNodeMap::append(theNewNode);
}

NodePtr
dom::TypedNamedNodeMap::appendWhileParsing(NodePtr theNewNode) {
    checkType(theNewNode);
    return NamedNodeMap::appendWhileParsing(theNewNode);
}

void dom::TypedNamedNodeMap::setItem(int theIndex, NodePtr theNewItem) {
    checkType(theNewItem);
    NamedNodeMap::setItem(theIndex, theNewItem);
}
void dom::TypedNamedNodeMap::insert(int theIndex, NodePtr theNewItem) {
    checkType(theNewItem);
    NamedNodeMap::insert(theIndex, theNewItem);
}
