/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testVectorOfString.tst.js,v $
//
//   $Revision: 1.4 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
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
			+"	<xs:element name='root'>\n"
			+"		<xs:complexType>\n"
			+"			<xs:sequence>\n"
			+"				<xs:element ref='vofs'/>\n"
			+"				<xs:element ref='vofi'/>\n"
			+"				<xs:element ref='vofv2f'/>\n"
			+"			</xs:sequence>\n"
			+"		</xs:complexType>\n"
			+"	</xs:element>\n"
            +"	<xs:element name='vofs'>\n"
            +"		<xs:complexType>\n"
            +"	            <xs:attribute name='foo' type='VectorOfString' />\n"
            +"		</xs:complexType>\n"
            +"	</xs:element>\n"
            +"	<xs:element name='vofi'>\n"
            +"		<xs:complexType>\n"
            +"	            <xs:attribute name='foo' type='VectorOfUnsignedInt' />\n"
            +"		</xs:complexType>\n"
            +"	</xs:element>\n"
            +"	<xs:element name='vofv2f'>\n"
            +"		<xs:complexType>\n"
            +"	            <xs:attribute name='foo' type='VectorOfVector2f' />\n"
            +"		</xs:complexType>\n"
            +"	</xs:element>\n"
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
			+"	<vofi foo='[1,2,3]'/>\n"
			+"	<vofs foo='[one,two,three]'/>\n"
			+"	<vofv2f foo='[[0.1,0.2], [2,3]]'/>\n"
			+"</root>\n"
			);
        obj.myVectorOfInt = obj.myDocument.firstChild.childNode(0).foo;
        obj.myVectorOfString = obj.myDocument.firstChild.childNode(1).foo;
        obj.myVectorOfVector2f = obj.myDocument.firstChild.childNode(2).foo;
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


