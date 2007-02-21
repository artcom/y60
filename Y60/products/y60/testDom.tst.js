/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testDom.tst.js,v $
//
//   $Revision: 1.9 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function NodeUnitTest() {
    this.Constructor(this, "NodeUnitTest");
};

NodeUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
		DPRINT('Node');
		DPRINT('Node.ELEMENT_NODE');
		ENSURE('Node.ELEMENT_NODE == 1');
		ENSURE('Node.DOCUMENT_NODE == 9');
		obj.myNode =  Node.createElement("bla");
		DPRINT('obj.myNode');
		ENSURE('obj.myNode.nodeType == Node.ELEMENT_NODE');
		ENSURE('obj.myNode.nodeName == "bla"');

		obj.myNode = new Node("<hello>world</hello>");
		ENSURE('obj.myNode.nodeType == Node.DOCUMENT_NODE');
		DPRINT('obj.myNode');
		ENSURE('obj.myNode.firstChild.nodeName == "hello"');
		ENSURE('obj.myNode.firstChild.firstChild.nodeName == "#text"');
		ENSURE('obj.myNode.firstChild.firstChild.nodeValue == "world"');
		ENSURE('obj.myNode.lastChild.nodeName == "hello"');
		ENSURE('obj.myNode.firstChild.firstChild.parentNode.nodeName == "hello"');
		ENSURE('obj.myNode.firstChild.hasChildNodes()');
		obj.myNode = new Node("<hello><world1/><world2/></hello>");
		ENSURE('obj.myNode.nodeType == Node.DOCUMENT_NODE');
		ENSURE('obj.myNode.firstChild.firstChild.nextSibling.nodeName == "world2"');
		ENSURE('obj.myNode.firstChild.lastChild.previousSibling.nodeName == "world1"');

		ENSURE('obj.myNode.isSameNode(obj.myNode)');
		ENSURE('!obj.myNode.isSameNode(new Node())');
		ENSURE('obj.myNode.firstChild.isSameNode(obj.myNode.lastChild)');

		// childNode
		ENSURE('obj.myNode.firstChild.isSameNode(obj.myNode.childNode(0))');
		ENSURE('obj.myNode.firstChild.isSameNode(obj.myNode.childNode("hello"))');
		ENSURE('obj.myNode.firstChild.isSameNode(obj.myNode.childNode("hello",0))');
		DPRINT('obj.myNode.childNode("hello",1)');
		ENSURE('obj.myNode.childNode("hello",1) == undefined');
		ENSURE('obj.myNode.firstChild.lastChild.isSameNode(obj.myNode.childNode("hello").childNode("world2"))');


		obj.myNodeList = obj.myNode.childNodes;
		DPRINT('obj.myNodeList');
		ENSURE('obj.myNodeList.length == 1');
		ENSURE('obj.myNodeList[0].isSameNode(obj.myNode.firstChild)');
		ENSURE('obj.myNodeList.item(0).isSameNode(obj.myNode.firstChild)');

		obj.myNodeList = obj.myNode.firstChild.childNodes;
		ENSURE('obj.myNodeList.length == 2');
		ENSURE('obj.myNodeList[0].isSameNode(obj.myNode.firstChild.firstChild)');
		ENSURE('obj.myNodeList.item(0).isSameNode(obj.myNode.firstChild.firstChild)');

		// clone
		obj.myNode = new Node("<hello><world1/><world2/></hello>");
		obj.myClone = obj.myNode.cloneNode(true);
		ENSURE('obj.myClone.firstChild.nodeName == "hello"');
		ENSURE('obj.myClone.firstChild.firstChild.nodeName == "world1"');
		ENSURE('!obj.myClone.firstChild.isSameNode(obj.myNode.firstChild)');

		// shallow clone
		obj.myClone = obj.myNode.firstChild.cloneNode(false);
		ENSURE('obj.myClone.nodeName == "hello"');
		ENSURE('obj.myClone.childNodes.length == 0');

		obj.myDocument = Node.createDocument();
		obj.myDocument.parse("<hello>world</hello>");
		DPRINT('obj.myDocument');
		obj.myNewNode = new Node(obj.myDocument);

		// attribute access
		obj.myDocument = new Node('<elem attrname="attrvalue"/>');
		ENSURE('myDocument.firstChild.nodeName == "elem"');
		ENSURE('myDocument.firstChild.getAttribute("attrname") == "attrvalue"');
		ENSURE('myDocument.firstChild.getAttribute("noattrname") == undefined');
		ENSURE('myDocument.firstChild.attrname == "attrvalue"');
		ENSURE('myDocument.firstChild.noattrname == undefined');

		obj.myTestObj = new Object();
		obj.myTestObj.attrname = new Object();
		ENSURE('"attrname" in myTestObj');
		ENSURE('!("noattrname" in myTestObj)');

		obj.myNode = obj.myDocument.firstChild;
		ENSURE('"attrname" in myNode');
		ENSURE('!("noattrname" in myNode)');

		obj.myNode.attrname = 'newvalue';
		ENSURE('myNode.attrname == "newvalue"');
		ENSURE('myDocument.firstChild.attrname == "newvalue"');

		// append child
		obj.myNode = new Node("<hello/>").firstChild;
		obj.myNewNode = obj.myNode.appendChild(new Node("<world/>").firstChild);
		ENSURE('obj.myNewNode.nodeName == "world"');
		ENSURE('obj.myNode.childNodes.length == 1');
		ENSURE('obj.myNode.firstChild.nodeName == "world"');

		// insert before
		obj.myNewNode = obj.myNode.insertBefore(new Node("<new/>").firstChild, obj.myNode.lastChild);
		ENSURE('obj.myNewNode.nodeName == "new"');
		ENSURE('obj.myNode.childNodes.length == 2');
		ENSURE('obj.myNode.firstChild.nodeName == "new"');
		obj.myNewNode = obj.myNode.appendChild(new Node("<order/>").firstChild);
		ENSURE('obj.myNewNode.nodeName == "order"');
		ENSURE('obj.myNode.childNodes.length == 3');
		ENSURE('obj.myNode.lastChild.nodeName == "order"');

		// remove child
		obj.myNewNode = obj.myNode.removeChild(obj.myNode.childNode(1));
        ENSURE('obj.myNewNode.nodeName == "world"');
		ENSURE('obj.myNode.childNodes.length == 2');
		ENSURE('obj.myNode.firstChild.nodeName == "new"');
		ENSURE('obj.myNode.lastChild.nodeName == "order"');
    }
}

