
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
//    $RCSfile: testXmlDom.tst.cpp,v $
//
//   $Revision: 1.35 $
//
//      Author:
//
// Description:
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Nodes.h"
#include "Schema.h"

#include <asl/base/Stream.h>
#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>
#include <asl/base/MappedBlock.h>
#include <asl/math/linearAlgebra.h>

#include <fstream>

#include <math.h>

#include <string>
#include <iostream>

using namespace std;
using namespace dom;
using namespace asl;

class XmlDomCloneNodeUnitTest : public UnitTest {
    public:
        XmlDomCloneNodeUnitTest() : UnitTest("XmlDomCloneNodeUnitTest") {  }
        void run() {
            try {
                dom::Node myElemNode(dom::Node::ELEMENT_NODE, "elem", 0);
                myElemNode.appendChild(dom::Element("elemchild"));
                dom::NodePtr myClonedNode = myElemNode.cloneNode(dom::Node::DEEP);
                ENSURE(myClonedNode->nodeType() == dom::Node::ELEMENT_NODE);
                ENSURE(myClonedNode->nodeName() == "elem");
                ENSURE(myClonedNode->childNodesLength() == 1);
                ENSURE(myClonedNode->firstChild()->parentNode()->nodeName() == "elem");
            }
            catch (asl::Exception & e) {
                std::cerr << "#### failure:" << e << std::endl;
                FAILURE("Exception");
            }
            catch (...) {
                std::cerr << "#### unknown exception occured" <<  std::endl;
                FAILURE("unknown Exception");
            }
        }
};

