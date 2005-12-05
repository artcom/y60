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
//    $RCSfile: Schema.h,v $
//
//   $Revision: 1.6 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_Schema_h_included_
#define _xml_Schema_h_included_

#include "Nodes.h"
#include "SchemaStrings.h"

#include <map>
#include <set>

/*
#include <asl/settings.h>
#include <asl/Ptr.h>
#include <asl/Exception.h>
#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Block.h>
#include <asl/MemoryPool.h>

#include <string>
#include <vector>
#include <iostream>

#include <typeinfo>


#define DBE(x) x
*/
namespace dom {

/*! \addtogroup aslxml */
/* @{ */

class Schema : public DocumentFragment {
	public:
		DEFINE_NESTED_EXCEPTION(dom::Schema,Exception,asl::Exception);
		DEFINE_NESTED_EXCEPTION(dom::Schema,DuplicateDeclaration,Exception);
		//DEFINE_NESTED_EXCEPTION(dom::Schema,CantResolveReference,Exception);
		DEFINE_NESTED_EXCEPTION(dom::Schema,UnknownBaseType,Exception);
		DEFINE_NESTED_EXCEPTION(dom::Schema,ValueTypeMismatch,Exception);
		DEFINE_NESTED_EXCEPTION(dom::Schema,NotAvailable,Exception);
		DEFINE_NESTED_EXCEPTION(dom::Schema,NotInFactory,Exception);

		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,ParseException,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,UndeclaredElement,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,InternalError,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,ElementWithoutType,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,TypeWithoutName,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,TextWithoutType,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,ElementNotAllowed,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,TextNotAllowed,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,TextWithUndeclaredType,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,ChoiceSequenceGroupConflict,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,ExtensionOrRestrictionRequired,dom::ParseException);

		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,UndeclaredAttribute,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,AttributeWithoutType,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,MalformedDeclaration,dom::ParseException);
		DEFINE_NESTED_PARSE_EXCEPTION(dom::Schema,CantResolveReference,dom::ParseException);

		Schema() {};
		Schema(const DOMString & xml, const DOMString & theNSPrefix="");

		//
		const NodePtr getAttributeDeclaration(const NodePtr theElementType, const DOMString & theAttributeName) const;

		// return a type name for a given xs:attribute node
		const NodePtr getAttributeType(const NodePtr myAttributeDeclaration) const;
	
		const NodePtr findElementDeclaration(const DOMString & theParentElementName,
									const NodePtr theParentElementType,
									const DOMString & theElementName,
									int theParsePos);
		const NodePtr tryFindElementDeclaration(const DOMString & theParentElementName,
									const NodePtr theParentElementType,
									const DOMString & theElementName,
									int theParsePos);

		const NodePtr findTopLevelElementDeclaration(const DOMString & theName) const;
		const NodePtr findTopLevelGroupDeclaration(const DOMString & theName) const;
		const NodePtr findTopLevelTypeDeclaration(const DOMString & theName) const;
		const NodePtr findTopLevelAttributeDeclaration(const DOMString & theName) const;
		dom::NodePtr addDocument(const Node & theDocument, const DOMString & theNSPrefix);
		void preprocess(const NodePtr theNewBranch, const DOMString & theNSPrefix);
		void check() const;
		void dump() const;

		typedef std::map<DOMString,const NodePtr> NodeMap;
		typedef std::set<DOMString> NameSet;
		typedef std::map<DOMString,NameSet > NameSetMap;

		typedef std::map<DOMString,NodePtr> ElementMap;
		typedef std::map<DOMString,ElementMap> ElementIDMaps;

		bool substitutionsGroupContains(const DOMString & theGroup, const DOMString & theName) const {
			NameSetMap::const_iterator myGroup = _mySubstitutionGroups.find(theGroup);
			if (myGroup != _mySubstitutionGroups.end()) {
				return myGroup->second.find(theName) != myGroup->second.end();
			}
			return false;
		}
		bool substitutionsGroupExists(const DOMString & theGroup) const {
			NameSetMap::const_iterator myGroup = _mySubstitutionGroups.find(theGroup);
			if (myGroup != _mySubstitutionGroups.end()) {
				return true;
			}
			return false;
		}
        NodePtr findType(NodePtr theSchemaDeclaration, unsigned long theParsePos);

		static const dom::Schema & getBuiltinSchema();
	private:
		void collectDeclarations(const NodePtr theNode,
							const DOMString & theNodeName,
							const DOMString & theAttributeName,
							int theDepth,
							int theMaxDepth,
							NodeMap & theResults,
							const DOMString & theNSPrefix);
		void collectSubstitutionGroups(const NodePtr theNode,
							const DOMString & theNodeName,
							const DOMString & theGroupAttributeName,
							const DOMString & theNameAttributeName,
							int theDepth,
							int theMaxDepth,
							NameSetMap & theResults,
							const DOMString & theNSPrefix);
		void checkDeclarationReferences(const NodePtr theNode,
							const DOMString & theElementName,
							const DOMString & theAttributeName,
							int theDepth,
							int theMaxDepth,
							const NodeMap & theLookupMap,
							const NodeMap * theSystemLookupMap) const;

        void collectElementDeclarations(const NodePtr theNode);
		NodeMap _myTopLevelElementDeclarations;
		NodeMap _myTopLevelTypeDeclarations;
		NodeMap _myTopLevelAttributeDeclarations;
		NodeMap _myTopLevelGroupDeclarations;
		NameSetMap _mySubstitutionGroups;
	};

    /* @} */
} //Namespace dom



#endif
