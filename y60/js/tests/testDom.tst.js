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

const XMLDOC_1 = "testDom_tmp_output.xml";
const XMLDOC_2 = searchFile("fixtures/unicode.xml");

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
            +"    <xs:element name='scene'>\n"
            +"        <xs:complexType>\n"
            +"            <xs:sequence>\n"
            +"                <xs:element name='shapes' type='xs:int'/>\n"
            +"                <xs:element name='materials' type='xs:int'/>\n"
            +"                <xs:element ref='worlds'/>\n"
            +"                <xs:element ref='uvset'/>\n"
            +"                <xs:element name='newchild' type='xs:int'/>\n"
            +"            </xs:sequence>\n"
            +"            <xs:attribute name='version' type='xs:unsignedLong'/>\n"
            +"            <xs:attribute name='newattr' type='xs:unsignedLong'/>\n"
            +"            <xs:attribute name='size' type='Vector3f'/>\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"    <xs:element name='worlds'>\n"
            +"        <xs:complexType>\n"
            +"            <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
            +"                <xs:element ref='world' />\n"
            +"            </xs:sequence>\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"    <xs:element name='world'>\n"
            +"        <xs:complexType>\n"
            +"            <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
//                        +"                <xs:element name='node' type='xs:hexBinary'/>\n"
            +"                <xs:element name='node' type='xs:string'/>\n"
            +"            </xs:sequence>\n"
            +"            <xs:attribute name='id' type='xs:ID' />\n"
            +"            <xs:attribute name='next' type='xs:IDREF' />\n"
            +"            <xs:attribute name='name' type='xs:string' />\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
            +"    <xs:element name='uvset'>\n"
            +"        <xs:complexType>\n"
            +"            <xs:simpleContent>\n"
            +"                <xs:extension base='xs:int'>\n"
            +"                    <xs:attribute name='name' type='xs:ID' />\n"
            +"                </xs:extension>\n"
            +"            </xs:simpleContent>\n"
            +"        </xs:complexType>\n"
            +"    </xs:element>\n"
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
            +"    <shapes>12345</shapes>\n"
            +"    <materials/>\n"
            +"    <worlds>\n"
            +"       <world id='world1' name='myFirstWorld' next='world2'/>\n"
            +"       <world id='world2' name='mySecondWorld'/>\n"
            +"  </worlds>\n"
            +"  <uvset name='bla'>23</uvset>\n"
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

        DPRINT('myScene.find("//world").name');
        ENSURE('myScene.find("//world").name == "myFirstWorld"');
        ENSURE('myScene.find("//world").$next.name == "mySecondWorld"');


        obj.myScene = obj.myDocument.firstChild;
        print(obj.myScene.childNodes);
        print(obj.myScene.attributes);
        obj.myVersionAttrNode = obj.myScene.attributes.getNamedItem("version");
        obj.myUVSet = obj.myScene.find("//uvset").firstChild;

        ENSURE("myVersionAttrNode.isSameNode(myScene.getAttributeNode('version'))");;
        //obj.myVersionAttrNode.nodeValueDependsOn(obj.myDocument.firstChild.firstChild.firstChild);

        print("onReconnect="+obj.myVersionAttrNode.onReconnect);
        obj.myVersionAttrNode.onReconnect = function() {
            print("onReconnect:");
            print("nodeValueDependsOn: root = "+this.rootNode.nodeName);
            print("nodeValueDependsOn:"+this.rootNode.find("//uvset").firstChild);
            this.nodeValueDependsOn(this.rootNode.find("//uvset").firstChild);
        };
        print("onReconnect="+obj.myVersionAttrNode.onReconnect);

        print("onOutdatedValue="+obj.myVersionAttrNode.onOutdatedValue);
        obj.myVersionAttrNode.onOutdatedValue = function() {
            print("recalculating myVersionAttrNode, this="+this);
            this.nodeValue = this.rootNode.find("//uvset").firstChild.nodeValue * 10;
        };
        print("onOutdatedValue="+obj.myVersionAttrNode.onOutdatedValue);

        print("onSetValue="+obj.myVersionAttrNode.onSetValue);
        obj.myVersionAttrNode.onSetValue = function() {print("onSet, this="+this)};
        print("onSetValue="+obj.myVersionAttrNode.onSetValue);

        gc();
        print("onReconnect="+obj.myVersionAttrNode.onReconnect);
        print("onOutdatedValue="+obj.myVersionAttrNode.onOutdatedValue);
        print("onSetValue="+obj.myVersionAttrNode.onSetValue);

        obj.myDocument.firstChild.firstChild.firstChild.onSetValue = function() {
            print("scene version onSetValue("+this.nodeValue+")");
        }
        obj.myUVSet.onSetValue = function() {
            print("UVSet onSetValue("+this.nodeValue+")");
        };
        print("reading myVersionAttrNode.nodeValue");
        print(obj.myVersionAttrNode.nodeValue);
        ENSURE("myVersionAttrNode.nodeValue == 230");
        print("assigning 42 to myUVSet.nodeValue");
        obj.myUVSet.nodeValue = 42;
        print("reading myVersionAttrNode.nodeValue");
        print(obj.myVersionAttrNode.nodeValue);
        print("reading myVersionAttrNode.nodeValue");
        print(obj.myVersionAttrNode.nodeValue);
        ENSURE("myVersionAttrNode.nodeValue == 420");

        for (var i in obj.myScene.childNodes) {
            print(i);
            print(obj.myScene.childNodes[i]);
        }
        for (i = 0; i < obj.myScene.childNodes.length;++i) {
            print(i);
            print(obj.myScene.childNodes[i]);
        }
        for (i = 0; i < obj.myScene.attributes.length;++i) {
            print(i);
            print(obj.myScene.attributes[i]);
        }
        for (i in obj.myScene.attributes) {
            print(i);
            print(obj.myScene.attributes[i]);
        }
        //exit(1);
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
        obj.myDocument.parse('<hello myattr="&lt;p&gt;">world&lt;p&gt;&amp;</hello>');
        print("myDoc="+obj.myDocument);
        ENSURE('obj.myDocument.firstChild.nodeName == "hello"');
        ENSURE('obj.myDocument.firstChild.myattr == "<p>"');
        ENSURE('obj.myDocument.firstChild.firstChild.nodeValue == "world<p>&"');
        ENSURE('obj.myDocument.firstChild.childNode("#text").nodeValue == "world<p>&"');
        ENSURE('obj.myDocument.firstChild.childNode("#text") == "world&lt;p&gt;&amp;"');
        // Test file saving
        var myDate = new Date();
        obj.myDateString = myDate.toLocaleString();
        obj.mySaveDoc = Node.createDocument();
        obj.mySaveDoc.parse('<date>' + obj.myDateString + '</date>');
        obj.mySaveDoc.saveFile(XMLDOC_1);
        obj.myLoadDoc = Node.createDocument();
        obj.myLoadDoc.parseFile(XMLDOC_1);
        ENSURE('obj.myLoadDoc.firstChild.firstChild.nodeValue == obj.myDateString');

        var myXMLDoc = new Node();
        myXMLDoc.parseFile(XMLDOC_2);
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
        var myDomEvent = new DomEvent("testEvent", myVector, true, false, 0.0);
        //ENSURE = print;
        ENSURE(myDomEvent.stopPropagation());
        ENSURE(myDomEvent.preventDefault());
        ENSURE(myDomEvent.type == "testEvent");
        ENSURE(myDomEvent.eventPhase == 1);
        ENSURE(myDomEvent.bubbles == true);
        ENSURE(myDomEvent.cancelable == false);
        ENSURE(myDomEvent.isDefaultPrevented == true);

        myChildNode2.addEventListener("testEvent", obj, true, "handleEvent");
        myChildNode3.dispatchEvent(myDomEvent);
    }

    obj.handleEvent = function(theEvent) {
        print("eventListener: " + theEvent);
    }
}
function ValueUnitTest() {
    this.Constructor(this, "SchemaUnitTest");
}

ValueUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        DTITLE("Starting Vector Value Tests");
        obj.mySchema = new Node(
                                "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                                +"   <xs:simpleType name='Vector3f'>\n"
                                +"       <xs:restriction base='xs:string' />\n"
                                +"   </xs:simpleType>\n"
                                +"   <xs:simpleType name='VectorOfVector3f'>\n"
                                +"       <xs:restriction base='xs:string' />\n"
                                +"   </xs:simpleType>\n"
                                +"   <xs:simpleType name='VectorOfFloat'>\n"
                                +"       <xs:restriction base='xs:string' />\n"
                                +"   </xs:simpleType>\n"
                                +"    <xs:element name='scene'>\n"
                                +"        <xs:complexType>\n"
                                +"            <xs:sequence>\n"
                                +"                <xs:element ref='vectoroffloat'/>\n"
                                +"                <xs:element ref='vectorofvector3f'/>\n"
                                +"            </xs:sequence>\n"
                                +"            <xs:attribute name='version' type='xs:unsignedLong'/>\n"
                                +"           <xs:attribute name=\"id\" type=\"xs:ID\"/>\n"
                                +"        </xs:complexType>\n"
                                +"    </xs:element>\n"

                                +"    <xs:element name=\"vectorofvector3f\">\n"
                                +"        <xs:complexType>\n"
                                +"            <xs:simpleContent>\n"
                                +"                <xs:extension base=\"VectorOfVector3f\">\n"
                                +"                    <xs:attribute name=\"id\" type=\"xs:ID\"/>\n"
                                +"                    <xs:attribute name=\"name\" type=\"xs:string\"/>\n"
                                +"                </xs:extension>\n"
                                +"            </xs:simpleContent>\n"
                                +"        </xs:complexType>\n"
                                +"    </xs:element>\n"
                                +"    <xs:element name=\"vectoroffloat\">\n"
                                +"        <xs:complexType>\n"
                                +"            <xs:simpleContent>\n"
                                +"                <xs:extension base=\"VectorOfFloat\">\n"
                                +"                    <xs:attribute name=\"id\" type=\"xs:ID\"/>\n"
                                +"                    <xs:attribute name=\"name\" type=\"xs:string\"/>\n"
                                +"                </xs:extension>\n"
                                +"            </xs:simpleContent>\n"
                                +"        </xs:complexType>\n"
                                +"    </xs:element>\n"
                                +"</xs:schema>\n"
                                );
        ENSURE('mySchema.ok');
        obj.myDocument = Node.createDocument();
        obj.myDocument.useFactories("w3c-schema,som");
        obj.myDocument.addSchema(obj.mySchema,"");
        SUCCESS("added Schema");
        DPRINT('obj.myDocument.schema');
        obj.myDocument.parse(
                              "<scene version='214'>"
                             +"       <vectorofvector3f id='iv' name='vectors'>\n"
                             +"       [[0,0,0],[1,1,1]]"
                             +"    </vectorofvector3f>"
                             +"       <vectoroffloat id='fv' name='floats'>\n"
                             +"       [0,1,2,3]"
                             +"    </vectoroffloat>"
                             +"</scene>\n"
                             );
        ENSURE('myDocument.ok');
        //myDocument.getSchema()->dump();
        ENSURE('myDocument.firstChild.nodeName == "scene"');

        obj.myFloats = obj.myDocument.getElementById("fv");
        ENSURE('myFloats.name == "floats"');
        obj.myFloatVectorNode = obj.myFloats.firstChild;
        DPRINT('myFloatVectorNode');
        DPRINT('myFloatVectorNode.nodeValueTypeName');
        ENSURE('myFloatVectorNode.nodeValueTypeName == "VectorOfFloat"');

        obj.myFloatVector = obj.myFloatVectorNode.nodeValue;
        obj.myFullList = obj.myFloatVector.getList(0,obj.myFloatVector.length);
        ENSURE("myFullList.length == myFloatVector.length");
        ENSURE("myFloatVector[0] == 0");
        ENSURE("myFloatVector.getItem(0) == 0");
        obj.myFloatVector.setItem(0,5);
        ENSURE("myFloatVector.getItem(0) == 5");
        obj.myFloatVector.setItem(0,0);
        obj.myFloatVector.appendList(obj.myFullList);
        ENSURE("myFullList.length*2 == myFloatVector.length");
        ENSURE("myFullList.length*2 == obj.myFloatVectorNode.nodeValue.length");

        obj.myFloatVector.eraseList(0, obj.myFullList.length);
        ENSURE("myFullList.length == myFloatVector.length");
        obj.myFloatVector.eraseItem(0);
        obj.myFloatVector.eraseItem(obj.myFloatVector.length-1);
        ENSURE("myFullList.length - 2 == myFloatVector.length");
        ENSURE_EXCEPTION("myFloatVector.eraseItem(myFloatVector.length)");
        obj.myFloatVector.eraseList(0, obj.myFloatVector.length);
        ENSURE("0 == myFloatVector.length");
        obj.myFloatVector.appendItem(23);
        ENSURE("myFloatVector.getItem(0) == 23");
        obj.myFloatVector.insertItemBefore(0,42);
        ENSURE("myFloatVector.getItem(0) == 42");
        obj.myFloatVector.insertListBefore(0,obj.myFullList);
        ENSURE("myFullList.length + 2 == myFloatVector.length");

        obj.myFloatVector.eraseList(0, obj.myFloatVector.length);
        obj.myFloatVector.appendList(new Vector4f(0,1,2,3));
        SUCCESS("appendList(new Vector4f(0,1,2,3))");
        ENSURE("4 == myFloatVector.length");
        obj.myFloatVector.appendList([0,1,2,3]);
        SUCCESS("appendList([0,1,2,3])");
        ENSURE("8 == myFloatVector.length");

    }
}

var myTestName = "testDOM.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new DomEventTest());
mySuite.addTest(new DomParseUnitTest());
mySuite.addTest(new SchemaUnitTest());
mySuite.addTest(new NodeUnitTest());
mySuite.addTest(new ValueUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
