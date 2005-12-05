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
//
//    $RCSfile: Schema.cpp,v $
//
//   $Revision: 1.7 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Schema.h"

#include <iostream>

#define DB(x) // x
#define DB2(x)  // x

using namespace std;
using namespace dom;

Schema::Schema(const dom::DOMString & theSchema, const dom::DOMString & theNSPrefix) : DocumentFragment(theSchema) {
	const NodePtr mySchema = childNode(XS_SCHEMA);
	if (mySchema) {
		preprocess(mySchema, theNSPrefix);
	}
}

void 
Schema::collectDeclarations(const NodePtr theNode,
							const DOMString & theNodeName,
							const DOMString & theAttributeName,
							int theDepth,
							int theMaxDepth,
							Schema::NodeMap & theResults,
							const DOMString & theNSPrefix) {
	if (theNode->nodeType() == Node::ELEMENT_NODE) {
		if (theNode->nodeName() == theNodeName) {
			NodePtr myNameAttribute = theNode->getAttribute(theAttributeName);
			if (myNameAttribute) {
				DOMString myAttrValue = myNameAttribute->nodeValue();
				if (theNSPrefix.size() && myAttrValue.find(COLON) == DOMString::npos) {
					myAttrValue = theNSPrefix+COLON+myAttrValue;
				}
				if (theResults.find(myAttrValue) != theResults.end()) {
					throw DuplicateDeclaration(std::string("Element: '<")+theNodeName+" "+
						theAttributeName+"='"+myAttrValue+"'>",
						"schema::collectDeclarations");
				}
//				AC_TRACE << "inserting name='" << myAttrValue <<"', value=" << *theNode << endl;
				theResults.insert(std::pair<DOMString,const NodePtr>(myAttrValue,theNode));
			}
		}
	}
	if (theDepth < theMaxDepth || theMaxDepth<0) {
		for (int i = 0; i < theNode->childNodes().length(); ++i) {
			collectDeclarations(theNode->childNodes().item(i),
				theNodeName, theAttributeName, theDepth+1, theMaxDepth, theResults, theNSPrefix);
		}
	}
}

void
Schema::collectSubstitutionGroups(const NodePtr theNode,
								const DOMString & theNodeName,
								const DOMString & theGroupAttributeName,
								const DOMString & theNameAttributeName,
								int theDepth,
								int theMaxDepth,
								NameSetMap & theResults,
								const DOMString & theNSPrefix) 
{
	DB2(AC_TRACE << "collectSubstitutionGroups theNodeName='"<<theNodeName <<"', theGroupAttributeName="<< theGroupAttributeName <<"', theNameAttributeName=" << theNameAttributeName << endl);
	if (theNode->nodeType() == Node::ELEMENT_NODE) {
		if (theNode->nodeName() == theNodeName) {
			NodePtr myNameAttribute = theNode->getAttribute(theNameAttributeName);
			NodePtr myGroupAttribute = theNode->getAttribute(theGroupAttributeName);
			if (myNameAttribute && myGroupAttribute) {
				DOMString myAttrValue = myNameAttribute->nodeValue();
				if (theNSPrefix.size() && myAttrValue.find(COLON) == DOMString::npos) {
					myAttrValue = theNSPrefix+COLON+myAttrValue;
				}
				DOMString myGroupAttrValue = myGroupAttribute->nodeValue();
				if (theNSPrefix.size() && myGroupAttrValue.find(COLON) == DOMString::npos) {
					myGroupAttrValue = theNSPrefix+COLON+myAttrValue;
				}
				theResults[myGroupAttrValue].insert(myAttrValue);
				DB(AC_TRACE << "inserting name='" << myAttrValue <<"', group=" << myGroupAttrValue << endl);
			}
		}
	}
	if (theDepth < theMaxDepth || theMaxDepth<0) {
		for (int i = 0; i < theNode->childNodes().length(); ++i) {
			collectSubstitutionGroups(theNode->childNodes().item(i),
				theNodeName, theGroupAttributeName, theNameAttributeName, theDepth+1, theMaxDepth, theResults, theNSPrefix);
		}
	}
}

