/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

*/

use("UnitTest.js");

function VectorOfStringTest() {
    this.Constructor(this, "VectorOfStringTest");
}

VectorOfStringTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.mySchema = new Node(
            "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
            +"   <xs:simpleType name='VectorOfUnsignedInt'>\n"
            +"       <xs:restriction base='xs:string' />\n"
            +"   </xs:simpleType>\n"
            +"   <xs:simpleType name='VectorOfString'>\n"
            +"       <xs:restriction base='xs:string' />\n"
            +"   </xs:simpleType>\n"
            +"   <xs:simpleType name='VectorOfVector2f'>\n"
            +"       <xs:restriction base='xs:string' />\n"
            +"   </xs:simpleType>\n"
            +"    <xs:element name='root'>\n"
            +"        <xs:complexType>\n"
            +"            <xs:sequence>\n"
            +"                <xs:element ref='vofs'/>\n"
            +"                <xs:element ref='vofi'/>\n"
            +"                <xs:element ref='vofv2f'/>\n"
            +"            </xs:sequence>\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"    <xs:element name='vofs'>\n"
            +"        <xs:complexType>\n"
            +"                <xs:attribute name='foo' type='VectorOfString' />\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"    <xs:element name='vofi'>\n"
            +"        <xs:complexType>\n"
            +"                <xs:attribute name='foo' type='VectorOfUnsignedInt' />\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"    <xs:element name='vofv2f'>\n"
            +"        <xs:complexType>\n"
            +"                <xs:attribute name='foo' type='VectorOfVector2f' />\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"</xs:schema>\n"
        );
        /*
        <xs:complexType>
            <xs:simpleContent>
                <xs:extension base="VectorOfVector2f">
                    <xs:attribute name="name" type="xs:string"/>
                </xs:extension>
            </xs:simpleContent>
        </xs:complexType>
        */
        ENSURE('mySchema.ok');
        obj.myDocument = Node.createDocument();
        obj.myDocument.useFactories("w3c-schema,som");
        obj.myDocument.addSchema(obj.mySchema,"");
        SUCCESS("added Schema");
        obj.myDocument.parse(
            "<root>"
            +"    <vofi foo=\"[1,2,3]\"/>\n"
            +"    <vofs foo=\"[`one`,`two`,`three`]\"/>\n"
            +"    <vofv2f foo=\"[[0.1,0.2], [2,3]]\"/>\n"
            +"    <vofs foo=\"[]\"/>\n"
            +"    <vofs foo=\"[``,`23`,`foo`,``,`bar`,``]\"/>\n"
            +"</root>\n"
            );
        obj.myVectorOfInt = obj.myDocument.firstChild.childNode(0).foo;
        obj.myVectorOfString = obj.myDocument.firstChild.childNode(1).foo;
        obj.myVectorOfVector2f = obj.myDocument.firstChild.childNode(2).foo;
        obj.myEmptyVectorOfString = obj.myDocument.firstChild.childNode(3).foo;
        obj.myVectorWithEmptyString = obj.myDocument.firstChild.childNode(4).foo;


        ENSURE('obj.myVectorOfInt[0] == 1');
        ENSURE('obj.myVectorOfInt[1] == 2');
        ENSURE('obj.myVectorOfInt[2] == 3');
        obj.myVectorOfInt[1] = 17;
        ENSURE('obj.myVectorOfInt[1] == 17');
        obj.myVectorOfInt[1] = 17.8;
        DPRINT('"### WARN we get an automatic float to int conversion here, type safety ? "');
        ENSURE('obj.myVectorOfInt[1] == 17');

        ENSURE('almostEqual(obj.myVectorOfVector2f[0],new Vector2f(0.1,0.2) )');
        obj.myVectorOfVector2f[1] = new Vector2f(0.2,0.4);
        ENSURE('almostEqual(obj.myVectorOfVector2f[1],new Vector2f(0.2,0.4) )');

        ENSURE('obj.myVectorOfString[0] == "one"');
        ENSURE('obj.myVectorOfString[1] == "two"');
        ENSURE('obj.myVectorOfString[2] == "three"');
        obj.myVectorOfString[1] = "zwei";
        ENSURE('obj.myVectorOfString[1] == "zwei"');

        ENSURE('obj.myEmptyVectorOfString.length == 0');

        ENSURE('obj.myVectorWithEmptyString.length == 6');
        ENSURE('obj.myVectorWithEmptyString[0] == ""');
        ENSURE('obj.myVectorWithEmptyString[4] == "bar"');
        ENSURE('obj.myVectorWithEmptyString[5] == ""');

        // test JS setProperty
        obj.myVectorOfString = ["foo","bar"];
        ENSURE('obj.myVectorOfString[0] == "foo"');
        ENSURE('obj.myVectorOfString[1] == "bar"');
        ENSURE('obj.myVectorOfString.length == 2');
    }
}

function main() {
    var myTestName = "testVectorOfString.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new VectorOfStringTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
};