class XmlDomUnitTest : public UnitTest {
    public:
        XmlDomUnitTest() : UnitTest("XmlDomUnitTest") {  }
        void run() {
            try {
                // test all Node constructors first
                dom::Node myNoNode;
                ENSURE(!myNoNode);

                dom::Node myElemNode(dom::Node::ELEMENT_NODE,"elem",0);
                ENSURE(myElemNode);
                ENSURE(myElemNode.nodeType() == dom::Node::ELEMENT_NODE);
                ENSURE(myElemNode.nodeName() == "elem");
                ENSURE(myElemNode.childNodes().length() == 0);

                dom::Node myAttribNode(dom::Node::ATTRIBUTE_NODE,"attribute","value", 0);
                ENSURE(myAttribNode);
                ENSURE(myAttribNode.nodeType() == dom::Node::ATTRIBUTE_NODE);
                ENSURE(myAttribNode.nodeName() == "attribute");
                ENSURE(myAttribNode.nodeValue() == "value");

                dom::Node myDocumentNode(dom::Node::DOCUMENT_NODE, 0);
                ENSURE(myAttribNode);
                ENSURE(myDocumentNode.nodeType() == dom::Node::DOCUMENT_NODE);
                ENSURE(myDocumentNode.nodeName() == "#document");

                {
                    dom::NodePtr myParsedDocument = dom::NodePtr(new dom::Node("<somexml/>"));
                    ENSURE(myParsedDocument);
                    ENSURE(myParsedDocument->nodeType() == dom::Node::DOCUMENT_NODE);
                    ENSURE(myParsedDocument->nodeName() == "#document");
                    ENSURE(myParsedDocument->childNodes().length() == 1);
                    ENSURE(myParsedDocument->childNodesLength() == 1);
                    ENSURE(myParsedDocument->childNode(0));
                    ENSURE(myParsedDocument->childNode(0)->parentNode() == &(*myParsedDocument));
                    ENSURE(myParsedDocument->childNode(0)->nodeType() == dom::Node::ELEMENT_NODE);
                    ENSURE(myParsedDocument->childNode(0)->nodeName() == "somexml");
                    dom::NodePtr myElement = myParsedDocument->childNode(0);
                    SUCCESS("myElement = myParsedDocument->childNode(0)");
                    myElement->appendChild(dom::NodePtr(new dom::Element("bla")));
                    SUCCESS("myElement->appendChild(dom::Element(bla))");
                    myElement->appendChild(dom::Element("blub"));
                    SUCCESS("myElement->appendChild(dom::Element(blub))");
                    dom::NodePtr myChild = myElement->childNode(1);
                    ENSURE(myChild->parentNode() == &(*myElement));
                    ENSURE(myElement->childNode(0)->nextSibling() == myChild);
                    ENSURE(myElement->childNode(1)->nextSibling() == dom::NodePtr(0));
                    ENSURE(myElement->removeChild(myChild) == myChild);
                    ENSURE(myElement->childNodes().length() == 1);
                    ENSURE(!myElement->removeChild(myChild));
                    myParsedDocument = dom::NodePtr(0);
                    ENSURE(myElement->parentNode() == 0);
                }

            
                dom::NamedNodeMap myNodeMap(0);
                dom::NodePtr myAttribNodePtr(new dom::Attribute("attribute","value"));
                myNodeMap.setNamedItem(myAttribNodePtr);
                ENSURE(myNodeMap.length() == 1);
                ENSURE(myNodeMap.item(0) == myAttribNodePtr);
                ENSURE(&(myNodeMap[0]) == &(*myAttribNodePtr));
                ENSURE(myNodeMap["attribute"].nodeValue() == "value");

                dom::Node myElemWithAttrib("elem_with_attrib",myNodeMap, 0);
                ENSURE(myElemWithAttrib);
                ENSURE(myElemWithAttrib.nodeType() == dom::Node::ELEMENT_NODE);
                ENSURE(myElemWithAttrib.nodeName() == "elem_with_attrib");
                ENSURE(myElemWithAttrib.childNodes().length() == 0);
                ENSURE(myElemWithAttrib.attributes().length() == 1);
                ENSURE(myElemWithAttrib.attributes().item(0)->nodeValue() == "value");
                ENSURE(myElemWithAttrib.attributes()[0].nodeValue() == "value");
                ENSURE(myElemWithAttrib.getAttribute("attribute")->nodeValue() == "value");

                // now try adavanced attribute stuff with attribute children and entity references
                ENSURE(myElemWithAttrib.appendAttribute("attr_with_children",""));
                ENSURE(myElemWithAttrib["attr_with_children"]);
                ENSURE(myElemWithAttrib["attr_with_children"].appendChild(dom::Text("bla")));
                ENSURE(myElemWithAttrib["attr_with_children"].appendChild(dom::Text("fasel")));
                ENSURE(myElemWithAttrib["attr_with_children"].childNodes().length() == 2);

                // and now serialize and reparse it
                dom::DOMString myElemString = asl::as_string(myElemWithAttrib);
                DDUMP(myElemString);
                ENSURE(myElemString.size());
                dom::Document myReloadedElem(myElemString);
                ENSURE(myReloadedElem);

                //...and append an entity reference now
                ENSURE(myElemWithAttrib["attr_with_children"].appendChild(dom::EntityReference("someEntity")));
                ENSURE(myElemWithAttrib["attr_with_children"].appendChild(dom::Text("fasel")));

                // create a document and entity definition for our element
                dom::Document myDocument;
                dom::NodePtr myDocumentType = myDocument.appendChild(dom::DocumentType("elem_with_attrib"));
                ENSURE(myDocumentType);
                dom::Entity myEntity("someEntity","-myEntityValue-");
                ENSURE(myEntity);
                ENSURE(myEntity.nodeType() == dom::Node::ENTITY_NODE);
                ENSURE(myEntity.nodeName() == "someEntity");
                ENSURE(myEntity.nodeValue() == "-myEntityValue-");
                ENSURE(myDocumentType->appendChild(myEntity));
                ENSURE(myDocument.appendChild(myElemWithAttrib));

                // and now serialize the document and reparse it
                dom::DOMString myDocString = asl::as_string(myDocument);
                DDUMP(myDocString);
                ENSURE(myDocString.size());
                dom::Document myReloadedDoc(myDocString);
                ENSURE(myReloadedDoc);
                ENSURE(myReloadedDoc("elem_with_attrib")["attr_with_children"].nodeValue() == "blafasel-myEntityValue-fasel");

                // now check a lot of parsing
                dom::Document myEmptyDocument("");
                ENSURE(!myEmptyDocument);

                dom::Document myGoodDocument1("<elem></elem>");
                ENSURE(myGoodDocument1);

                dom::Document myGoodDocument2("<elem/>");
                ENSURE(myGoodDocument2);

                dom::Document myGoodDocument3("<elem attrib='value'/>");
                ENSURE(myGoodDocument3);

                dom::Document myGoodDocument4("<elem attrib = 'value' />");
                ENSURE(myGoodDocument4);

                dom::Document myBadDocument8("<bookmark orientation='[0] zoom='0'/>");
                ENSURE(!myBadDocument8);

                dom::Document myBadDocument7("<bookmark id='SixFaceFreeViewer' name='free' viewtype='free'"
                                             "orientation='[0.11277,-0.376132,-0.305966,0.86729] zoom='0' position='[0,0,0]'/>");
                ENSURE(!myBadDocument7);
               
                dom::Document myBadDocument1("bla");
                ENSURE(!myBadDocument1);

                dom::Document myBadDocument2("<bla>");
                ENSURE(!myBadDocument2);

                dom::Document myBadDocument3("<>");
                ENSURE(!myBadDocument3);

                dom::Document myBadDocument4("<*>");
                ENSURE(!myBadDocument4);

                dom::Document myBadDocument5("<bla attrib/>");
                ENSURE(!myBadDocument5);

                dom::Document myBadDocument6("<");
                ENSURE(!myBadDocument6);

 
                dom::Element myElem;
                myElem.appendAttribute("myattr","myval");
                ENSURE(myElem["myattr"].nodeValue() == "myval");
                myElem.appendAttribute("myint",int(1));
                ENSURE(myElem["myint"].nodeValue() == "1");

                // Test getElementById without schema
                {
                    char * myTestString =
                        "<root id='r0' attrib='value'>"
                        "   <child0 id='c0' attrib='value'/>"
                        "   <child1 id='c1' attrib='value'>"
                        "       <grandchild0 id='gc0' attrib='value'/>"
                        "       <grandchild1 id='gc1' attrib='value'/>"
                        "   </child1>"
                        "</root>";
                    {
                        dom::Document myIdDocument(myTestString);
                        ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                        ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child0");
                        ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child1");
                        dom::NodePtr myGrandChild = myIdDocument.getElementById("gc0");
                        ENSURE(myGrandChild->nodeName() == "grandchild0");
                        ENSURE(myGrandChild->getElementById("c0")->nodeName() == "child0");
                        ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));
                        dom::NodePtr myRoot = myIdDocument.getElementById("r0");
                        ENSURE(myRoot->getElementById("gc1")->nodeName() == "grandchild1");

                    }
                    {
                        const dom::Document myIdDocument(myTestString);
                        ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                        ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child0");
                        ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child1");
                        const dom::NodePtr myGrandChild = myIdDocument.getElementById("gc0");
                        ENSURE(myGrandChild->nodeName() == "grandchild0");
                        ENSURE(myGrandChild->getElementById("c0")->nodeName() == "child0");
                        ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));
                        const dom::NodePtr myRoot = myIdDocument.getElementById("r0");
                        ENSURE(myRoot->getElementById("gc1")->nodeName() == "grandchild1");
                    }
                }

                 // Test getElementById with schema
                {
                    char * myTestString =
                        "<root id='r0' attrib='value'>"
                        "   <child0 id='c0' attrib='value'/>"
                        "   <child1 id='c1' attrib='value'>"
                        "       <grandchild0 id='gc0' attrib='value'/>"
                        "       <grandchild1 id='gc1' attrib='value'/>"
                        "   </child1>"
                        "</root>";
                    {
                        dom::Document myIdDocument(myTestString);
                        ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                        ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child0");
                        ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child1");
                        ENSURE(myIdDocument.getElementById("gc0")->nodeName() == "grandchild0");
                        ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));
                        dom::NodePtr myRoot = myIdDocument.getElementById("r0");
                        ENSURE(myRoot->getElementById("gc1")->nodeName() == "grandchild1");
                    }
                    {
                        const dom::Document myIdDocument(myTestString);
                        ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                        ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child0");
                        ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child1");
                        ENSURE(myIdDocument.getElementById("gc0")->nodeName() == "grandchild0");
                        ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));
                        const dom::NodePtr myRoot = myIdDocument.getElementById("r0");
                        ENSURE(myRoot->getElementById("gc1")->nodeName() == "grandchild1");
                    }
                }

               // replaceChild Test
                dom::Node myReplaceTestDocument("<somexml><replaceElem/></somexml>");
                dom::NodePtr myRepElem = myReplaceTestDocument("somexml").childNode("replaceElem");
                ENSURE(myRepElem);
                dom::NodePtr myNewElem = dom::NodePtr(new dom::Element("newElem"));
                dom::NodePtr myNewRep = myReplaceTestDocument("somexml").replaceChild(myNewElem,myRepElem);
                ENSURE(myNewRep);
                ENSURE(myNewRep == myNewElem);
                ENSURE(myReplaceTestDocument("somexml").childNode(0)->nodeName()=="newElem");

                runGetAttributePerformanceTest();
            }
            catch (dom::DomException & de) {
                std::cerr << "#### fatal failure:" << de << std::endl;
                FAILURE("DomException");
            }
            catch (dom::Schema::Exception & e) {
                std::cerr << "#### schema validation failure:" << std::endl;
                std::cerr << e << std::endl;
                FAILURE("dom::Schema::Exception");
            }
            catch (asl::Exception & e) {
                std::cerr << "#### failure:" << e << std::endl;
                FAILURE("Exception");
            }
            catch (...) {
                std::cerr << "#### unknown exception occured" <<  std::endl;
                FAILURE("unknown Exception");
            }
        }

        void runGetAttributePerformanceTest() {
            const static char * VISIBLE     = "visible";
            const static char * POSITION    = "position";
            const static char * SCALE       = "scale";
            const static char * ORIENT      = "orientation";
            const static char * SHEAR       = "shear";
            const static char * PIVOT       = "pivot";
            const static char * NAME        = "name";
            const static char * SHAPE       = "shape";
            const static char * ID          = "id";
            const static char * CULLED      = "culled";
            const static char * GLOBALM     = "globalmatrix";
            const static char * BOUNDINGBOX = "boundingbox";

            dom::Node myAttributeTestDocument(
                "<body visible='1' position='[0,0,0]' scale='[1,1,1]' orientation='[0,0,0]'"
                " shear='[0,0,0]' pivot='[0,0,0]' name='abgrund6' shape='s5' id='b5' "
                " culled='0' globalmatrix='identity[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]'"
                " boundingbox='[[69.8129,0,213.687],[404.389,44.7255,476.326]]'/>");

            dom::NodePtr myNode = myAttributeTestDocument.firstChild();
            ENSURE(myNode);
            asl::NanoTime myTimer;
            const unsigned myTestCount = 10000;
            for (unsigned i = 0; i < myTestCount; ++i) {
                NodePtr myAttrib0  = myNode->getAttribute(VISIBLE);
                NodePtr myAttrib1  = myNode->getAttribute(POSITION);
                NodePtr myAttrib2  = myNode->getAttribute(SCALE);
                NodePtr myAttrib3  = myNode->getAttribute(ORIENT);
                NodePtr myAttrib4  = myNode->getAttribute(SHEAR);
                NodePtr myAttrib5  = myNode->getAttribute(PIVOT);
                NodePtr myAttrib6  = myNode->getAttribute(NAME);
                NodePtr myAttrib7  = myNode->getAttribute(SHAPE);
                NodePtr myAttrib8  = myNode->getAttribute(ID);
                NodePtr myAttrib9  = myNode->getAttribute(CULLED);
                NodePtr myAttrib10 = myNode->getAttribute(GLOBALM);
                NodePtr myAttrib11 = myNode->getAttribute(BOUNDINGBOX);
            }
            asl::NanoTime myTimeDiff = asl::NanoTime() - myTimer;
            cerr << "getAttribute Time: " << myTimeDiff.nanos() / myTestCount << endl;
        }
};
class XmlSchemaUnitTest : public UnitTest {
    public:
        XmlSchemaUnitTest() : UnitTest("XmlSchemaUnitTest") {  }
        void run() {
            try {
#if 1
                dom::Schema::Exception ex("what","where");
                ENSURE(ex.what() == "what");
                ENSURE(ex.where() == "where");

                dom::ParseException pex("what","where",23);
                ENSURE(pex.what() == "what");
                ENSURE(pex.where() == "where");
                ENSURE(pex.parsedUntil() == 23);

                dom::Schema::ParseException spex("what","where",23);
                ENSURE(spex.what() == "what");
                ENSURE(spex.where() == "where");
                ENSURE(spex.parsedUntil() == 23);

                {
                    dom::Document myDocument("");
                    ENSURE(!myDocument);

                    ENSURE(dom::Schema::getBuiltinSchema());

                    try {
                        myDocument.addSchema("<noschema/>","");
                        FAILURE("no exception thrown");
                    }
                    catch (dom::Node::Exception &ex) {
                        SUCCESS(asl::as_string(ex).c_str());
                    }

                    myDocument.addSchema("<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'/>","");
                    SUCCESS("addSchema empty");

                    myDocument.addSchema(
                        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                        "   <xs:element name='myelement' type='xs:int'/>"
                        "</xs:schema>",
                        "");
                    myDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    myDocument.checkSchemas();
                    myDocument.getSchema()->dump();


                    SUCCESS("checkSchemas 1");

                    ENSURE(myDocument.parse("<myelement>64</myelement>"));
                }

                {
                    DTITLE("Starting 2nd Schema tests");
                    dom::Document mySchema(
                        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                        "   <xs:element name='scene'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element name='shapes' type='xs:int'/>\n"
                        "               <xs:element name='materials' type='xs:int'/>\n"
                        "               <xs:element ref='worlds'/>\n"
                        "               <xs:element ref='uvset'/>\n"
                        "               <xs:element name='newchild' type='xs:int'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='version' type='xs:unsignedLong'/>\n"
                        "           <xs:attribute name='newattr' type='xs:unsignedLong'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "   <xs:element name='worlds'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
                        "               <xs:element ref='world' />\n"
                        "           </xs:sequence>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "   <xs:element name='world'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
//                        "             <xs:element name='node' type='xs:hexBinary'/>\n"
                        "               <xs:element name='node' type='xs:string'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='id' type='xs:int' />\n"
                        "           <xs:attribute name='name' type='xs:string' />\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "   <xs:element name='uvset'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:simpleContent>\n"
                        "               <xs:extension base='xs:int'>\n"
                        "                   <xs:attribute name='name' type='xs:ID' />\n"
                        "               </xs:extension>\n"
                        "           </xs:simpleContent>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "</xs:schema>\n"
                    );
                    ENSURE(mySchema);
                    dom::Document myDocument;
                    myDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    dom::registerStandardTypes(*myDocument.getValueFactory());
                    myDocument.addSchema(mySchema,"");
                    SUCCESS("added Schema");
                    myDocument.parse(
                        "<scene version='214'>"
                        "   <shapes>12345</shapes>\n"
                        "   <materials/>\n"
                        "   <worlds></worlds>\n"
                        "   <uvset name='bla'>23</uvset>\n"
                        "</scene>\n"
                        );
                    ENSURE(myDocument);
                    myDocument.getSchema()->dump();
                    ENSURE(myDocument("scene")("shapes")("#text").nodeValue() == "12345");

                    ENSURE(dom::dynamic_cast_Value<int>(&myDocument("scene")("shapes")("#text").nodeValueWrapper()));
                    ENSURE(*dom::dynamic_cast_Value<int>(&myDocument("scene")("shapes")("#text").nodeValueWrapper()) == 12345);

                    ENSURE(myDocument("scene")["version"].nodeValue() == "214");
                    ENSURE(dom::dynamic_cast_Value<asl::Unsigned64>(&myDocument("scene")["version"].nodeValueWrapper()));
                    ENSURE(*dom::dynamic_cast_Value<asl::Unsigned64>(&myDocument("scene")["version"].nodeValueWrapper()) == 214);
                    ENSURE(myDocument("scene")["version"].nodeValueAs<asl::Unsigned64>() == 214);
                    ENSURE(myDocument("scene")["version"].nodeValueAs<int>() == 214);
                    ENSURE(myDocument("scene")["version"].nodeValueAs<int>() == 214);

                    ENSURE(myDocument("scene")("uvset")("#text").nodeValueAs<int>() == 23);

                    ENSURE(dom::dynamic_cast_Value<int>(&myDocument("scene")("uvset")("#text").nodeValueWrapper()));
myDocument.getValueFactory()->dump();
                    ENSURE(*dom::dynamic_cast_Value<int>(&myDocument("scene")("uvset")("#text").nodeValueWrapper()) == 23);

                    // now test adding new dom types
                    ENSURE(          myDocument("scene")("worlds").appendChild(dom::Element("world")));
                    ENSURE_EXCEPTION(myDocument("scene")("worlds").appendChild(dom::Element("illegalElement")),dom::Schema::ElementNotAllowed);
                    ENSURE(          myDocument("scene").appendAttribute("newattr","1"));
                    ENSURE_EXCEPTION(myDocument("scene").appendAttribute("illegalAttr","1"),dom::Schema::UndeclaredAttribute);
                    ENSURE_EXCEPTION(myDocument("scene")("worlds")("world").appendChild(dom::Text("A24CFF")), dom::Schema::TextNotAllowed);
                    dom::Node & myNode = myDocument("scene")("worlds")("world")("node");
                    ENSURE(myNode);
                    myNode.appendChild(dom::Text("A24CFF"));
                    ENSURE( myNode.childNode("#text")->nodeValue() == "A24CFF");
                    ENSURE_EXCEPTION(myDocument("scene")("worlds")("world").appendAttribute("id","bla"),dom::ConversionFailed);
                    ENSURE(          myDocument("scene")("worlds")("world").appendAttribute("id","23"));
                    cerr << myDocument;

                    // now lets test some binary serialization
                    dom::NodePtr myOtherWorld = myDocument("scene")("worlds")("world").cloneNode();
                    cerr << *myOtherWorld;
                    myDocument("scene")("worlds").appendChild(myOtherWorld->cloneNode());
                    myDocument("scene")("worlds").appendChild(myOtherWorld->cloneNode());
                    cerr << myDocument;
                    DTITLE("Binarization tests");
                    asl::Block myBinaryRep;
                    dom::DOMString myOriginalXml = asl::as_string(myDocument);
                    myDocument.binarize(myBinaryRep);
                    SUCCESS("binarized dom");
                    DPRINT(myBinaryRep.size());
                    ENSURE(myBinaryRep.size());

                    dom::Document rebinDocument;
                    rebinDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    dom::registerStandardTypes(*rebinDocument.getValueFactory());
                    rebinDocument.addSchema(mySchema,"");
                    SUCCESS("setup rebinDocument");

                    rebinDocument.debinarize(myBinaryRep,0);
                    ENSURE(rebinDocument);

                    dom::DOMString myRebinXml = asl::as_string(rebinDocument);
                    ENSURE(myRebinXml.size());

                    ENSURE(myOriginalXml == myRebinXml);
                    DDUMP(myOriginalXml);
                    DDUMP(myRebinXml);

                    ENSURE(rebinDocument("scene")("shapes")("#text").nodeValue() == "12345");

                    ENSURE(dom::dynamic_cast_Value<int>(&rebinDocument("scene")("shapes")("#text").nodeValueWrapper()));
                    ENSURE(*dom::dynamic_cast_Value<int>(&rebinDocument("scene")("shapes")("#text").nodeValueWrapper()) == 12345);

                    ENSURE(rebinDocument("scene")["version"].nodeValue() == "214");
                    ENSURE(dom::dynamic_cast_Value<asl::Unsigned64>(&rebinDocument("scene")["version"].nodeValueWrapper()));
                    ENSURE(*dom::dynamic_cast_Value<asl::Unsigned64>(&rebinDocument("scene")["version"].nodeValueWrapper()) == 214);
                }

                {
                    DTITLE("Starting 2nd Schema tests");
                    dom::Document mySchema(
                        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                        "   <xs:element name='scene'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element name='shapes' type='xs:int'/>\n"
                        "               <xs:element name='materials' type='xs:int'/>\n"
                        "               <xs:element name='world' type='xs:int'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='version' type='xs:unsignedLong'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "</xs:schema>\n"
                    );
                    ENSURE(mySchema);
                    dom::Document myDocument;
                    myDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    dom::registerStandardTypes(*myDocument.getValueFactory());
                    myDocument.addSchema(mySchema,"");
                    SUCCESS("added Schema");
                    myDocument.parse(
                        "<scene version='214'>"
                        "   <shapes>12345</shapes>\n"
                        "   <materials/>\n"
                        "   <world>FAIL34567</world>\n"
                        "</scene>\n"
                        );
                    ENSURE(!myDocument);

                }
#endif

                //TODO: The xml spec says: Validity constraint: One ID per Element Type 
                //TODO: No element type may have more than one ID attribute specified.
                //TODO: Therefore the following test does not use a valid schema

                // Test getElementById with schema
                {
                    DTITLE("Starting getElementById with Schema tests");
                    dom::Document mySchema(
                        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                        "   <xs:element name='root'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element ref='child'/>\n"
                        "               <xs:element ref='refchild'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='attrib' type='xs:string'/>\n"
                        "           <xs:attribute name='id' type='xs:ID'/>\n"
                        "           <xs:attribute name='id2' type='xs:ID'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "   <xs:element name='child'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element ref='child'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='id' type='xs:ID'/>\n"
                        "           <xs:attribute name='id2' type='xs:ID'/>\n"
                        "           <xs:attribute name='attrib' type='xs:string'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "   <xs:element name='refchild'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element ref='refchild'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='id' type='xs:ID'/>\n"
                        "           <xs:attribute name='childid' type='xs:IDREF'/>\n"
                        "           <xs:attribute name='attrib' type='xs:string'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                         "</xs:schema>\n"
                    );
                    ENSURE(mySchema);
                    dom::Document myIdDocument;
                    myIdDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    dom::registerStandardTypes(*myIdDocument.getValueFactory());
                    myIdDocument.addSchema(mySchema,"");
                    SUCCESS("added Schema");
                    DTITLE("Parsing original document");
                    myIdDocument.parse(
                        "<root id='r0' id2='xr0' attrib='value0'>"
                        "   <child id='c0' id2='xc0' attrib='value1'/>"
                        "   <child id='c1' id2='xc1' attrib='value2'>"
                        "       <child id='gc0' id2='xgc0' attrib='value3'/>"
                        "       <child id='gc1' id2='xgc1' attrib='value4'/>"
                        "   </child>"
                        "   <refchild id='x1' childid='c0' attrib='value5'>"
                        "       <refchild id='x2' childid='gc1' attrib='value6'/>"
                        "   </refchild>"
                        "</root>"
                    );
                    ENSURE(myIdDocument);

                    ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                    ENSURE(myIdDocument.getElementById("r0")->getAttributeString("id") == "r0");
                    ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child");
                    ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child");
                    ENSURE(myIdDocument.getElementById("gc0")->nodeName() == "child");
                    ENSURE(myIdDocument.getElementById("gc0")->getElementById("r0")->nodeName() == "root");
                    ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));

                    ENSURE(myIdDocument.getElementById("xgc1", "id2")->getAttributeString("id") == "gc1");
                    {
                        const dom::Document & myCIDDocument = myIdDocument;
                        ENSURE(myCIDDocument.getElementById("r0")->nodeName() == "root");
                        ENSURE(myCIDDocument.getElementById("c0")->nodeName() == "child");
                        ENSURE(myCIDDocument.getElementById("c1")->nodeName() == "child");
                        ENSURE(myCIDDocument.getElementById("gc0")->nodeName() == "child");
                        ENSURE(myCIDDocument.getElementById("gc0")->getElementById("r0")->nodeName() == "root");
                        ENSURE(myCIDDocument.getElementById("xxx") == dom::NodePtr(0));
                    }
                    myIdDocument.getElementById("gc0")->getAttribute("id")->nodeValue("newid");
                    ENSURE(myIdDocument.getElementById("newid")->getAttributeString("id") == "newid");
                    ENSURE(myIdDocument.getElementById("gc0") == dom::NodePtr(0));

                    DTITLE("Modified document");
                    cerr << myIdDocument << endl;

                    DTITLE("Cloning document");
                    Node myClone = myIdDocument;
                    DTITLE("Cloned document");
                    cerr << myClone << endl;
                    ENSURE(myClone.getElementById("r0")->nodeName() == "root");
                    ENSURE(myClone.getElementById("r0")->getAttributeString("id") == "r0");
                    ENSURE(myClone.getElementById("c0")->nodeName() == "child");
                    ENSURE(myClone.getElementById("c1")->nodeName() == "child");
                    ENSURE(myClone.getElementById("newid")->nodeName() == "child");
                    ENSURE(myClone.getElementById("newid")->getElementById("r0")->nodeName() == "root");
                    ENSURE(myClone.getElementById("xxx") == dom::NodePtr(0));
                    myClone.getElementById("newid")->getAttribute("id")->nodeValue("newid2");
                    ENSURE(myClone.getElementById("newid2")->getAttributeString("id") == "newid2");
                    ENSURE(myClone.getElementById("newid") == dom::NodePtr(0));

                    DTITLE("Replaced original document root element with cloned root element");
                    myIdDocument("root") = myClone("root");
                    cerr << myIdDocument << endl;
                    ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                    ENSURE(myIdDocument.getElementById("r0")->getAttributeString("id") == "r0");
                    ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child");
                    ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child");
                    ENSURE(myIdDocument.getElementById("newid2")->nodeName() == "child");
                    ENSURE(myIdDocument.getElementById("newid2")->getElementById("r0")->nodeName() == "root");
                    ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));

                    dom::NodePtr myNodeToRemove = myIdDocument.getElementById("c0");
                    ENSURE(myNodeToRemove);
                    dom::NodePtr myRemovedNode = myIdDocument("root").removeChild(myNodeToRemove);
                    ENSURE(myRemovedNode);
                    ENSURE(!myIdDocument.getElementById("c0"));
                    myIdDocument("root")("child")["id"] = "c0";
                    cerr << myIdDocument << endl;
                    ENSURE(myIdDocument.getElementById("c0"));
                    dom::NodePtr myNodeToReplace = myIdDocument.getElementById("c0");
                    myIdDocument("root").replaceChild(myRemovedNode, myNodeToReplace);
                    ENSURE(myIdDocument.getElementById("c0")->getAttributeString("attrib") == "value1");
                }
            }
            catch (dom::DomException & de) {
                std::cerr << "#### fatal failure:" << de << std::endl;
                FAILURE("DomException");
            }
            catch (dom::Schema::Exception & e) {
                std::cerr << "#### schema validation failure:" << std::endl;
                std::cerr << e << std::endl;
                FAILURE("dom::Schema::Exception");
            }
            catch (asl::Exception & e) {
                std::cerr << "#### failure:" << e << std::endl;
                FAILURE("Exception");
            }
            catch (...) {
                std::cerr << "#### unknown exception occured" <<  std::endl;
                FAILURE("unknown Exception");
            }
        }
};