void
Schema::checkDeclarationReferences(const NodePtr theNode,
							const DOMString & theElementName,
							const DOMString & theAttributeName,
							int theDepth,
							int theMaxDepth,
							const Schema::NodeMap & theLookupMap,
							const Schema::NodeMap * theBuiltInMap) const {
	if (theNode->nodeType() == Node::ELEMENT_NODE) {
		if (theNode->nodeName() == theElementName) {
			NodePtr myRefAttribute = theNode->getAttribute(theAttributeName);
			if (myRefAttribute) {
				if (theLookupMap.find(myRefAttribute->nodeValue())== theLookupMap.end() &&
					theBuiltInMap &&
					theBuiltInMap->find(myRefAttribute->nodeValue())== theBuiltInMap->end()) {
					throw CantResolveReference(std::string("Element: <")+theElementName+" "+
						theAttributeName+"='"+myRefAttribute->nodeValue()+"'>",
						"schema::checkDeclarationReferences");
				}
			}
		}
	}
	if (theDepth < theMaxDepth || theMaxDepth<0) {
		for (int i = 0; i < theNode->childNodes().length(); ++i) {
			checkDeclarationReferences(theNode->childNodes().item(i),
				theElementName,theAttributeName, theDepth+1, theMaxDepth, theLookupMap, theBuiltInMap);
		}
	}
}

void
Schema::preprocess(const NodePtr theNewBranch, const DOMString & theNSPrefix) {
	collectDeclarations(theNewBranch,XS_ELEMENT,ATTR_NAME,0,1,_myTopLevelElementDeclarations,theNSPrefix);
	collectDeclarations(theNewBranch,XS_COMPLEXTYPE,ATTR_NAME,0,1,_myTopLevelTypeDeclarations,theNSPrefix);
	collectDeclarations(theNewBranch,XS_SIMPLETYPE,ATTR_NAME,0,1,_myTopLevelTypeDeclarations,theNSPrefix);
	collectDeclarations(theNewBranch,XS_ATTRIBUTE,ATTR_NAME,0,1,_myTopLevelAttributeDeclarations,theNSPrefix);
	collectDeclarations(theNewBranch,XS_GROUP,ATTR_NAME,0,1,_myTopLevelGroupDeclarations,theNSPrefix);
	collectSubstitutionGroups(theNewBranch,XS_ELEMENT,"substitutionGroup",ATTR_NAME,0,-1,_mySubstitutionGroups,theNSPrefix);
	collectSubstitutionGroups(theNewBranch,XS_ELEMENT,"substitutionGroup",ATTR_REF,0,-1,_mySubstitutionGroups,theNSPrefix);
}

void
Schema::check() const {
	int i = 0;
	while (const NodePtr myBranch = childNode(XS_SCHEMA, i)) {
		checkDeclarationReferences(myBranch,XS_ELEMENT,ATTR_REF,0,-1,_myTopLevelElementDeclarations,0);
		checkDeclarationReferences(myBranch,XS_ELEMENT,ATTR_TYPE,0,-1,_myTopLevelTypeDeclarations,
			&Schema::getBuiltinSchema()._myTopLevelTypeDeclarations);
		++i;
	}
	//dump();
}