function SchemaUnitTest() {
    this.Constructor(this, "SchemaUnitTest");
}

SchemaUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
    //TODO: repair cast to bool

		obj.myDocument = new Node();
		ENSURE('!myDocument.ok');
		obj.myDocument = new Node("");
		ENSURE('!myDocument.ok');

		DTITLE("Starting 2nd Schema tests");
		obj.mySchema = new Node(
			"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
			+"   <xs:simpleType name='Vector3f'>\n"
			+"       <xs:restriction base='xs:string' />\n"
			+"   </xs:simpleType>\n"
			+"	<xs:element name='scene'>\n"
			+"		<xs:complexType>\n"
			+"			<xs:sequence>\n"
			+"				<xs:element name='shapes' type='xs:int'/>\n"
			+"				<xs:element name='materials' type='xs:int'/>\n"
			+"				<xs:element ref='worlds'/>\n"
			+"				<xs:element ref='uvset'/>\n"
			+"				<xs:element name='newchild' type='xs:int'/>\n"
			+"			</xs:sequence>\n"
			+"			<xs:attribute name='version' type='xs:unsignedLong'/>\n"
			+"			<xs:attribute name='newattr' type='xs:unsignedLong'/>\n"
			+"			<xs:attribute name='size' type='Vector3f'/>\n"
			+"		</xs:complexType>\n"
			+"	</xs:element>\n"
            +"	<xs:element name='worlds'>\n"
            +"		<xs:complexType>\n"
            +"			<xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
            +"				<xs:element ref='world' />\n"
            +"			</xs:sequence>\n"
            +"		</xs:complexType>\n"
            +"	</xs:element>\n"
            +"	<xs:element name='world'>\n"
            +"		<xs:complexType>\n"
            +"			<xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
//                        +"				<xs:element name='node' type='xs:hexBinary'/>\n"
            +"				<xs:element name='node' type='xs:string'/>\n"
            +"			</xs:sequence>\n"
            +"			<xs:attribute name='id' type='xs:int' />\n"
            +"			<xs:attribute name='name' type='xs:string' />\n"
            +"		</xs:complexType>\n"
            +"	</xs:element>\n"
            +"	<xs:element name='uvset'>\n"
            +"		<xs:complexType>\n"
            +"			<xs:simpleContent>\n"
            +"				<xs:extension base='xs:int'>\n"
            +"			        <xs:attribute name='name' type='xs:ID' />\n"
            +"			    </xs:extension>\n"
            +"			</xs:simpleContent>\n"
            +"		</xs:complexType>\n"
            +"	</xs:element>\n"
            +"</xs:schema>\n"
		);
		ENSURE('mySchema.ok');
		obj.myDocument = Node.createDocument();
		obj.myDocument.useFactories("w3c-schema,som");
		obj.myDocument.addSchema(obj.mySchema,"");
		SUCCESS("added Schema");
		DPRINT('obj.myDocument.schema');
		obj.myDocument.parse(
			"<scene version='214' size='[1,2,3]'>"
			+"	<shapes>12345</shapes>\n"
			+"	<materials/>\n"
			+"	<worlds></worlds>\n"
            +"   <uvset name='bla'>23</uvset>\n"
			+"</scene>\n"
			);
		ENSURE('myDocument.ok');
		//myDocument.getSchema()->dump();
		ENSURE('myDocument.firstChild.nodeName == "scene"');
		ENSURE('myDocument.firstChild.firstChild.nodeName == "shapes"');
		ENSURE('myDocument.firstChild.firstChild.firstChild.nodeName == "#text"');
		ENSURE('myDocument.firstChild.firstChild.firstChild.nodeValue == "12345"');
		ENSURE('myDocument.firstChild.firstChild.firstChild.nodeValue === 12345');
		DPRINT('myDocument.firstChild.firstChild.firstChild.nodeValueTypeName');
		ENSURE('myDocument.firstChild.firstChild.firstChild.nodeValueTypeName == "int"');

		DPRINT('obj.myDocument.firstChild.version');
		ENSURE('myDocument.firstChild.version == 214');
		obj.myDocument.firstChild.version = 215;
		ENSURE('myDocument.firstChild.version == 215');

		obj.myScene = obj.myDocument.childNode('scene');
		ENSURE('myScene.nodeName == "scene"');
		ENSURE('almostEqual(myScene.size,new Vector3f(1,2,3))');
		obj.mySceneSize = obj.myScene.size;
		obj.mySceneSize.value = new Vector3f(2,3,4);
		ENSURE('almostEqual(mySceneSize,new Vector3f(2,3,4))');
		ENSURE('almostEqual(myScene.size,new Vector3f(2,3,4))');
	}
}