class XmlCatalogUnitTest : public UnitTest {
    public:
        XmlCatalogUnitTest() : UnitTest("XmlCatalogUnitTest") {  }
        void run() {
            try {
                // Test loadElementById with schema
                DTITLE("Starting getElementById with Schema tests");
                dom::Document mySchema(
                        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                        "   <xs:element name='root'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element ref='child'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='attrib' type='xs:string'/>\n"
                        "           <xs:attribute name='id' type='xs:ID'/>\n"
                        "           <xs:attribute name='id2' type='xs:ID'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "   <xs:element name='child'>\n"
                        "       <xs:complexType>\n"
                        "           <xs:sequence>\n"
                        "               <xs:element ref='child'/>\n"
                        "           </xs:sequence>\n"
                        "           <xs:attribute name='id' type='xs:ID'/>\n"
                        "           <xs:attribute name='id2' type='xs:ID'/>\n"
                        "           <xs:attribute name='attrib' type='xs:string'/>\n"
                        "       </xs:complexType>\n"
                        "   </xs:element>\n"
                        "</xs:schema>\n"
                        );
                ENSURE(mySchema);
                dom::Document myIdDocument;
                myIdDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                dom::registerStandardTypes(*myIdDocument.getValueFactory());
                myIdDocument.addSchema(mySchema,"");
                SUCCESS("added Schema");
                DTITLE("Parsing original document");
                myIdDocument.parse(
                        "<root id='r0' id2='xr0' attrib='value0'>"
                        "   <child id='c0' id2='xc0' attrib='value1'/>"
                        "   <child id='c1' id2='xc1' attrib='value2'>"
                        "       <child id='gc0' id2='xgc0' attrib='value3'/>"
                        "       <child id='gc1' id2='xgc1' attrib='value4'/>"
                        "       <child id='gc2' id2='xgc2' attrib='value5'>"
                        "           <child id='ggc0' id2='xggc2' attrib='value6'/>"
                        "           <child id='ggc1' id2='xggc3' attrib='value7'>"
                        "               <child id='gggc0' id2='xgggc3' attrib='value8'/>"
                        "           </child>"
                        "       </child>"
                        "   </child>"
                        "</root>"
                        );
                ENSURE(myIdDocument);

                ENSURE(myIdDocument.getElementById("r0")->nodeName() == "root");
                ENSURE(myIdDocument.getElementById("r0")->getAttributeString("id") == "r0");
                ENSURE(myIdDocument.getElementById("c0")->nodeName() == "child");
                ENSURE(myIdDocument.getElementById("c1")->nodeName() == "child");
                ENSURE(myIdDocument.getElementById("gc0")->nodeName() == "child");
                ENSURE(myIdDocument.getElementById("gc0")->getElementById("r0")->nodeName() == "root");
                ENSURE(myIdDocument.getElementById("xxx") == dom::NodePtr(0));

                // write out the document with catalog
                {
                    asl::WriteableFile myFile("cattest.b60");
                    asl::WriteableFile myCatalogFile("cattest.c60");
                    myIdDocument.binarize(myFile, myCatalogFile);
                    asl::WriteableFile myDBFile("cattest.d60");
                    myIdDocument.binarize(myDBFile, myDBFile);
                }
                SUCCESS("file and catalog binarized");
                {
                    Dictionaries myDictionaries;
                    asl::ReadableFile myCatalogFile("cattest.c60");
                    unsigned myPos = myDictionaries.debinarize(myCatalogFile, 0);
                    SUCCESS("debinarized dictionaries");
                    NodeOffsetCatalog myCatalog;
                    ENSURE(myCatalog.debinarize(myCatalogFile, myPos)+8 == asl::getFileSize("cattest.c60"));
                    SUCCESS("debinarized catalog");
                }
                {
                    Dictionaries myDictionaries;
                    NodeOffsetCatalog myCatalog;
                    asl::ReadableFile myCatalogFile("cattest.c60");
                    ENSURE(dom::loadDictionariesAndCatalog(myCatalogFile, 0, myDictionaries, myCatalog) == asl::getFileSize("cattest.c60")) ; 
                    SUCCESS("debinarized dictionaries & catalog");
                }
                {
                    Dictionaries myDictionaries;
                    NodeOffsetCatalog myCatalog;
                    asl::ConstMappedBlock myCatalogFile("cattest.d60");
                    ENSURE(dom::loadDictionariesAndCatalog(myCatalogFile, myDictionaries, myCatalog) == asl::getFileSize("cattest.d60")) ; 
                    SUCCESS("debinarized dictionaries & catalog from end of file");
                }
                {
                    asl::ReadableFile myFile("cattest.b60");
                    NodePtr myRoot = myIdDocument.getElementById("r0"); 
                    NodePtr myChild0 = myIdDocument.getElementById("c0"); 
                    NodePtr myChild1 = myIdDocument.getElementById("c1");
                    ENSURE(myRoot->removeChild(myChild0));
                    ENSURE(myRoot->removeChild(myChild1));
                    DPRINT(myIdDocument);
                    NodePtr myNewChild0 = myRoot->appendChild(NodePtr(new Element("child")));
                    NodePtr myNewChild1 = myRoot->appendChild(NodePtr(new Element("child")));
                    DPRINT(myIdDocument);
                    DictionariesPtr myDictionaries = DictionariesPtr(new Dictionaries) ;
                    NodeOffsetCatalog myCatalog;
                    asl::ConstMappedBlock myCatalogFile("cattest.c60");
                    ENSURE(dom::loadDictionariesAndCatalog(myCatalogFile, *myDictionaries, myCatalog) == asl::getFileSize("cattest.c60")) ; 
                    SUCCESS("debinarized dictionaries & catalog from end of file");
                    ENSURE(myNewChild0->loadElementById("c0","id",myFile, 0, *myDictionaries, myCatalog));
                    DPRINT(myIdDocument);
                    ENSURE(myNewChild1->loadElementById("c1","id",myFile, 0, *myDictionaries, myCatalog));
                    DPRINT(myIdDocument);
                }
                  
                {
                    DTITLE("Testing Lazy Loading");
                    dom::Document myLazyIdDocument;
                    myLazyIdDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    dom::registerStandardTypes(*myLazyIdDocument.getValueFactory());
                    myLazyIdDocument.addSchema(mySchema,"");
                    SUCCESS("added Schema to myLazyIdDocument");
                    DTITLE("Lazy loading original document");

                    asl::Ptr<asl::ReadableStreamHandle> myFile(new asl::AlwaysOpenReadableFileHandle("cattest.d60"));
                    myLazyIdDocument.debinarizeLazy(myFile);
                    ENSURE(myLazyIdDocument);
                    ENSURE(myLazyIdDocument.hasLazyChildren());
                    ENSURE(myLazyIdDocument.firstChild());
                    ENSURE(!myLazyIdDocument.hasLazyChildren());
                    DPRINT(myLazyIdDocument);
                }
                {
                    DTITLE("Testing Lazy Loading by ID");
                    dom::Document myLazyIdDocument;
                    myLazyIdDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                    dom::registerStandardTypes(*myLazyIdDocument.getValueFactory());
                    myLazyIdDocument.addSchema(mySchema,"");
                    SUCCESS("added Schema to myLazyIdDocument");
                    DTITLE("Lazy loading original document");

                    asl::Ptr<asl::ReadableStreamHandle> myFile(new asl::AlwaysOpenReadableFileHandle("cattest.d60"));
                    myLazyIdDocument.debinarizeLazy(myFile);
                    ENSURE(myLazyIdDocument);
                    ENSURE(myLazyIdDocument.hasLazyChildren());
                    
                    NodePtr myRoot = myLazyIdDocument.getElementById("r0");
                    ENSURE(myRoot); 
                    NodePtr myChild0 = myLazyIdDocument.getElementById("c0");
                    ENSURE(myChild0); 
                    NodePtr myChild1 = myLazyIdDocument.getElementById("c1");
                    ENSURE(myChild1); 
                    NodePtr myChild8 = myLazyIdDocument.getElementById("gggc0");
                    ENSURE(myChild8); 
                    ENSURE(myChild8->getAttribute("id")->nodeValue() == "gggc0"); 
                    DPRINT(myLazyIdDocument);
                }
            }
            catch (dom::DomException & de) {
                std::cerr << "#### fatal failure:" << de << std::endl;
                FAILURE("DomException");
            }
            catch (dom::Schema::Exception & e) {
                std::cerr << "#### schema validation failure:" << std::endl;
                std::cerr << e << std::endl;
                FAILURE("dom::Schema::Exception");
            }
            catch (asl::Exception & e) {
                std::cerr << "#### failure:" << e << std::endl;
                FAILURE("Exception");
            }
            catch (...) {
                std::cerr << "#### unknown exception occured" <<  std::endl;
                FAILURE("unknown Exception");
            }
        }
};
class XmlDomEventsUnitTest : public UnitTest {
    public:
        XmlDomEventsUnitTest() : UnitTest("XmlDomEventsUnitTest") {  }

