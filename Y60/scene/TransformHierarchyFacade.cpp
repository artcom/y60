/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: TransformHierarchyFacade.cpp,v $
//
//   $Revision: 1.5 $
//
//   Description: 
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "TransformHierarchyFacade.h"
#include <asl/console_functions.h>

using namespace std;
using namespace asl;
using namespace dom;


namespace y60 {
 
#if 1
    // do not print debug messages
    #define PRINTMESSAGE(theNode, theMessage, theColor)
    #define DB(x) // x
#else   
    // print debug messages
    #define PRINTMESSAGE(theNode, theMessage, theColor) \
            if (AC_DEBUG_ON) cerr << theNode.nodeName() << " (" << theNode.getAttributeString("name") << "): "\
                << theColor << theMessage << ENDCOLOR << endl;
    #define DB(x) x
#endif


 
    TransformHierarchyFacade::TransformHierarchyFacade(dom::Node & theNode) : dom::Facade(theNode),
        IdTag::Plug(theNode),
        NameTag::Plug(theNode),
        VisibleTag::Plug(theNode),
        InsensibleTag::Plug(theNode),
        PositionTag::Plug(theNode),
        OrientationTag::Plug(theNode),
        ScaleTag::Plug(theNode),
        ShearTag::Plug(theNode),
        PivotTag::Plug(theNode),
        PivotTranslationTag::Plug(theNode),
        BillboardTag::Plug(theNode),
        SensorTag::Plug(theNode),
        SensorRadiusTag::Plug(theNode),
        CullableTag::Plug(theNode),
        ClippingPlanesTag::Plug(theNode),
        dom::FacadeAttributePlug<BoundingBoxTag>(this),
        dom::FacadeAttributePlug<GlobalMatrixTag>(this),
        dom::FacadeAttributePlug<LocalMatrixTag>(this),
        dom::FacadeAttributePlug<InverseGlobalMatrixTag>(this)        
    {}

