
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
//    $RCSfile: testFacade.tst.cpp,v $
//
//   $Revision: 1.10 $
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
#include "AttributePlug.h"
#include "PropertyPlug.h"
#include "ChildNodePlug.h"

#include <asl/base/UnitTest.h>
#include <asl/math/numeric_functions.h>

#include <string>
#include <iostream>

using namespace std;
using namespace dom;
using namespace asl;

#define NO_EXPORT

//                       theTagName     theType         theAttributeName  theDefault
DEFINE_ATTRIBUT_TAG        (IdTag,         std::string,    "id",             "",    NO_EXPORT);
DEFINE_ATTRIBUT_TAG        (FloatTag,      float,          "float",          1,     NO_EXPORT);
DEFINE_FACADE_ATTRIBUTE_TAG(Float2Tag,     float,          "float2",         1,     NO_EXPORT);
DEFINE_ATTRIBUT_TAG        (BoolTag,       bool,           "bool",           false, NO_EXPORT);
DEFINE_FACADE_ATTRIBUTE_TAG(HalfFloatTag,  float,          "halffloat",      0.5,   NO_EXPORT);
DEFINE_FACADE_ATTRIBUTE_TAG(DoubleParentFloatTag,  float,  "doubleparentfloat", 2,  NO_EXPORT);
DEFINE_FACADE_ATTRIBUTE_TAG(ChildFloatSumTag,  float,      "childfloatsumtag", 0,   NO_EXPORT);

DEFINE_PROPERTY_TAG(FloatPropTag,  TestPropertiesFacade, float, "float", "floatproperty",  "properties", 0.5);

DEFINE_PROPERTY_TAG(ReqTexturesTag,  RequirementsFacade, std::string, "feature", "textures", "requires", "[100[paint]]");
DEFINE_PROPERTY_TAG(ReqTexcoordTag, RequirementsFacade, std::string, "feature", "texcoord", "requires", "");

class IdFacade :
    public Facade,
    public IdTag::Plug
{
    public:
        IdFacade(Node & theNode) :
            Facade(theNode),
            IdTag::Plug(theNode)
        {}

        IMPLEMENT_FACADE(IdFacade);
};

class RequirementsFacade :
    public Facade,
    public ReqTexturesTag::Plug,
    public ReqTexcoordTag::Plug
{
    public:
        RequirementsFacade(Node & theNode) :
            Facade(theNode),
            ReqTexturesTag::Plug(this),
            ReqTexcoordTag::Plug(this)
        {}
        IMPLEMENT_FACADE(RequirementsFacade);
};
typedef asl::Ptr<RequirementsFacade, dom::ThreadingModel> RequirementsFacadePtr;

DEFINE_CHILDNODE_TAG(RequirementsTag, RootFacade, RequirementsFacade, "requires");

class RootFacade :
    public IdFacade,
    public FloatTag::Plug,
    public RequirementsTag::Plug
{
    public:
        RootFacade(Node & theNode) :
            IdFacade(theNode),
            FloatTag::Plug(theNode),
            ChildNodePlug<RequirementsTag,RootFacade>(this)
        {}

        IMPLEMENT_PARENT_FACADE(RootFacade);

};


class ChildFacade;
typedef asl::Ptr<ChildFacade, dom::ThreadingModel> ChildFacadePtr;


class TestPropertiesFacade :
    public Facade,
    public FloatPropTag::Plug
{
    public:
        TestPropertiesFacade(Node & theNode) :
            Facade(theNode),
            FloatPropTag::Plug(this)
        {}
        IMPLEMENT_FACADE(TestPropertiesFacade);
};
typedef asl::Ptr<TestPropertiesFacade, dom::ThreadingModel> TestPropertiesFacadePtr;

DEFINE_CHILDNODE_TAG(PropertiesTag, ChildFacade, TestPropertiesFacade, "properties");