        struct MyTestEventListener;
        void checkEvent(MyTestEventListener * theTestListener, EventPtr evt);

        struct MyTestEventListener : public dom::EventListener {
            MyTestEventListener(XmlDomEventsUnitTest * theShell,
                    EventTargetPtr theTestTargetNode,
                    DOMString theEventType)
                : _myTestTargetNode(theTestTargetNode),
                _myEventType(theEventType),
                _myShell(theShell),
                _myCount(0),
                _cancelOnCapture(false),
                _cancelOnBubbling(false),
                _cancelAtTarget(false)
            {}

            virtual void handleEvent(EventPtr evt) {
                _lastEvent = evt;
                _myShell->checkEvent(this, evt);
                ++_myCount;
                if (_cancelOnCapture && evt->eventPhase() == Event::CAPTURING_PHASE) {
                    evt->stopPropagation();
                }
                if (_cancelOnBubbling && evt->eventPhase() == Event::BUBBLING_PHASE) {
                    evt->stopPropagation();
                }
                if (_cancelAtTarget && evt->eventPhase() == Event::AT_TARGET) {
                    evt->stopPropagation();
                }
            }
            EventPtr _lastEvent;
            EventTargetPtr _myTestTargetNode;
            DOMString _myEventType;
            XmlDomEventsUnitTest * _myShell;
            int _myCount;
            bool _cancelOnCapture;
            bool _cancelOnBubbling;
            bool _cancelAtTarget;
        };

