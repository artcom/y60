/* ==========================================================================
//
// Copyright (C) 1993-2001, ART+COM GmbH Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
/* ==========================================================================
//
// !!!!!!! AUTOMATICALLY GENERATED FILE - ONLY PERFORM CHANGES IN THE !!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!! CORRESPONDING XSD FILE !!!!!!!!!!!!!!!!!!!!!!!!!
*/
/* ==========================================================================
*/
#include <string>
std::string oureventxsd = std::string(
"<?xml version='1.0' encoding='utf-8' ?>\n"
"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>\n"
"    \n"
"    <xs:simpleType name='Vector3i'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:element name='generic'>\n"
"        <xs:complexType>\n"
"            <xs:attribute name='when' type='xs:double'/>\n"
"            <xs:attribute name='simulation_time' type='xs:double'/>\n"
"            <xs:attribute name='callback' type='xs:string'/>\n"
"            <xs:attribute name='id' type='xs:ID'/>\n"
"            <xs:attribute name='type' type='xs:string'/>\n"
"            <xs:attribute name='motiondata' type='Vector3i'/>\n"
"            <xs:attribute name='buttonname' type='xs:string'/>\n"
"            <xs:attribute name='pressed' type='xs:boolean'/>\n"
"         </xs:complexType>\n"
"    </xs:element>\n"
"</xs:schema>\n"
);