class ChildFacade :
    public IdFacade,
    public BoolTag::Plug,
    public PropertiesTag::Plug,
    public FloatTag::Plug,
    public FacadeAttributePlug<Float2Tag>,
    public FacadeAttributePlug<HalfFloatTag>,
    public FacadeAttributePlug<DoubleParentFloatTag>,
    public FacadeAttributePlug<ChildFloatSumTag>
{
    public:
        ChildFacade(Node & theNode) :
            IdFacade(theNode),
            BoolTag::Plug(theNode),
            PropertiesTag::Plug(this),
            FloatTag::Plug(theNode),
            FacadeAttributePlug<Float2Tag>(this),
            FacadeAttributePlug<HalfFloatTag>(this),
            FacadeAttributePlug<DoubleParentFloatTag>(this),
            FacadeAttributePlug<ChildFloatSumTag>(this)
        {
            AC_PRINT << "ChildFacade Ctor ";
        }

        void registerDependenciesForHalfFloat() {
            AC_TRACE << "ChildFacade::registerDependencies: this ="<<(void*)this;
            Node & myNode = getNode();
            if (myNode) {
                HalfFloatTag::Plug::dependsOn<FloatTag>(*this);
                asl::Ptr<ChildFacade, dom::ThreadingModel> mySelf = dynamic_cast_Ptr<ChildFacade>(getSelf());
                HalfFloatTag::Plug::setCalculatorFunction(&ChildFacade::recalculateHalfFloat);
            }
         }

         void registerDependenciesForDoubleParentFloat() {
            AC_TRACE << "ChildFacade::registerDependenciesForDoubleParentFloat: this ="<<(void*)this;
            Node & myNode = getNode();
            if (myNode) {
                asl::Ptr<ChildFacade, dom::ThreadingModel> mySelf = dynamic_cast_Ptr<ChildFacade>(getSelf());
                if (myNode.parentNode() && myNode.parentNode()->nodeName() != "root") {
                    asl::Ptr<ChildFacade, dom::ThreadingModel> myParent = myNode.parentNode()->getFacade<ChildFacade>();
                    DoubleParentFloatTag::Plug::dependsOn<DoubleParentFloatTag>(*myParent);
                    DoubleParentFloatTag::Plug::setCalculatorFunction(&ChildFacade::recalculateDoubleParentFloat);
                } else {
                    DoubleParentFloatTag::Plug::dependsOn<FloatTag>(*this);
                    DoubleParentFloatTag::Plug::setCalculatorFunction(&ChildFacade::copyOwnFloat);
                }
#ifdef DEBUG_VARIANT
                DoubleParentFloatTag::Plug::debug();
#endif
            }
         }

         void registerDependenciesForChildFloatSum() {
            AC_TRACE << "ChildFacade::registerDependenciesForChildFloatSum: this ="<<(void*)this;
            Node & myNode = getNode();
            if (myNode) {
#ifdef OLD_PARENT_DEPENDENCY_UPDATE
                sdfgsdfg
                // for uptree dependencies, we have to register both directions
                // register with parent first
                if (myNode.parentNode() && myNode.parentNode()->nodeName() == "child") {
                    ChildFacadePtr myParent = myNode.parentNode()->getFacade<ChildFacade>();
                    if (!FloatTag::Plug::alreadyAffects<ChildFloatSumTag>(*myParent)) {
                        FloatTag::Plug::affects<ChildFloatSumTag>(*myParent);
                    }
                }
#endif
                // register with all children now
                for (unsigned i = 0; i < myNode.childNodesLength(); ++i) {
                    NodePtr myChildNode = myNode.childNode(i);
                    if (myChildNode->nodeName() == "child") {
                        ChildFacadePtr myChild = myChildNode->getFacade<ChildFacade>();
                        //if (!ChildFloatSumTag::Plug::alreadyDependsOn<FloatTag>(*myChild)) {
                            ChildFloatSumTag::Plug::dependsOn<FloatTag>(*myChild);
                        //}
                    }
                }
                ChildFloatSumTag::Plug::setCalculatorFunction(&ChildFacade::calculateChildFloatSum);
            }
        }

       void registerDependenciesRegistrators() {
            AC_TRACE << "ChildFacade::registerDependencyCallbacks: this ="<<(void*)this;
            HalfFloatTag::Plug::setReconnectFunction(&ChildFacade::registerDependenciesForHalfFloat);
            DoubleParentFloatTag::Plug::setReconnectFunction(&ChildFacade::registerDependenciesForDoubleParentFloat);
            ChildFloatSumTag::Plug::setReconnectFunction(&ChildFacade::registerDependenciesForChildFloatSum);
        }

        void recalculateHalfFloat() {
            AC_TRACE << "recalculateHalfFloat: this="<<(void*)this <<",node=" << getNode() << endl;
            set<HalfFloatTag>(get<FloatTag>() / 2);
            AC_TRACE << "result(HalfFloatTag) =" << get<HalfFloatTag>() << endl;
        }
        void recalculateDoubleParentFloat() {
            AC_TRACE << "recalculateDoubleParentFloat: this="<<(void*)this <<",node=" << getNode() << endl;
            asl::Ptr<ChildFacade, dom::ThreadingModel> myParent = getNode().parentNode()->getFacade<ChildFacade>();
            if (myParent) {
                AC_TRACE << "recalculateDoubleParentFloat: parent="<<(void*)myParent.get() <<",node=" << getNode() << endl;
                float myValue = myParent->get<DoubleParentFloatTag>();
                AC_TRACE << "recalculateDoubleParentFloat: parent value="<<myValue<<endl;
                set<DoubleParentFloatTag>(myParent->get<DoubleParentFloatTag>() * 2);
                AC_TRACE << "result(DoubleParentFloatTag) =" << get<DoubleParentFloatTag>() << endl;
            }
        }

        void copyOwnFloat() {
            AC_TRACE << "copyOwnFloat: this="<<(void*)this <<",node=" << getNode() << endl;
            set<DoubleParentFloatTag>(get<FloatTag>());
            AC_TRACE << "result(DoubleParentFloatTag) =" << get<DoubleParentFloatTag>() << endl;
        }

        void calculateChildFloatSum() {
            float mySum = 0;
            for (unsigned i = 0; i < getNode().childNodesLength(); ++i) {
                NodePtr myChildNode = getNode().childNode(i);
                if (myChildNode->nodeName() == "child") {
                    ChildFacadePtr myChild = myChildNode->getFacade<ChildFacade>();
                    mySum += myChild->get<FloatTag>();
                }
            }
            set<ChildFloatSumTag>(mySum);
        }

        IMPLEMENT_PARENT_FACADE(ChildFacade);
};