        void run() {
            try {
                // Test getElementById
                {
                    char * myTestString =
                        "<root id='r0' attrib='value'>"
                        "   <child0 id='c0' attrib='value'/>"
                        "   <child1 id='c1' attrib='value'>"
                        "       <grandchild0 id='gc0' attrib='value'/>"
                        "       <grandchild1 id='gc1' attrib='value'/>"
                        "   </child1>"
                        "</root>";

                    dom::Document myIdDocument(myTestString);
                    dom::NodePtr myR0 = myIdDocument.getElementById("r0");
                    dom::NodePtr myC0 = myIdDocument.getElementById("c0");
                    dom::NodePtr myC1 = myIdDocument.getElementById("c1");
                    dom::NodePtr myGC0 = myIdDocument.getElementById("gc0");
                    dom::NodePtr myGC1 = myIdDocument.getElementById("gc1");
                    ENSURE(myR0->nodeName() == "root");
                    ENSURE(myC0->nodeName() == "child0");
                    ENSURE(myC1->nodeName() == "child1");
                    ENSURE(myGC0->nodeName() == "grandchild0");
                    ENSURE(myGC1->nodeName() == "grandchild1");
                    dom::NodePtr myGC1attrib = myGC1->getAttribute("attrib");

                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myRootTestListener0(new MyTestEventListener(this,myR0,"test"));
                    myR0->addEventListener("test", myRootTestListener0, true);

                    EventPtr myTestEvent = dom::StandardEventFactory().createEvent("test");
                    DTITLE("testing single listener");
                    myGC1attrib->dispatchEvent(myTestEvent);
                    ENSURE(myRootTestListener0->_myCount == 1);
                    ENSURE(myRootTestListener0->_lastEvent == myTestEvent);

                    // now lets test the whole chain
                    DTITLE("testing whole chain");
                    myR0->addEventListener("test", myRootTestListener0, false);

                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myC0TestListener0(new MyTestEventListener(this,myC0,"test"));
                    myC0->addEventListener("test", myC0TestListener0, true);
                    myC0->addEventListener("test", myC0TestListener0, false);
                    myC0->addEventListener("nevercall", myC0TestListener0, true);
                    myC0->addEventListener("nevercall", myC0TestListener0, false);

                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myC1TestListener0(new MyTestEventListener(this,myC1,"test"));
                    myC1->addEventListener("test", myC1TestListener0, true);
                    myC1->addEventListener("test", myC1TestListener0, false);
                    myC1->addEventListener("nevercall", myC1TestListener0, true);
                    myC1->addEventListener("nevercall", myC1TestListener0, false);

                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myGC1TestListener0(new MyTestEventListener(this,myGC1,"test"));
                    myGC1->addEventListener("test", myGC1TestListener0, true);
                    myGC1->addEventListener("test", myGC1TestListener0, false);

                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myGC1TestListener1(new MyTestEventListener(this,myGC1,"test"));
                    myGC1->addEventListener("test", myGC1TestListener1, true);
                    myGC1->addEventListener("test", myGC1TestListener1, false);

                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myGC1AttribTestListener0(new MyTestEventListener(this,myGC1attrib,"test"));
                    myGC1attrib->addEventListener("test", myGC1AttribTestListener0, true);
                    myGC1attrib->addEventListener("test", myGC1AttribTestListener0, false);
                    myGC1attrib->addEventListener("nevercall", myGC1AttribTestListener0, true);
                    myGC1attrib->addEventListener("nevercall", myGC1AttribTestListener0, false);

                    myGC1attrib->dispatchEvent(myTestEvent);

                    ENSURE(myRootTestListener0->_myCount == 3);
                    ENSURE(myC0TestListener0->_myCount == 0);
                    ENSURE(myC1TestListener0->_myCount == 2);
                    ENSURE(myGC1TestListener0->_myCount == 2);
                    ENSURE(myGC1TestListener1->_myCount == 2);
                    DPRINT(myGC1AttribTestListener0->_myCount);
                    ENSURE(myGC1AttribTestListener0->_myCount == 2);

                    // now lets test removal
                    DTITLE("testing removeEventListener");
                    myGC1attrib->removeEventListener("test", myGC1AttribTestListener0, true);
                    myGC1attrib->removeEventListener("test", myGC1AttribTestListener0, false);
                    myGC1attrib->dispatchEvent(myTestEvent);

                    ENSURE(myRootTestListener0->_myCount == 5);
                    ENSURE(myC0TestListener0->_myCount == 0);
                    ENSURE(myC1TestListener0->_myCount == 4);
                    ENSURE(myGC1TestListener0->_myCount == 4);
                    ENSURE(myGC1TestListener1->_myCount == 4);
                    ENSURE(myGC1AttribTestListener0->_myCount == 2);

                    // now we add even more listeners
                    DTITLE("testing with more listeners");
                    asl::Ptr<MyTestEventListener, ThreadingModel>
                        myUniversalTestListener0(new MyTestEventListener(this,NodePtr(0),"test"));

                    // and now we try some canceling by the listener
                    DTITLE("testing canceling by the listener");
                    myGC1attrib->addEventListener("test", myGC1AttribTestListener0, true);
                    myGC1attrib->addEventListener("test", myGC1AttribTestListener0, false);

                    // cancel root on capturing
                    DTITLE("testing canceling by root on capturing");
                    myRootTestListener0->_cancelOnCapture = true;
                    myGC1attrib->dispatchEvent(myTestEvent);
                    ENSURE(myRootTestListener0->_myCount == 6);
                    ENSURE(myC0TestListener0->_myCount == 0);
                    ENSURE(myC1TestListener0->_myCount == 4);
                    ENSURE(myGC1TestListener0->_myCount == 4);
                    ENSURE(myGC1TestListener1->_myCount == 4);
                    ENSURE(myGC1AttribTestListener0->_myCount == 2);
                    myRootTestListener0->_cancelOnCapture = false;

                    // cancel grandchild on bubbling on bubbling
                    DTITLE("testing canceling by grandchild on bubbling");
                    myGC1TestListener1->_cancelOnBubbling = true;
                    myTestEvent = dom::StandardEventFactory().createEvent("test");
                    myGC1attrib->dispatchEvent(myTestEvent);
                    ENSURE(myRootTestListener0->_myCount == 7);
                    ENSURE(myC0TestListener0->_myCount == 0);
                    ENSURE(myC1TestListener0->_myCount == 5);
                    ENSURE(myGC1TestListener0->_myCount == 6);
                    ENSURE(myGC1TestListener1->_myCount == 6);
                    ENSURE(myGC1AttribTestListener0->_myCount == 4);
                    myGC1TestListener1->_cancelOnBubbling = false;

                    // restrict phases by event info
                    DTITLE("testing restrict phases by event into (uncancelable,non-bubbling)");
                    myTestEvent = dom::StandardEventFactory().createEvent("test");
                    myTestEvent->initEvent("test", false, false);
                    myRootTestListener0->_cancelOnCapture = true;
                    myGC1attrib->dispatchEvent(myTestEvent);
                    ENSURE(myRootTestListener0->_myCount == 8);
                    ENSURE(myC0TestListener0->_myCount == 0);
                    ENSURE(myC1TestListener0->_myCount == 6);
                    ENSURE(myGC1TestListener0->_myCount == 7);
                    ENSURE(myGC1TestListener1->_myCount == 7);
                    ENSURE(myGC1AttribTestListener0->_myCount == 5);
                    myRootTestListener0->_cancelOnCapture = false;

                    // test root element == target
                    DTITLE("test root element == target");
                    myTestEvent = dom::StandardEventFactory().createEvent("test");
                    myR0->dispatchEvent(myTestEvent);
                    ENSURE(myRootTestListener0->_myCount == 10);
                    ENSURE(myC0TestListener0->_myCount == 0);
                    ENSURE(myC1TestListener0->_myCount == 6);
                }
            }
            catch (dom::DomException & de) {
                std::cerr << "#### fatal failure:" << de << std::endl;
                FAILURE("DomException");
            }
            catch (dom::Schema::Exception & e) {
                std::cerr << "#### schema validation failure:" << std::endl;
                std::cerr << e << std::endl;
                FAILURE("dom::Schema::Exception");
            }
            catch (asl::Exception & e) {
                std::cerr << "#### failure:" << e << std::endl;
                FAILURE("Exception");
            }
            catch (...) {
                std::cerr << "#### unknown exception occured" <<  std::endl;
                FAILURE("unknown Exception");
            }
        }

};

