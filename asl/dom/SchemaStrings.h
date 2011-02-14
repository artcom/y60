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

#include "asl_dom_settings.h"

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
const String MAXOCCURS_NAME("maxOccurs");
const String UNBOUNDED_MAXOCCURS("unbounded");
const String ATTR_BASE("base");
const String ATTR_SUBSTITUTIONGROUP("substitutionGroup");

const String COLON(":");

/* @} */

} // namespace dom
#endif