DEFINE_ATTRIBUT_TAG(WidthTag,          float,    "width",      0, NO_EXPORT);
DEFINE_ATTRIBUT_TAG(HeightTag,         float,    "height",     0, NO_EXPORT);
DEFINE_ATTRIBUT_TAG(AspectRatioTag,    float,    "aspect",     0, NO_EXPORT);
DEFINE_ATTRIBUT_TAG(InvAspectRatioTag, float,    "inv-aspect", 0, NO_EXPORT);

class DynamicFacade :
    public IdFacade,
    public FacadeAttributePlug<HeightTag>,
    public FacadeAttributePlug<WidthTag>,
    public DynamicAttributePlug<AspectRatioTag, DynamicFacade>,
    public DynamicAttributePlug<InvAspectRatioTag, DynamicFacade>
{
    public:
        DynamicFacade(Node & theNode) :
            IdFacade(theNode),
            FacadeAttributePlug<HeightTag>(this),
            FacadeAttributePlug<WidthTag>(this),
            DynamicAttributePlug<AspectRatioTag, DynamicFacade>(this, &DynamicFacade::getAspectRatio),
            DynamicAttributePlug<InvAspectRatioTag, DynamicFacade>(this, &DynamicFacade::getInvAspectRatio)
            {}

        IMPLEMENT_DYNAMIC_FACADE(DynamicFacade);
   private:
        bool getAspectRatio(float & theNewValue) const {
            theNewValue = get<HeightTag>() ? get<WidthTag>()/get<HeightTag>() : 0.0f;
            return true;
        }
        bool getInvAspectRatio(float & theNewValue) const {
            theNewValue = get<WidthTag>() ? get<HeightTag>()/get<WidthTag>() : 0.0f;
            return true;
        }
};