    void
    TransformHierarchyFacade::registerDependenciesRegistrators() {
        AC_TRACE << "TransformHierarchyFacade::registerDependenciesRegistrators: this ="<<(void*)this; 
        LocalMatrixTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForLocalMatrix);
        GlobalMatrixTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForGlobalMatrix);
        BoundingBoxTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForBoundingBox);
        InverseGlobalMatrixTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForInverseGlobalMatrix);
    }

    void
    TransformHierarchyFacade::registerDependenciesForLocalMatrix() {
        Node & myNode = getNode();
        if (myNode) {
            // local matrix
            LocalMatrixTag::Plug::dependsOn<PositionTag>(*this);
            LocalMatrixTag::Plug::dependsOn<OrientationTag>(*this);
            LocalMatrixTag::Plug::dependsOn<ScaleTag>(*this);
            LocalMatrixTag::Plug::dependsOn<ShearTag>(*this);
            LocalMatrixTag::Plug::dependsOn<PivotTag>(*this);
            LocalMatrixTag::Plug::dependsOn<PivotTranslationTag>(*this);  

            LocalMatrixTag::Plug::setCalculatorFunction(&TransformHierarchyFacade::recalculateLocalMatrix);
        }
    }
    void
    TransformHierarchyFacade::registerDependenciesForGlobalMatrix() {
        Node & myNode = getNode();
        if (myNode) {
            // global matrix
            GlobalMatrixTag::Plug::dependsOn<LocalMatrixTag>(*this);

            if (myNode.parentNode() && myNode.parentNode()->nodeName() != WORLD_LIST_NAME) {
                TransformHierarchyFacadePtr myParent = myNode.parentNode()->getFacade<TransformHierarchyFacade>();                
                GlobalMatrixTag::Plug::dependsOn<GlobalMatrixTag>(*myParent);                  
                GlobalMatrixTag::Plug::setCalculatorFunction(&TransformHierarchyFacade::recalculateGlobalMatrix);
            } else {
                GlobalMatrixTag::Plug::setCalculatorFunction(&TransformHierarchyFacade::copyLocalToGlobalMatrix);
            }
        }
    }

    void
    TransformHierarchyFacade::registerDependenciesForBoundingBox() {
        Node & myNode = getNode();
        if (myNode) {
            // bounding box
            BoundingBoxTag::Plug::dependsOn<GlobalMatrixTag>(*this);  
            BoundingBoxTag::Plug::dependsOn<VisibleTag>(*this);  

#ifdef OLD_PARENT_DEPENDENCY_UPDATE
            if (myNode.parentNode() && myNode.parentNode()->nodeName() != WORLD_LIST_NAME) {
                TransformHierarchyFacadePtr myParent = myNode.parentNode()->getFacade<TransformHierarchyFacade>();                
                BoundingBoxTag::Plug::affects<BoundingBoxTag>(*myParent);  
            }
#else
            for (unsigned i = 0; i < myNode.childNodesLength(); ++i) {
                NodePtr myChildNode = myNode.childNode(i);
                if (myChildNode->nodeType() == dom::Node::ELEMENT_NODE) {
                    TransformHierarchyFacadePtr myChild = myChildNode->getFacade<TransformHierarchyFacade>();                
                    BoundingBoxTag::Plug::dependsOn<BoundingBoxTag>(*myChild);  
                }
            }
#endif
            BoundingBoxTag::Plug::setCalculatorFunction(&TransformHierarchyFacade::recalculateBoundingBox);
        }
    }

    void
    TransformHierarchyFacade::registerDependenciesForInverseGlobalMatrix() {
        Node & myNode = getNode();
        if (myNode) {
            // inverse global matrix
            InverseGlobalMatrixTag::Plug::dependsOn<GlobalMatrixTag>(*this);  
            InverseGlobalMatrixTag::Plug::setCalculatorFunction(&TransformHierarchyFacade::recalculateInverseGlobalMatrix);

            DB(if (AC_TRACE_ON){
                PRINTMESSAGE (getNode(),"TransformHierarchyFacade::registerDependencies(): Dependencies of InverseGlobalMatrixTag of facade", TTYYELLOW);
                InverseGlobalMatrixTag::Plug::getValuePtr()->printPrecursorGraph();
                //debug<GlobalMatrixTag>();
            })
        }
    }
   
    asl::Vector3f
    TransformHierarchyFacade::getViewVector() const {
        const asl::Vector4f & myViewVector = get<GlobalMatrixTag>().getRow(2);
        return normalized(asl::Vector3f(myViewVector[0], myViewVector[1], -1.0f * myViewVector[2]));
    }

    asl::Vector3f
    TransformHierarchyFacade::getUpVector() const {
        const asl::Vector4f & myViewVector = get<GlobalMatrixTag>().getRow(1);
        return normalized(asl::Vector3f(myViewVector[0], myViewVector[1], -1.0f * myViewVector[2]));
    }

    asl::Vector3f
    TransformHierarchyFacade::getRightVector() const {
        const asl::Vector4f & myViewVector = get<GlobalMatrixTag>().getRow(0);
        return normalized(asl::Vector3f(myViewVector[0], myViewVector[1], -1.0f * myViewVector[2]));
    }

    void 
    TransformHierarchyFacade::recalculateLocalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating local matrix (copy local to global) @ " << (Field*)(&*LocalMatrixTag::Plug::getValuePtr()), TTYRED);
        asl::Matrix4f myMatrix = Matrix4f::Identity(); 
        myMatrix.scale(get<ScaleTag>());
        myMatrix.translate(-get<PivotTag>());
        Matrix4f myRotation(get<OrientationTag>());
        myMatrix.postMultiply(myRotation);
        myMatrix.translate(get<PositionTag>() + get<PivotTag>() + get<PivotTranslationTag>());
        set<LocalMatrixTag>(myMatrix);
    }

    void 
    TransformHierarchyFacade::copyLocalToGlobalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating global matrix (copy local to global) @ " << (Field*)(&*GlobalMatrixTag::Plug::getValuePtr()), TTYRED);
        set<GlobalMatrixTag>(get<LocalMatrixTag>());
    }

    void
    TransformHierarchyFacade::recalculateGlobalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating global matrix @ " << (Field*)(&*GlobalMatrixTag::Plug::getValuePtr()), TTYRED);
        asl::Matrix4f myMatrix = get<LocalMatrixTag>();

        TransformHierarchyFacadePtr myParent = getNode().parentNode()->getFacade<TransformHierarchyFacade>();
        if (myParent) {
            myMatrix.postMultiply(myParent->get<GlobalMatrixTag>());  
            set<GlobalMatrixTag>(myMatrix);
        }
    }
    
    void
    TransformHierarchyFacade::recalculateBoundingBox() {
        PRINTMESSAGE(getNode(), "Recalculating bounding matrix @ " << (Field*)(&*BoundingBoxTag::Plug::getValuePtr()), TTYGREEN);
        asl::Box3f myBoundingBox;
        myBoundingBox.makeEmpty();

        if (get<VisibleTag>()) {
            // Extend by child bounding boxes
            for (unsigned i = 0; i < getNode().childNodesLength(); ++i) {
                NodePtr myChildNode = getNode().childNode(i);
                if (myChildNode == Node::ELEMENT_NODE) {
                    TransformHierarchyFacadePtr myChild = myChildNode->getFacade<TransformHierarchyFacade>();
                    myBoundingBox.extendBy(myChild->get<BoundingBoxTag>());
                }
            }
        }
        set<BoundingBoxTag>(myBoundingBox);        
    }
    
    void
    TransformHierarchyFacade::recalculateInverseGlobalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating inverse global matrix @ " << (Field*)(&*InverseGlobalMatrixTag::Plug::getValuePtr()), TTYYELLOW);
        set<InverseGlobalMatrixTag>(asl::inverse(get<GlobalMatrixTag>()));
    }
}