void
XmlDomEventsUnitTest::checkEvent(XmlDomEventsUnitTest::MyTestEventListener * theTestListener, EventPtr evt) {
    NodePtr myNode = dynamic_cast_Ptr<Node>(evt->currentTarget());
    DTITLE(string("Handling Event at Node name = ")+myNode->nodeName()+", phase = " + dom::PhaseTypeName[evt->eventPhase()]);
    ENSURE(evt->type() == theTestListener->_myEventType);
    if (theTestListener->_myTestTargetNode) {
        ENSURE(evt->currentTarget() == theTestListener->_myTestTargetNode);
    }
}

void
printChangedNodes(const Node & theNode, unsigned long long theLastVersion, int theLevel) {
    if (theNode.nodeVersion() > theLastVersion || theLastVersion == 0xffffffffffffffffULL) {
        std::string myPrefix;
        for (int i = 0; i < theLevel; ++i) {
            myPrefix += "  ";
        }
        cerr << myPrefix << theNode.nodeTypeName() << " '"<< theNode.nodeName() <<"', V="<<theNode.nodeVersion()<<", UID="<<theNode.getUniqueId();
        cerr << " = ";
        if (theNode.mayHaveValue()) {
            cerr << theNode.nodeValue();
        } else {
            cerr <<"#NO VALUE";
        }
        cerr << endl;
        for (unsigned i = 0; i < theNode.attributesLength(); ++i ) {
            printChangedNodes(*theNode.attributes().item(i), theLastVersion, theLevel+1);
        }
        for (unsigned i = 0; i < theNode.childNodesLength(); ++i ) {
            printChangedNodes(*theNode.childNode(i), theLastVersion, theLevel+1);
        }
    }
}



