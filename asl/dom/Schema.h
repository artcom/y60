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

#include "asl_dom_settings.h"

#include "Nodes.h"
#include "SchemaStrings.h"

#include <map>
#include <set>

/*
#include <asl/base/settings.h>
#include <asl/base/Ptr.h>
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Block.h>
#include <asl/base/MemoryPool.h>

#include <string>
#include <vector>
#include <iostream>

#include <typeinfo>


#define DBE(x) x
*/
namespace dom {

/*! \addtogroup aslxml */
/* @{ */

class ASL_DOM_DECL Schema : public DocumentFragment {
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

        bool checkSchemaRestriction(const NodePtr theParentElementType,
                                    const NodePtr theSchemaDeclaration, 
                                    const Node* theElement);
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
#ifdef check
	#ifndef _SETTING_NO_UNDEF_WARNING_
		#warning Symbol 'check' defined as macro, undefining. (Outrageous namespace pollution by Apples AssertMacros.h, revealing arrogance and incompetence)
	#endif
	#undef check
#endif
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
