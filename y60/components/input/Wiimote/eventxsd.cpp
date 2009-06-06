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
"    <xs:simpleType name='Vector3f'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:simpleType name='Vector2i'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:simpleType name='Vector2f'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:element name='generic'>\n"
"        <xs:complexType>\n"
"            <xs:attribute name='when' type='xs:double'/>\n"
"            <xs:attribute name='simulation_time' type='xs:double'/>\n"
"            <xs:attribute name='callback' type='xs:string'/>\n"
"            <xs:attribute name='id' type='xs:ID'/>\n"
"            <xs:attribute name='type' type='xs:string'/>\n"
"            <xs:attribute name='motiondata' type='Vector3f'/>\n"
"            <xs:attribute name='buttonname' type='xs:string'/>\n"
"            <xs:attribute name='pressed' type='xs:boolean'/>\n"
"            <xs:attribute name='irposition0' type='Vector2i'/>\n"
"            <xs:attribute name='irposition1' type='Vector2i'/>\n"
"            <xs:attribute name='irposition2' type='Vector2i'/>\n"
"            <xs:attribute name='irposition3' type='Vector2i'/>\n"
"            <xs:attribute name='screenposition' type='Vector2f'/>\n"
"            <xs:attribute name='angle' type='xs:float'/>\n"
"\n"
"            <!-- Status Report Attributes -->\n"
"            <xs:attribute name='battery_level'     type='xs:float'/>\n"
"            <xs:attribute name='extension'         type='xs:boolean'/>\n"
"            <xs:attribute name='speaker_enabled'   type='xs:boolean'/>\n"
"            <xs:attribute name='continous_reports' type='xs:boolean'/>\n"
"            <xs:attribute name='led0'              type='xs:boolean'/>\n"
"            <xs:attribute name='led1'              type='xs:boolean'/>\n"
"            <xs:attribute name='led2'              type='xs:boolean'/>\n"
"            <xs:attribute name='led3'              type='xs:boolean'/>\n"
"        </xs:complexType>\n"
"    </xs:element>\n"
"</xs:schema>\n"
);