class XmlPatchUnitTest : public UnitTest {
public:
    XmlPatchUnitTest() : UnitTest("XmlPatchUnitTest") {  }

        const dom::Document & getTestSchema() {
           static dom::Document mySchema(
                "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                "   <xs:element name='root'>\n"
                "       <xs:complexType>\n"
                "           <xs:sequence>\n"
                "               <xs:element ref='child'/>\n"
                "           </xs:sequence>\n"
                "           <xs:attribute name='attrib' type='xs:string'/>\n"
                "           <xs:attribute name='id' type='xs:ID'/>\n"
                "           <xs:attribute name='id2' type='xs:ID'/>\n"
                "           <xs:attribute name='newattrib' type='xs:string'/>\n"
                "           <xs:attribute name='intattrib' type='xs:int'/>\n"
                "       </xs:complexType>\n"
                "   </xs:element>\n"
                "   <xs:element name='child'>\n"
                "       <xs:complexType>\n"
                "           <xs:sequence>\n"
                "               <xs:element ref='child'/>\n"
                "           </xs:sequence>\n"
                "           <xs:attribute name='id' type='xs:ID'/>\n"
                "           <xs:attribute name='id2' type='xs:ID'/>\n"
                "           <xs:attribute name='attrib' type='xs:string'/>\n"
                "           <xs:attribute name='newattrib' type='xs:string'/>\n"
                "           <xs:attribute name='intattrib' type='xs:int'/>\n"
                "       </xs:complexType>\n"
                "   </xs:element>\n"
                "</xs:schema>\n"
                );
            ENSURE(mySchema);
            return mySchema;
        }
        void setupDocument(dom::Document & theDocument) {
            theDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
            dom::registerStandardTypes(*theDocument.getValueFactory());
            theDocument.addSchema(getTestSchema(),"");
            SUCCESS("added Schema");
        }
        void testPatch(dom::Node & myOldDocument,
            unsigned long long myOldVersion,
            dom::Node & myNewDocument,
            const asl::ReadableBlock & theOriginalDocumentBlock)
        {
            {
                cerr << "myOldDocument:" << endl;
                cerr << myOldDocument << endl;
                // make an patch containing no modifactions and apply it; no changes should be made to the patched document
                cerr << "make empty patch from version "<< myNewDocument.nodeVersion() << " -> " <<  myNewDocument.nodeVersion() << endl;
                asl::Block myNewBlock;
                cerr << endl << "myNewDocument.binarize (emptytest-pre)"<<endl;
                myNewDocument.binarize(myNewBlock);
                asl::Block myPatch;
                cerr << endl << "myNewDocument.makePatch (empty)"<<endl;
                myNewDocument.makePatch(myPatch, myNewDocument.nodeVersion());
                cerr << endl <<"myNewDocument.applyPatch (empty)"<<endl;
                myNewDocument.applyPatch(myPatch);
                asl::Block myPatchedBlock;
                cerr << endl << "myNewDocument.binarize (emptytest-post)"<<endl;
                myNewDocument.binarize(myPatchedBlock);
                ENSURE(myPatchedBlock == myNewBlock);
            } {
                // now make a real patch and apply it
                asl::Block myPatch;
                cerr << endl << "make patch from version "<< myOldVersion << " -> " <<  myNewDocument.nodeVersion() << endl;
                myNewDocument.makePatch(myPatch, myOldVersion);
#define DUMP_NODES
#ifdef DUMP_NODES
                cerr <<"myNewDocument:"<<endl;
                printChangedNodes(myNewDocument, 0xffffffffffffffffULL, 0);           
                cerr <<"myOldDocument (pre patch):"<<endl;
                printChangedNodes(myOldDocument, 0xffffffffffffffffULL, 0);                
#endif
                cerr << endl << "applying patch from version "<< myOldVersion << " -> " <<  myNewDocument.nodeVersion() << ", size =" << myPatch.size() << endl;
                unsigned long long myPrePatchVersion = myOldDocument.nodeVersion();
                cerr << "myOldVersion=" << myOldVersion << ", myOldDocument.nodeVersion (myPrePatchVersion) = " << myPrePatchVersion << endl;

                myOldDocument.applyPatch(myPatch);
                SUCCESS("applied patch");
                cerr << "myOldDocument.nodeVersion (post patch version) = " <<  myOldDocument.nodeVersion() << endl;
#ifdef DUMP_NODES
                cerr <<"myOldDocument (post patch):"<<endl;
                printChangedNodes(myOldDocument, 0xffffffffffffffffULL, 0);                
#endif                
                // check if the patched document is identical to the original
                asl::Block myPatchedBlock;
                cerr << endl << "myNewDocument.binarize (post patch check)"<<endl;
                myOldDocument.binarize(myPatchedBlock);
                SUCCESS("binarized old");            
                asl::Block myNewBlock;
                cerr << endl << "myNewDocument.binarize (post patch check ref)"<<endl;
                myNewDocument.binarize(myNewBlock);
                SUCCESS("binarized new");  
                ENSURE(myPatchedBlock == myNewBlock);
                if (myPatchedBlock != myNewBlock) {
                    cerr << "patched myOldDocument:"<< endl;
                    cerr << myOldDocument << endl;
                    cerr << "myNewDocument:"<< endl;
                    cerr << myNewDocument << endl;
                }
                
                // now make a patch of the patched document and see if it matches the applied patch
                asl::Block myGen2Patch;
                cerr << endl << "myNewDocument.binarize (post patch patch gen)"<<endl;
                myOldDocument.makePatch(myGen2Patch, myPrePatchVersion);
                ENSURE(myGen2Patch == myPatch);
                if (myGen2Patch != myPatch) {
                    cerr << "myPatch    :" << myPatch << endl;
                    cerr << "myGen2Patch:" << myGen2Patch << endl;
                }

                // check the total patch from the first document 
                dom::Document myOriginalDocument;
                setupDocument(myOriginalDocument);;
                SUCCESS("setup myOriginalDocument");
                myOriginalDocument.debinarize(theOriginalDocumentBlock);
#ifdef DUMP_NODES
                cerr <<"myOriginalDocument:"<<endl;
                printChangedNodes(myOriginalDocument, 0xffffffffffffffffULL, 0);                
                cerr <<"myNewDocument:"<<endl;
                printChangedNodes(myNewDocument, 0xffffffffffffffffULL, 0);
#endif
                unsigned long long myNewDocumentVersionPrePrint = myNewDocument.nodeVersion();
                cerr << endl << "make patch from version "<< 1 << " -> " <<  myNewDocument.nodeVersion() << endl;
#ifdef DUMP_NODES
                cerr <<"myNewDocument again:"<<endl;
                printChangedNodes(myNewDocument, 0xffffffffffffffffULL, 0);
#endif                
                ENSURE(myNewDocumentVersionPrePrint == myNewDocument.nodeVersion());
                asl::Block myCumulatedPatch;
                cerr <<"changes myOriginalDocument->myNewDocument:"<<endl;
                myNewDocument.makePatch(myCumulatedPatch, 1);
#ifdef DUMP_NODES
                printChangedNodes(myNewDocument, 1, 0);
#endif                
                cerr << endl << "applying patch from version "<< 1 << " -> " <<  myNewDocument.nodeVersion() << ", size =" << myCumulatedPatch.size() << endl;
                myOriginalDocument.applyPatch(myCumulatedPatch);
                SUCCESS("applied cumulated patch");

                // check if the patched document is identical to the original
                asl::Block myCumPatchedBlock;
                cerr << endl << "myNewDocument.binarize (post patch check)"<<endl;
                myOriginalDocument.binarize(myCumPatchedBlock);
                SUCCESS("binarized old");            
                ENSURE(myCumPatchedBlock == myNewBlock);
                if (myCumPatchedBlock != myNewBlock) {
                    cerr << "patched myOldDocument:"<< endl;
                    cerr << myOldDocument << endl;
                    cerr << "myNewDocument:"<< endl;
                    cerr << myNewDocument << endl;
                }
            }
        }
        void run() {
        try {           
            DTITLE("Starting patch with Schema tests");
            dom::Document myIdDocument;
            setupDocument(myIdDocument);

            DTITLE("Parsing original document");
            myIdDocument.parse(
                "<root id='r0' id2='xr0' attrib='value0'>"
                "   <child id='c0' id2='xc0' attrib='value1'/>"
                "   <child id='c1' id2='xc1' attrib='value2'>"
                "       <child id='gc0' id2='xgc0' attrib='value3'/>"
                "       <child id='gc1' id2='xgc1' attrib='value4'/>"
                "   </child>"
                "</root>"
                );
            ENSURE(myIdDocument);

            asl::Block myBinarizedOriginalDocument;
            myIdDocument.binarize(myBinarizedOriginalDocument);

            dom::Document myIdDocument2;
            setupDocument(myIdDocument2);;
            SUCCESS("added Schema 2");

            myIdDocument2.debinarize(myBinarizedOriginalDocument);

            asl::Block myBinarizedDocument2;
            myIdDocument2.binarize(myBinarizedDocument2);
            ENSURE(myBinarizedOriginalDocument == myBinarizedDocument2);
            SUCCESS("rebinarized document");

            dom::NodePtr myR0 = myIdDocument.getElementById("r0");
            ENSURE(myR0);
            unsigned long long myDocumentVersion = myIdDocument.nodeVersion();
            DPRINT(myDocumentVersion);
            ENSURE(&myIdDocument == myIdDocument.getRootNode());
            ENSURE(myR0->getRealRootNode() == myIdDocument.getRealRootNode());
            NodePtr myNewAttr = myR0->appendAttribute("newattrib", "newvalue");
            ENSURE(myNewAttr);

            unsigned long long myNewDocumentVersion = myIdDocument.nodeVersion(); 
            DPRINT(myNewDocumentVersion);
            ENSURE(myNewDocumentVersion > myDocumentVersion);
#define PERFORM_TEST_PATCH
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            
            myDocumentVersion = myIdDocument.nodeVersion();
            DOMString myNewAttrStringValue = myNewAttr->nodeValue();
            ENSURE(myDocumentVersion == myIdDocument.nodeVersion());
            ValuePtr myNewAttrValue = myNewAttr->nodeValueWrapperPtr();
            ENSURE(myDocumentVersion == myIdDocument.nodeVersion());

            // set attribute value via nodeValue
            DTITLE("set attribute value via nodeValue");
            myNewAttr->nodeValue("newvalue2");
            DPRINT(myIdDocument.nodeVersion());
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // append new gc attribute value via nodeValue
            DTITLE("append new gc attribute value via nodeValue");
            dom::NodePtr myGC1 = myIdDocument.getElementById("gc1");
            NodePtr myNewIntAttr = myR0->appendAttribute("intattrib", "0");
            DPRINT(myIdDocument.nodeVersion());
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // set attribute value via nodeValueAssign
            DTITLE("set attribute value via nodeValueAssign");
            myNewIntAttr->nodeValueAssign<int>(1);
            DPRINT(myDocumentVersion);
            DPRINT(myIdDocument.nodeVersion());
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // append child node
            DTITLE("append child node");
            NodePtr myChild = myIdDocument("root").appendChild(dom::Element("child"));
            (*myChild)["id"].nodeValue("nc");
            DPRINT(myIdDocument.nodeVersion());
            DPRINT(myDocumentVersion);
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // append two more childs
            DTITLE("append two more childs");
            DPRINT(myIdDocument.nodeVersion());
            NodePtr myChild2 = myIdDocument("root").appendChild(dom::Element("child"));
            (*myChild2)["id"].nodeValue("nc2");
            NodePtr myChild3 = myIdDocument("root").appendChild(dom::Element("child"));
            (*myChild3)["id"].nodeValue("nc3");
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // remove middle child
            DTITLE("remove middle child");
            DPRINT(myIdDocument.nodeVersion());
            myChild2->parentNode()->removeChild(myChild2);
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // remove first child
            DTITLE("remove first child");
            DPRINT(myIdDocument.nodeVersion());
            dom::NodePtr myC0 = myIdDocument.getElementById("c0");
            myC0->parentNode()->removeChild(myC0);
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

            // remove last child
            DTITLE("remove last child");
            DPRINT(myIdDocument.nodeVersion());
            myChild3->parentNode()->removeChild(myChild3);
            ENSURE(myDocumentVersion < myIdDocument.nodeVersion());
#ifdef PERFORM_TEST_PATCH
            testPatch(myIdDocument2, myDocumentVersion, myIdDocument, myBinarizedOriginalDocument);
#endif
            myDocumentVersion = myIdDocument.nodeVersion();

        } catch (dom::DomException & de) {
            std::cerr << "#### fatal failure:" << de << std::endl;
            FAILURE("DomException");
        }
        catch (dom::Schema::Exception & e) {
            std::cerr << "#### schema validation failure:" << std::endl;
            std::cerr << e << std::endl;
            FAILURE("dom::Schema::Exception");
        }
        catch (asl::Exception & e) {
            std::cerr << "#### failure:" << e << std::endl;
            FAILURE("Exception");
        }
        catch (...) {
            std::cerr << "#### unknown exception occured" <<  std::endl;
            FAILURE("unknown Exception");
        }
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new XmlDomUnitTest);
        addTest(new XmlDomCloneNodeUnitTest);
        addTest(new XmlDomEventsUnitTest);
        addTest(new XmlPatchUnitTest);
        addTest(new XmlSchemaUnitTest); 
        addTest(new XmlCatalogUnitTest);
     }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}