void
Schema::dump() const {
	cerr << "================= Begin of XML-Schema Dump ================" << endl;
	cerr << *this;
	cerr << "=== Begin of XML-Schema Top Level Element Declarations Dump" << endl;
	for (Schema::NodeMap::const_iterator it = _myTopLevelElementDeclarations.begin();
		it != _myTopLevelElementDeclarations.end();++it)
	{
		cerr << "Element name = '" << it->first << "', decl=" <<(it->second ? asl::as_string(*it->second) : string("null"))<<endl;
	}
	cerr << "=== Begin of XML-Schema Top Level Type Declarations Dump" << endl;
	for (Schema::NodeMap::const_iterator it = _myTopLevelTypeDeclarations.begin();
		it != _myTopLevelTypeDeclarations.end();++it)
	{
		cerr << "Type name = '" << it->first << "', type=" << it->second->nodeName() <<endl;
	}
	cerr << "=== Begin of XML-Schema Substitution Group Dump" << endl;
	for (Schema::NameSetMap::const_iterator it = _mySubstitutionGroups.begin();
		it != _mySubstitutionGroups.end();++it)
	{
		cerr << "Subsitution Group Name = '" << it->first << "' types = ";
		for (Schema::NameSet::const_iterator sit = it->second.begin();
			sit != it->second.end();++sit) {
			cerr << "'" << *sit << "' ";
		}
		cerr << endl;
	}
	cerr << "============= end of XML-Schema Dump ================" << endl;
}

const NodePtr
Schema::findTopLevelElementDeclaration(const DOMString & theName) const {
	DB(AC_TRACE << "looking for "<<theName << std::endl;)
	DB(AC_TRACE << "_myTopLevelElementDeclarations size= "<<_myTopLevelElementDeclarations.size() << std::endl;)

	NodeMap::const_iterator myResult = _myTopLevelElementDeclarations.find(theName);
	if (myResult != _myTopLevelElementDeclarations.end()) {
		DB(AC_TRACE << "returning "<<*myResult->second << std::endl;)
		return myResult->second;
	}
	DB(AC_TRACE << "not found, returning null" << std::endl;)
	return NodePtr(0);
}

const NodePtr 
Schema::findTopLevelTypeDeclaration(const DOMString & theName) const {
	NodeMap::const_iterator myResult = _myTopLevelTypeDeclarations.find(theName);
	if (myResult != _myTopLevelTypeDeclarations.end()) {
		return myResult->second;
	}
	return NodePtr(0);
}

const NodePtr 
Schema::findTopLevelAttributeDeclaration(const DOMString & theName) const {
	NodeMap::const_iterator myResult = _myTopLevelAttributeDeclarations.find(theName);
	if (myResult != _myTopLevelAttributeDeclarations.end()) {
		return myResult->second;
	}
	return NodePtr(0);
}

const NodePtr 
Schema::findTopLevelGroupDeclaration(const DOMString & theName) const {
	NodeMap::const_iterator myResult = _myTopLevelGroupDeclarations.find(theName);
	if (myResult != _myTopLevelGroupDeclarations.end()) {
		return myResult->second;
	}
	return NodePtr(0);
}


dom::NodePtr
Schema::addDocument(const Node & theDocument, const DOMString & theNSPrefix) {
    if (!theDocument(XS_SCHEMA)) {
        throw SchemaNotParsed(
            std::string("No xs:schema element in '")+asl::as_string(theDocument)+"'",
            "dom::Schema::addDocument");
    }
    dom::NodePtr myNewBranch = appendChild(theDocument.childNode(XS_SCHEMA));
    preprocess(myNewBranch, theNSPrefix);
    return myNewBranch;
}

NodePtr 
Schema::findType(NodePtr theSchemaDeclaration, unsigned long theParsePos) {
    NodePtr myType;
	NodePtr myTypeAttribute = theSchemaDeclaration->getAttribute(ATTR_TYPE);
	if (myTypeAttribute) {
		myType = findTopLevelTypeDeclaration(myTypeAttribute->nodeValue());
		if (myType) {
			return myType;
		}
		// we did'nt find a declaration yet, so lets look if it is a builtin type
		myType = Schema::getBuiltinSchema().findTopLevelTypeDeclaration(myTypeAttribute->nodeValue());
		if (myType) {
			return myType;
		}
		// we could not find a declaration for our type
        throw Schema::ElementWithoutType(asl::as_string(*theSchemaDeclaration),"Node::findSchemaType (type attribute)", theParsePos);
	} else {
		// we don't have a type attribute, so our children describe the type
		myType = theSchemaDeclaration->childNode(XS_COMPLEXTYPE);
		if (myType) {
			return myType;
		}
		myType = theSchemaDeclaration->childNode(XS_SIMPLETYPE);
		if (myType) {
			return myType;
		}
		throw Schema::ElementWithoutType(asl::as_string(*theSchemaDeclaration),"Node::findSchemaType (type by child)", theParsePos);
	}
}

