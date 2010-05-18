/* ==========================================================================
//
// Copyright (C) 1993-2001, ART+COM GmbH Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// ==========================================================================
//
// XXX currently not regenerated automatically
// TODO add asl_add_schema magic to CMakeLists.txt
//
// ==========================================================================
*/

#include "y60/base/Y60xsd.h"
#include <string>

std::string ourosceventxsd = std::string(
"<?xml version='1.0' encoding='utf-8' ?>\n"
"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>\n"
"    <xs:element name='generic'>\n"
"        <xs:complexType>\n"
"            <xs:attribute name='when' type='xs:int'/>\n"
"            <xs:attribute name='callback' type='xs:string'/>\n"
"            <xs:attribute name='type' type='xs:string'/> \n"
"            <xs:attribute name='sender' type='xs:string'/> \n"
"            <xs:attribute name='simulation_time' type='xs:double'/>\n"
"            <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
"                <xs:element ref='float' />\n"
"                <xs:element ref='string' />\n"
"                <xs:element ref='int' />\n"
"                <xs:element ref='double' />\n"
"                <xs:element ref='bool' />\n"
"            </xs:sequence>\n"
"        </xs:complexType>\n"
"    </xs:element>\n"
"    <xs:element name='float'>\n"
"        <xs:complexType>\n"
"            <xs:simpleContent>\n"
"                <xs:extension base='xs:float'>\n"
"                   </xs:extension>\n"
"            </xs:simpleContent>\n"
"        </xs:complexType>\n"
"    </xs:element>      \n"
"    <xs:element name='string'>\n"
"        <xs:complexType>\n"
"            <xs:simpleContent>\n"
"                <xs:extension base='xs:string'>\n"
"                   </xs:extension>\n"
"            </xs:simpleContent>\n"
"        </xs:complexType>\n"
"    </xs:element>      \n"
"    <xs:element name='int'>\n"
"        <xs:complexType>\n"
"            <xs:simpleContent>\n"
"                <xs:extension base='xs:int'>\n"
"                   </xs:extension>\n"
"            </xs:simpleContent>\n"
"        </xs:complexType>\n"
"    </xs:element>      \n"
"    <xs:element name='double'>\n"
"        <xs:complexType>\n"
"            <xs:simpleContent>\n"
"                <xs:extension base='xs:double'>\n"
"                   </xs:extension>\n"
"            </xs:simpleContent>\n"
"        </xs:complexType>\n"
"    </xs:element>      \n"
"    <xs:element name='bool'>\n"
"        <xs:complexType>\n"
"            <xs:simpleContent>\n"
"                <xs:extension base='xs:bool'>\n"
"                   </xs:extension>\n"
"            </xs:simpleContent>\n"
"        </xs:complexType>\n"
"    </xs:element>      \n"
"\n"
"</xs:schema>\n"
);
