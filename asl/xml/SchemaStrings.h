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
//    $RCSfile: SchemaStrings.h,v $
//
//   $Revision: 1.3 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_SchemaStrings_h_included_
#define _xml_SchemaStrings_h_included_

#include "typedefs.h"
#include <string>

namespace dom {

/*! \addtogroup aslxml */
/* @{ */

const String XSI_SCHEMALOCATION("xsi:schemaLocation");
const String XSI_NONAMESPACESCHEMALOCATION("xsi:noNamespaceSchemaLocation");
const String XMLNS("xmlns:");

const String XS_SCHEMA("xs:schema");
const String XS_ELEMENT("xs:element");
const String XS_ATTRIBUTE("xs:attribute");
const String XS_SEQUENCE("xs:sequence");
const String XS_CHOICE("xs:choice");
const String XS_GROUP("xs:group");
const String XS_EXTENSION("xs:extension");
const String XS_RESTRICTION("xs:restriction");
const String XS_COMPLEXTYPE("xs:complexType");
const String XS_SIMPLETYPE("xs:simpleType");
const String XS_COMPLEXCONTENT("xs:complexContent");
const String XS_SIMPLECONTENT("xs:simpleContent");

const String ATTR_TYPE("type");
const String ATTR_NAME("name");
const String ATTR_REF("ref");
const String ATTR_BASE("base");
const String ATTR_SUBSTITUTIONGROUP("substitutionGroup");

const String COLON(":");

/* @} */

} // namespace dom
#endif