const NodePtr
dom::Schema::findElementDeclaration(const DOMString & theParentElementName,
									const NodePtr theParentElementType,
									const DOMString & theElementName,
									int theParsePos)
{
    NodePtr myResult = tryFindElementDeclaration(theParentElementName,
									theParentElementType,
									theElementName,
									theParsePos);
    if (myResult) {
        return myResult;
    }
    throw Schema::ElementNotAllowed(
            string("'")+theElementName+"' is not allowed as child of '"+theParentElementName+"'"
            +", declaration:"+asl::as_string(*theParentElementType),
            "Node::checkSchemaForElement (complexType)",
            theParsePos);
}

const NodePtr
dom::Schema::tryFindElementDeclaration(const DOMString & theParentElementName,
									const NodePtr theParentElementType,
									const DOMString & theElementName,
									int theParsePos) 
{
    DB(AC_TRACE << "Schema::tryFindElementDeclaration(theParentElementName="<<theParentElementName<<","
									<< "theParentElementType="<< *theParentElementType << ","
									<< "theElementName="<< theElementName << ","<<endl;)

	// if parent is a complex type, lets locate our type
	if (theParentElementType->nodeName() == XS_COMPLEXTYPE || 
        theParentElementType->nodeName() == XS_GROUP ||
        theParentElementType->nodeName() == XS_SEQUENCE ||
        theParentElementType->nodeName() == XS_CHOICE)
    {
		NodePtr myComplexContent = theParentElementType->childNode(XS_COMPLEXCONTENT);
		NodePtr mySequence;
		NodePtr myExtension;
		NodePtr myRestriction;
		NodePtr myChoice;
		NodePtr myGroup;
        if (myComplexContent) {
            myExtension = myComplexContent->childNode(XS_EXTENSION);
            myRestriction = myComplexContent->childNode(XS_RESTRICTION);
            if (myExtension) {
                mySequence = myExtension->childNode(XS_SEQUENCE);
                myChoice = myExtension->childNode(XS_CHOICE);
                myGroup = myExtension->childNode(XS_GROUP);
            } else if (myRestriction) {
                mySequence = myRestriction->childNode(XS_SEQUENCE);
                myChoice = myRestriction->childNode(XS_CHOICE);
                myGroup = myRestriction->childNode(XS_GROUP);
            } else {
                throw ExtensionOrRestrictionRequired(
                    string("'Either xs:extension or xs:restriction required as child of xs:complexContent'")
                    +", element declaration:"+asl::as_string(*theParentElementType),
                    "Node::checkSchemaForElement (complexType)", theParsePos);
             }
        } else {
            mySequence = theParentElementType->childNode(XS_SEQUENCE);
            myChoice = theParentElementType->childNode(XS_CHOICE);
            myGroup = theParentElementType->childNode(XS_GROUP);
        }
        DB(AC_TRACE << "Schema::tryFindElementDeclaration: mySequence="<< (mySequence ? *mySequence : dom::Node()) << endl);
        DB(AC_TRACE << "Schema::tryFindElementDeclaration: myChoice="<< (myChoice ? *myChoice : dom::Node()) << endl);
        DB(AC_TRACE << "Schema::tryFindElementDeclaration: myGroup="<< (myGroup ? *myGroup : dom::Node()) << endl);
        DB(AC_TRACE << "Schema::tryFindElementDeclaration: myExtension="<< (myExtension ? *myExtension : dom::Node()) << endl);
       
		NodePtr myModelGroup = mySequence ? mySequence : myChoice;
        if (myModelGroup && myGroup) {
            throw Schema::ChoiceSequenceGroupConflict(
                    string("'Only one of xs:choice, xs:sequence or xs:group allowed as model group specifier'")
                    +", element declaration:"+asl::as_string(*theParentElementType),
                    "Node::checkSchemaForElement (complexType)", theParsePos);
        }
        if (myGroup) {
			NodePtr myRefAttr = myGroup->getAttribute(ATTR_REF);
			if (myRefAttr) {
				myGroup = findTopLevelGroupDeclaration(myRefAttr->nodeValue());
			}
            myModelGroup = myGroup;
		 }

        if (myModelGroup) {
            DB(AC_TRACE << "Schema::tryFindElementDeclaration: myModelGroup found" << endl;)
			// we found a sequence node in the element type definition, so look
			// for a child with our name
			// TODO: check our order and restrictions here, for now we just see if our element
			//       is mentioned in the sequence at all
            for (int i = 0; i < myModelGroup->childNodes().length();++i) {
			    NodePtr myDecl = myModelGroup->childNode(i);
                DB(AC_TRACE << "Schema::tryFindElementDeclaration: check modelgroup child="<<*myDecl << endl;)
				if (myDecl->nodeName() == XS_ELEMENT) {
                    NodePtr myElementDecl = myDecl;
                    NodePtr myNameAttr = myElementDecl->getAttribute(ATTR_NAME);
                    if (myNameAttr) {
                        if (myNameAttr->nodeValue() == theElementName || 
                                substitutionsGroupContains(myNameAttr->nodeValue(),theElementName)) 
                        {
                            return myElementDecl;
                        }
                    } else {
                        NodePtr myRefAttr = myElementDecl->getAttribute(ATTR_REF);
                        if (myRefAttr->nodeValue() == theElementName || 
                                substitutionsGroupContains(myRefAttr->nodeValue(),theElementName)) 
                        {
                            NodePtr myReferencedElementDecl = findTopLevelElementDeclaration(theElementName);
                            if (myReferencedElementDecl) {
                                return myReferencedElementDecl;
                            }
                            throw Schema::UndeclaredElement(theElementName,
                                    "Node::checkSchemaForElement (sublevel)", theParsePos);
                        }
                    }
                } else if (myDecl->nodeName() == XS_GROUP ||
                           myDecl->nodeName() == XS_SEQUENCE ||
                           myDecl->nodeName() == XS_CHOICE)
                {
                    if (myDecl->nodeName() == XS_GROUP) {
                        NodePtr myRefAttr = myDecl->getAttribute(ATTR_REF);
                        if (myRefAttr) {
                            myDecl = findTopLevelGroupDeclaration(myRefAttr->nodeValue());
                        }
                    }
                    NodePtr myResult = tryFindElementDeclaration(theParentElementName, myDecl, theElementName, theParsePos);
                    if (myResult) {
                        return myResult;
                    }
                }
			}
		}
		// we did not find a matching element declaration yet, so lets see if we have a base type
		if (myExtension) {
			NodePtr myBase = myExtension->getAttribute(ATTR_BASE);
			if (myBase) {
				NodePtr myBaseType = findTopLevelTypeDeclaration(myBase->nodeValue());
				if (myBaseType) {
					return tryFindElementDeclaration(theParentElementName, myBaseType, theElementName, theParsePos);
				}
			}
		}
        if (myGroup) {
		    return tryFindElementDeclaration(theParentElementName, myGroup, theElementName, theParsePos);
        }
        return NodePtr(0);
	}
	if (theParentElementType->nodeName() == XS_SIMPLETYPE) {
		// parent is a simple type and must not have child element
		throw Schema::ElementNotAllowed(string("'")+theElementName+"' is not allowed as child of element'"+theParentElementName+"'"
			+", which is a simpleType, declaration:"+asl::as_string(*theParentElementType),
			"Node::tryFindElementDeclaration", theParsePos);
	}
	return NodePtr(0);
}
// search for the attribute declaration of one the element's attributes
const NodePtr
dom::Schema::getAttributeDeclaration(const NodePtr theElementType, const DOMString & theAttributeName) const {       DB(AC_TRACE << "getAttributeDeclaration("<<theAttributeName<<") const"<< endl);
	if (theElementType->nodeName() == XS_COMPLEXTYPE) {															  DB(AC_TRACE << "getAttributeDeclaration: childNodeByAttribute complexType <xs:attribute name='"<<theAttributeName<<"'>"<<endl);
		NodePtr myAttrDecl = theElementType->childNodeByAttribute(XS_ATTRIBUTE,ATTR_NAME,theAttributeName);
		if (myAttrDecl) {																					  DB(AC_TRACE << "getAttributeDeclaration: childNodeByAttribute : returning myAttrDecl = "<<*myAttrDecl<<endl);
			return myAttrDecl;
		}
		NodePtr myContent = theElementType->childNode(XS_SIMPLECONTENT);
		if (!myContent) {																					  DB(AC_TRACE << "getAttributeDeclaration: childNodeByAttribute : no simpleContent= "<<endl);
			myContent = theElementType->childNode(XS_COMPLEXCONTENT);
		}
		if (myContent) {															                          DB(AC_TRACE << "getAttributeDeclaration: childNodeByAttribute : myContent= "<<*myContent<<endl);
			NodePtr myExtension = myContent->childNode(XS_EXTENSION);
			if (myExtension) {															                      DB(AC_TRACE << "getAttributeDeclaration: childNodeByAttribute : extension = "<<*myExtension<<endl);
				myAttrDecl = myExtension->childNodeByAttribute(XS_ATTRIBUTE,ATTR_NAME,theAttributeName);
				if (myAttrDecl) {														                      DB(AC_TRACE << "getAttributeDeclaration: childNodeByAttribute : returning myAttrDecl = "<<*myAttrDecl<<endl);
					return myAttrDecl;
				}
				NodePtr myBase = myExtension->getAttribute(ATTR_BASE);
				if (myBase) {
					NodePtr myBaseType = findTopLevelTypeDeclaration(myBase->nodeValue());
					if (myBaseType) {
						return getAttributeDeclaration(myBaseType,theAttributeName);
					} else {
						throw Schema::UnknownBaseType(string("Base type '")+myBase->nodeValue()+"' not found, referenced in "
							+asl::as_string(*myContent),
							"dom::Schema::getAttributeDeclaration for attribute '"+theAttributeName+"'");
					}
				}
			}
		}
	}
	return NodePtr(0);
}