typedef asl::Ptr<IdFacade, dom::ThreadingModel>    IdFacadePtr;
typedef asl::Ptr<RootFacade, dom::ThreadingModel>  RootFacadePtr;
typedef asl::Ptr<DynamicFacade, dom::ThreadingModel> DynamicFacadePtr;

class FacadeUnitTest : public UnitTest {
    public:
        FacadeUnitTest() : UnitTest("FacadeUnitTest") {  }
        void run() {
            try {
                char * myTestString =
                    "<root id='r0' float='1.2'>\n"
                    "   <child id='c0' bool='1'/>\n"
                    "   <child id='c1'>\n"
                    "       <properties>\n"
                    "           <float name='floatproperty'>1.5</float>\n"
                    "       </properties>\n"
                    "   </child>\n"
                    "   <dynamic id='v0'/>\n"
                    "   <child id='c2'>\n"
                    "       <child id='cc3'>\n"
                    "          <child id='ccc4'/>\n"
                    "       </child>\n"
                    "   </child>\n"
                    "   <child id='c5'/>\n"
                    "   <requires>\n"
                    "           <feature name='textures'>b0rken</feature>\n"
                    "   </requires>\n"
                    "</root>";

                dom::Document myTestSchema(
                    "<?xml version='1.0' encoding='utf-8' ?>\n"
                    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>\n"
                    "   <xs:element name='float'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:simpleContent>\n"
                    "               <xs:extension base='xs:float'>\n"
                    "                   <xs:attribute name='name' type='xs:string'/>\n"
                    "               </xs:extension>\n"
                    "           </xs:simpleContent>\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "   <xs:element name='root'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
                    "               <xs:element ref='child'/>\n"
                    "               <xs:element ref='dynamic'/>\n"
                    "               <xs:element ref='requires'/>\n"
                    "           </xs:sequence>\n"
                    "           <xs:attribute name='id' type='xs:ID' />\n"
                    "           <xs:attribute name='float' type='xs:float' />\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "   <xs:element name='child'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
                    "               <xs:element ref='properties'/>\n"
                    "               <xs:element ref='child'/>\n"
                    "           </xs:sequence>\n"
                    "           <xs:attribute name='id' type='xs:ID' />\n"
                    "           <xs:attribute name='float' type='xs:float' />\n"
                    "           <xs:attribute name='bool' type='xs:boolean' />\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "   <xs:element name='properties'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
                    "               <xs:element ref='float'/>\n"
                    "           </xs:sequence>\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "   <xs:element name='feature'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:simpleContent>\n"
                    "               <xs:extension base='xs:string'>\n"
                    "                   <xs:attribute name='name' type='xs:string'/>\n"
                    "               </xs:extension>\n"
                    "           </xs:simpleContent>\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "   <xs:element name='requires'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
                    "               <xs:element ref='feature'/>\n"
                    "           </xs:sequence>\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "   <xs:element name='dynamic'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:attribute name='id' type='xs:ID' />\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n" ); // XXX
                    // "</xs:schema>");     //

                Document myDocument;
                myDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
                dom::registerStandardTypes(*myDocument.getValueFactory());

                myDocument.setFacadeFactory(FacadeFactoryPtr(new dom::FacadeFactory()));
                myDocument.getFacadeFactory()->registerPrototype("root", FacadePtr(new RootFacade(Node::Prototype)));
                myDocument.getFacadeFactory()->registerPrototype("child", FacadePtr(new ChildFacade(Node::Prototype)));
                myDocument.getFacadeFactory()->registerPrototype("dynamic", FacadePtr(new DynamicFacade(Node::Prototype)));
                myDocument.getFacadeFactory()->registerPrototype("requires", FacadePtr(new RequirementsFacade(Node::Prototype)),"root");
                myDocument.getFacadeFactory()->registerPrototype("properties", FacadePtr(new TestPropertiesFacade(Node::Prototype)),"child");

                myDocument.addSchema(myTestSchema,"");
                myDocument.parse(myTestString);
                ENSURE(myDocument);

                NodePtr myRoot = myDocument.firstChild();

                ENSURE(myRoot->nodeName() == "root");
                ENSURE(myRoot->getAttribute("id"));
                ENSURE(myRoot->getAttribute("id")->nodeValue() == "r0");
                ENSURE(myRoot->getAttribute("float"));
                ENSURE(almostEqual(myRoot->getAttribute("float")->nodeValueRef<float>(), 1.2));

               AC_PRINT << "start req facade testing...";
                RootFacadePtr myFacade = myRoot->getFacade<RootFacade>();

                RequirementsFacadePtr myReqFacade = myFacade->getChild<RequirementsTag>();

                AC_PRINT << "1";
                ENSURE(myFacade);
                ENSURE(myFacade->get<IdTag>() == "r0");
                ENSURE(myReqFacade->get<ReqTexturesTag>() == "b0rken");
                ENSURE(myReqFacade->get<ReqTexcoordTag>() == ReqTexcoordTag::getDefault());
                ENSURE(almostEqual(myFacade->get<FloatTag>(), 1.2));
                AC_PRINT << "2";

                AC_PRINT << "CHILDFACADE ?";
                ChildFacadePtr myChildFacade = myRoot->firstChild()->getFacade<ChildFacade>();
                AC_PRINT << "CHILDFACADE OK";
                ENSURE(myChildFacade);
                ENSURE(myChildFacade->get<IdTag>() == "c0");
                ENSURE(myChildFacade->get<BoolTag>() == true);

                IdFacadePtr myIdRootFacade = myRoot->getFacade<IdFacade>();
                ENSURE(myIdRootFacade);
                ENSURE(myIdRootFacade->get<IdTag>() == "r0");

                IdFacadePtr myIdChildFacade = myRoot->firstChild()->getFacade<IdFacade>();
                ENSURE(myIdChildFacade);
                ENSURE(myIdChildFacade->get<IdTag>() == "c0");

                {
                    // setAttributeString -> Facade->get()
                    myRoot->getAttribute("id")->nodeValue("newid");
                    ENSURE(myFacade->get<IdTag>() == "newid");
                    myRoot->getAttribute("float")->nodeValue("2.3");
                    ENSURE(almostEqual(myFacade->get<FloatTag>(), 2.3f));

                    // setAttributeValue  -> Facade->get()
                    static string myId = "newerid";
                    myRoot->getAttribute("id")->nodeValueWrapperPtr()->setString(myId);
                    ENSURE(myFacade->get<IdTag>() == "newerid");
                    static float  myFloat = 5.0;
                    myRoot->getAttribute("float")->nodeValueAssign(myFloat);
                    ENSURE(almostEqual(myFacade->get<FloatTag>(), 5.0f));

                    // Facade->set() -> getAttributeString()
                    myFacade->set<IdTag>("newestid");
                    ENSURE(myRoot->getAttribute("id")->nodeValue() == "newestid");

                    myFacade->set<FloatTag>(4.2f);
                    ENSURE(myRoot->getAttribute("float")->nodeValue() == "4.2");

                    myFacade->set<FloatTag>(2.4f);
                    // String representation is not set dirty, so the next test fails :-(
                    //ENSURE(myRoot->getAttribute("float")->nodeValue() == "2.4");

                    // Facade->set() -> getAttributeValue()
                    myFacade->set<IdTag>("latestid");
                    ENSURE(myRoot->getAttribute("id")->nodeValueWrapperPtr()->getString() == "latestid");
                    myFacade->set<FloatTag>(19.75f);
                    ENSURE(almostEqual(myRoot->getAttribute("float")->nodeValueRef<float>(), 19.75f));
                }
                 // test additional attributes (FacadeAttributePlug)
                ENSURE(myChildFacade->get<Float2Tag>() == 1.0f);
                myChildFacade->set<Float2Tag>(42.0);
                ENSURE(myChildFacade->get<Float2Tag>() == 42.0f);
                ENSURE(myChildFacade->getNamedItem("float2"));
                ENSURE(myChildFacade->getNamedItem("float2")->nodeValueRef<float>() == 42.0f);

                // test all attribute creation
                NodePtr myChild2 = myRoot->childNode("child", 1);
                ENSURE(myChild2);
                ChildFacadePtr myChild2Facade = myChild2->getFacade<ChildFacade>();
                ENSURE(myChild2Facade);
                ENSURE(myChild2->getAttribute("bool"));

                // test dynamic attributes
                NodePtr myDynamicNode = myRoot->childNode("dynamic",0);
                ENSURE(myDynamicNode);
                DPRINT(*myDynamicNode)
                DynamicFacadePtr myDynamic = myDynamicNode->getFacade<DynamicFacade>();
                ENSURE(myDynamic);

                myDynamic->set<WidthTag>(100.0f);
                myDynamic->set<HeightTag>(50.0f);
                dom::NodePtr myDynAttrNode = myDynamic->getNamedItem("aspect");
                dom::NodePtr myDynAttrNode2 = myDynamic->getNamedItem("inv-aspect");
                ENSURE(almostEqual(myDynAttrNode->nodeValueRef<float>(), 2.0f));
                ENSURE(almostEqual(myDynamic->get<AspectRatioTag>(), 2.0f));
                ENSURE(almostEqual(myDynAttrNode2->nodeValueRef<float>(), 0.5f));
                ENSURE(almostEqual(myDynamic->get<InvAspectRatioTag>(), 0.5f));

                myDynamic->set<WidthTag>(200.0f);

                myDynAttrNode = myDynamic->getNamedItem("aspect");
                myDynAttrNode2 = myDynamic->getNamedItem("inv-aspect");
                ENSURE(almostEqual(myDynAttrNode2->nodeValueRef<float>(), 0.25f));
                ENSURE(almostEqual(myDynamic->get<InvAspectRatioTag>(), 0.25f));
                ENSURE(almostEqual(myDynAttrNode->nodeValueRef<float>(), 4.0f));
                ENSURE(almostEqual(myDynamic->get<AspectRatioTag>(), 4.0f));

                // Test property facades
                {
                    ChildFacadePtr myChildFacade = myRoot->childNode("child", 0)->getFacade<ChildFacade>();
                    TestPropertiesFacadePtr myPropFacade = myChildFacade->getChild<PropertiesTag>();
                    ENSURE(myPropFacade->get<FloatPropTag>() == 0.5);
                    myPropFacade->set<FloatPropTag>(2.5);
                    ENSURE(myPropFacade->get<FloatPropTag>() == 2.5);
                }
                {
                    ChildFacadePtr myChildFacade = myRoot->childNode("child", 1)->getFacade<ChildFacade>();
                    TestPropertiesFacadePtr myPropFacade = myChildFacade->getChild<PropertiesTag>();
                    ENSURE(myPropFacade->get<FloatPropTag>() == 1.5);
                    ENSURE(myPropFacade->set<FloatPropTag>(2.5) == 2.5);
                    ENSURE(myPropFacade->get<FloatPropTag>() == 2.5);
                }
                {
                    ChildFacadePtr myChildFacade = myRoot->childNode("child", 1)->getFacade<ChildFacade>();
                    TestPropertiesFacadePtr myPropFacade = myChildFacade->getChild<PropertiesTag>();
                    ENSURE(myPropFacade->set<FloatPropTag>(3.5) == 3.5);
                    ENSURE(myPropFacade->get<FloatPropTag>() == 3.5);
                }
                // Test dependent attributes
                {
                    myChildFacade->set<FloatTag>(42.0);
                    ENSURE(myChildFacade->get<FloatTag>() == 42);
                    ENSURE(myChildFacade->isDirty<HalfFloatTag>());
                    DPRINT(myChildFacade->get<FloatTag>());
                    DPRINT(myChildFacade->get<HalfFloatTag>());
                    ENSURE(myChildFacade->get<HalfFloatTag>() == myChildFacade->get<FloatTag>() / 2);
                    ENSURE(!myChildFacade->isDirty<HalfFloatTag>());

                    NodePtr myChild2 = myRoot->getElementById("c2");
                    ENSURE(myChild2);
                    ChildFacadePtr myChild2Facade = myChild2->getFacade<ChildFacade>();
                    ENSURE(myChild2Facade);
                    myChild2Facade->set<FloatTag>(3);
                    ENSURE(myChild2Facade->get<FloatTag>() == 3);
                    ENSURE(myChild2Facade->isDirty<DoubleParentFloatTag>());
                    ENSURE(myChild2Facade->get<DoubleParentFloatTag>() == 3);

                    // assign value using various dom::nodeValue() interfaces
                    NodePtr myFloatAttr2;
                    ENSURE(myFloatAttr2 = myChild2->getAttribute("float"));
                    ENSURE(*(myFloatAttr2->nodeValuePtr<float>()) == 3.0f);
                    ENSURE((myFloatAttr2->nodeValueRefOpen<float>() = 4) == 4);
                    myFloatAttr2->nodeValueRefClose<float>();
                    ENSURE(myFloatAttr2->nodeValueRef<float>() == 4);
                    ENSURE(myChild2Facade->get<FloatTag>() == 4);
                    ENSURE(myChild2Facade->isDirty<HalfFloatTag>());
                    ENSURE(myChild2Facade->get<HalfFloatTag>() == myChild2Facade->get<FloatTag>() / 2);

                    // now assign using nodeValueWrapperPtr()
                    ValuePtr myNewValue = myFloatAttr2->nodeValueWrapperPtr()->clone(0);

                    myNewValue->setString("3.0");
                    ENSURE(myNewValue->getString() == "3");

                    const float * myNewFloatPtr = dynamic_cast_Value<float>(myNewValue.get());
                    ENSURE(myNewFloatPtr);
                    ENSURE(*myNewFloatPtr == 3.0f);
                    myFloatAttr2->nodeValueWrapperPtr(myNewValue);

                    ENSURE(myChild2Facade->get<FloatTag>() == 3.0f);
                    ENSURE(myChild2Facade->isDirty<HalfFloatTag>());
                    ENSURE(myChild2Facade->get<HalfFloatTag>() == myChild2Facade->get<FloatTag>() / 2);

                    // check child/parent dependencies
                    NodePtr myChild3 = myRoot->getElementById("cc3");
                    ENSURE(myChild3);
                    ChildFacadePtr myChild3Facade = myChild3->getFacade<ChildFacade>();
                    ENSURE(myChild3Facade);
                    ENSURE(myChild3Facade->get<DoubleParentFloatTag>() == 6);

                    NodePtr myChild4 = myRoot->getElementById("ccc4");
                    ENSURE(myChild4);

                    ChildFacadePtr myChild4Facade = myChild4->getFacade<ChildFacade>();
                    ENSURE(myChild4Facade);
                    DPRINT(myChild4Facade->get<DoubleParentFloatTag>());
                    ENSURE(myChild4Facade->get<DoubleParentFloatTag>() == 12);

                    myChild4Facade->getNamedItem("doubleparentfloat")->nodeValueWrapperPtr()->printPrecursorGraph();
                    (*myChild2)["float"].nodeValueWrapperPtr()->printDependendGraph();

                    // now remove a child
                    ENSURE(myChild3->parentNode()->removeChild(myChild3));

                    ENSURE(myChild2->appendChild(myChild4) == myChild4);

                    myChild4Facade->getNamedItem("doubleparentfloat")->nodeValueWrapperPtr()->printPrecursorGraph();
                    (*myChild2)["float"].nodeValueWrapperPtr()->printDependendGraph();

                    ENSURE(myChild4Facade->get<DoubleParentFloatTag>() == 6);
                    DPRINT(myChild4Facade->get<DoubleParentFloatTag>());

                    myChild4Facade->getNamedItem("doubleparentfloat")->nodeValueWrapperPtr()->printPrecursorGraph();
                    (*myChild2)["float"].nodeValueWrapperPtr()->printDependendGraph();

                    ENSURE(myChild2Facade->set<FloatTag>(4) == 4);
                    myChild4Facade->getNamedItem("doubleparentfloat")->nodeValueWrapperPtr()->printPrecursorGraph();
                    (*myChild2)["float"].nodeValueWrapperPtr()->printDependendGraph();

                    ENSURE(myChild2Facade->isDirty<DoubleParentFloatTag>());
                    myChild4Facade->getNamedItem("doubleparentfloat")->nodeValueWrapperPtr()->printPrecursorGraph();
                    (*myChild2)["float"].nodeValueWrapperPtr()->printDependendGraph();

                    ENSURE(myChild4Facade->isDirty<DoubleParentFloatTag>());
                    myChild4Facade->getNamedItem("doubleparentfloat")->nodeValueWrapperPtr()->printPrecursorGraph();
                    (*myChild2)["float"].nodeValueWrapperPtr()->printDependendGraph();

                    ENSURE(myChild2Facade->get<DoubleParentFloatTag>() == 4);
                    ENSURE(myChild4Facade->get<DoubleParentFloatTag>() == 8);
                    DPRINT(myChild4Facade->get<DoubleParentFloatTag>());

                    // Add new child at runtime
                    {
                        NodePtr myChild5 = NodePtr(new dom::Element("child"));
                        ENSURE(myChild5->appendAttribute("id", "cc5"));
                        myChild5 = myChild4->appendChild(myChild5);
                        ENSURE(myChild5);
                        ChildFacadePtr myChild5Facade = myChild5->getFacade<ChildFacade>();
                        ENSURE(myChild5Facade);
                        ENSURE(myChild5Facade->get<DoubleParentFloatTag>() == 16);
                        ENSURE(myChild2Facade->set<FloatTag>(2) == 2);
                        ENSURE(myChild5Facade->isDirty<DoubleParentFloatTag>());
                        ENSURE(myChild5Facade->get<DoubleParentFloatTag>() == 8);
                    }

                    // Remove child and reinsert it again
                    {
                        NodePtr myChild5 = myChild4->childNode("child");
                        myChild5 = myChild4->appendChild(myChild5);
                        myChild5 = myChild4->appendChild(myChild5);
                        myChild5 = myChild4->appendChild(myChild5);
                        myChild5 = myChild4->appendChild(myChild5);
                        ChildFacadePtr myChild5Facade = myChild5->getFacade<ChildFacade>();
                        ENSURE(myChild5Facade);
                        ENSURE(myChild5Facade->get<DoubleParentFloatTag>() == 8);
                        ENSURE(myChild2Facade->set<FloatTag>(4) == 4);
                        AC_PRINT << "For setDirty: " << *myChild5;
                        ENSURE(myChild5Facade->isDirty<DoubleParentFloatTag>());
                        AC_PRINT << "-Nach setDirty: " << *myChild5;
                        ENSURE(myChild5Facade->get<DoubleParentFloatTag>() == 16);
                        myChild4->removeChild(myChild5);
                    }
                    // Add child, with dependency registered by parent
                    {
                        ENSURE(myChild4Facade->get<ChildFloatSumTag>() == 0);
                        NodePtr myChild6 = NodePtr(new dom::Element("child"));
                        myChild6->appendAttribute("id", "cc6");
                        ENSURE(myChild4->appendChild(myChild6) == myChild6);
                        ENSURE(myChild4Facade->isDirty<ChildFloatSumTag>());
                        ENSURE(myChild4Facade->get<ChildFloatSumTag>() == 1);
                        ChildFacadePtr myChild6Facade = myChild6->getFacade<ChildFacade>();
                        ENSURE(myChild6Facade);
                        ENSURE(myChild6Facade->set<FloatTag>(2) == 2);
                        ENSURE(myChild4Facade->get<ChildFloatSumTag>() == 2);
                    }
                }
            } catch (asl::Exception & e) {
                cerr << "#### failure:" << e << endl;
                FAILURE("Exception");
            } catch (...) {
                cerr << "#### unknown exception occured" <<  endl;
                FAILURE("unknown Exception");
            }
        }
};

class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
        void setup() {
            UnitTestSuite::setup();
            addTest(new FacadeUnitTest);
        }
};


int main(int argc, char *argv[]) {

#ifdef FULL_HEAP_DEBUG
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
    
    // Set the debug-heap flag to keep freed blocks in the
    // heap's linked list - This will allow us to catch any
    // inadvertent use of freed memory
    int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
    tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    _CrtSetDbgFlag(tmpDbgFlag);
#endif

    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();

    std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

