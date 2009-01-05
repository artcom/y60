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
std::string ourasseventxsd = std::string(
"<?xml version='1.0' encoding='utf-8' ?>\n"
"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>\n"
"    <!-- example nodes\n"
"         <generic when='1.17026e+09'\n"
"                  callback='onASSEvent'\n"
"                  id='24'\n"
"                  type='configure'\n"
"                  grid_size='[20,10]' />\n"
"\n"
"         <generic when='1.17026e+09'\n"
"                  callback='onASSEvent'\n"
"                  id='23'\n"
"                  type='add'\n"
"                  raw_position='[9.22574,7.24158]'\n"
"                  position3D='[44.8317,-15.4852,0]'\n"
"                  roi='[[0,0][1,1]]'/>\n"
"\n"
"         <generic when='1.17026e+09'\n"
"                  callback='onASSEvent'\n"
"                  id='23'\n"
"                  type='move'\n"
"                  raw_position='[9.22574,7.24158]'\n"
"                  position3D='[44.8317,-15.4852,0]'\n"
"                  roi='[[0,0][1,1]]'/>\n"
"\n"
"         <generic when='1.17026e+09'\n"
"                  callback='onASSEvent'\n"
"                  id='23'\n"
"                  type='remove'\n"
"                  raw_position='[9.22574,7.24158]'\n"
"                  position3D='[44.8317,-15.4852,0]'\n"
"                  roi='[[0,0][1,1]]'/>\n"
"    -->\n"
"    <xs:simpleType name='Vector3f'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:simpleType name='Vector2f'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:simpleType name='Vector2i'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:simpleType name='Box2f'>\n"
"        <xs:restriction base='xs:string'/>\n"
"    </xs:simpleType>\n"
"    <xs:element name='generic'>\n"
"        <xs:complexType>\n"
"            <xs:attribute name='when' type='xs:double'/>\n"
"            <xs:attribute name='simulation_time' type='xs:double'/>\n"
"            <xs:attribute name='callback' type='xs:string'/>\n"
"            <xs:attribute name='id' type='xs:ID'/>\n"
"            <xs:attribute name='type' type='xs:string'/>\n"
"            <xs:attribute name='raw_position' type='Vector2f'/>\n"
"            <xs:attribute name='position3D' type='Vector3f'/>\n"
"            <xs:attribute name='grid_size' type='Vector2i'/>\n"
"            <xs:attribute name='roi' type='Box2f'/>\n"
"            <xs:attribute name='intensity' type='xs:double'/>\n"
"            <xs:attribute name='frameno' type='xs:int'/>\n"
"        </xs:complexType>\n"
"    </xs:element>\n"
"\n"
"    <!-- This node can be used by clients to store multiple cursors -->\n"
"    <xs:element name='cursors'>\n"
"        <xs:complexType>\n"
"            <xs:sequence>\n"
"                <xs:element ref='generic' />\n"
"            </xs:sequence>\n"
"        </xs:complexType>\n"
"    </xs:element>\n"
"</xs:schema>\n"
);