const NodePtr 
dom::Schema::getAttributeType(const NodePtr myAttributeDeclaration) const {
	DB(AC_TRACE<<"getAttributeType: myAttributeDeclaration="<<*myAttributeDeclaration<< endl;)
	// check for type name attribute first
	NodePtr myTypeNameAttribute = myAttributeDeclaration->getAttribute(ATTR_TYPE);
	if (myTypeNameAttribute) {
		DB(AC_TRACE<<"getAttributeType: findTopLevelTypeDeclaration="<<myTypeNameAttribute->nodeValue()<< endl;)
		NodePtr myType = findTopLevelTypeDeclaration(myTypeNameAttribute->nodeValue());
		if (myType) {
			return myType;
		}
		return getBuiltinSchema().findTopLevelTypeDeclaration(myTypeNameAttribute->nodeValue());
	}
	// check if this attribute declaration references a global one
	NodePtr myTypeNameRef = myAttributeDeclaration->getAttribute(ATTR_REF);
	if (myTypeNameRef) {
		NodePtr myReferencedAttrDecl = findTopLevelAttributeDeclaration(myTypeNameRef->nodeValue());
		return getAttributeType(myReferencedAttrDecl);
	}
	// now the type can be only a child <simpleType> 
	return myAttributeDeclaration->childNode(XS_SIMPLETYPE);
}