function DomParseUnitTest() {
    this.Constructor(this, "DomParseUnitTest");
}

DomParseUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);


    obj.run = function() {
    	obj.myDocument = Node.createDocument();

        // Test parsing of quoted characters
    	obj.myDocument.parse('<hello myattr="&lt;p&gt;">world&lt;p&gt;</hello>');
        ENSURE('obj.myDocument.firstChild.nodeName == "hello"');
        ENSURE('obj.myDocument.firstChild.myattr == "<p>"');
        ENSURE('obj.myDocument.firstChild.firstChild.nodeValue == "world<p>"');

        // Test file saving
        var myDate = new Date();
        obj.myDateString = myDate.toLocaleString();
        obj.mySaveDoc = Node.createDocument();
        obj.mySaveDoc.parse('<date>' + obj.myDateString + '</date>');
        obj.mySaveDoc.saveFile("../../testfiles/output.xml");
        obj.myLoadDoc = Node.createDocument();
        obj.myLoadDoc.parseFile("../../testfiles/output.xml");
        ENSURE('obj.myLoadDoc.firstChild.firstChild.nodeValue == obj.myDateString');

        var myXMLDoc = new Node();
        myXMLDoc.parseFile("../../testfiles/unicode.xml");
        var myXMLNode = myXMLDoc.firstChild;

        var myUnicode = myXMLNode.firstChild.firstChild.firstChild.nodeValue;
    }
}

function DomEventTest() {
    this.Constructor(this, "DomEventTest");
}

DomEventTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
    	obj.myDocument = Node.createDocument();
    	obj.myDocument.parse('<test name="root">\
    	                      <child1 name="child1">\
    	                      <child2 name="child2">\
    	                      <child3 name="child3"></child3></child2></child1>\
    	                      </test>');
    	var myChildNode1 = obj.myDocument.childNode(0).childNode(0);
    	var myChildNode2 = myChildNode1.childNode(0);
    	var myChildNode3 = myChildNode2.childNode(0);
    	var myVector = new Vector3f(0,1,2);
    	var myDomEvent = new JSEvent("testEvent", myVector, true, false, 0.0);
    	//ENSURE = print;
    	ENSURE(myDomEvent.stopPropagation());
    	ENSURE(myDomEvent.preventDefault());
    	ENSURE(myDomEvent.type == "testEvent");
    	ENSURE(myDomEvent.eventPhase == 1);
    	ENSURE(myDomEvent.bubbles == true);
    	ENSURE(myDomEvent.cancelable == false);
    	ENSURE(myDomEvent.isDefaultPrevented == true);

    	myChildNode2.addEventListener("testEvent", obj, true);
    	myChildNode3.dispatchEvent(myDomEvent);
    }

    obj.handleEvent = function(theEvent) {
        print("eventListener: " + theEvent);
    }
}

var myTestName = "testDOM.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new DomEventTest());
mySuite.addTest(new DomParseUnitTest());
mySuite.addTest(new SchemaUnitTest());
mySuite.addTest(new NodeUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
