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

*/

// own header
#include "TransformHierarchyFacade.h"

#include <asl/base/console_functions.h>

using namespace std;
using namespace asl;
using namespace dom;

// PM: The following lines introduce an unwanted forward dependency to jslib due to insufficient
// language barrier crossing design of the event payload - this should be fixed during a
// general event system overhaul that should unite the at least three different event types
// used throughout Y60
struct JSObject;
namespace jslib {
typedef dom::GenericEvent<JSObject*> GenericJSDomEvent;
}


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
        PivotTag::Plug(theNode),
        BillboardTag::Plug(theNode),
        BillboardLookatTag::Plug(theNode),
        RenderOrderTag::Plug(theNode),
        CullableTag::Plug(theNode),
        ClippingPlanesTag::Plug(theNode),
        ScissorTag::Plug(theNode),
        StickyTag::Plug(theNode),
        EventsTag::Plug(theNode),
        dom::FacadeAttributePlug<BoundingBoxTag>(this),
        dom::FacadeAttributePlug<GlobalMatrixTag>(this),
        dom::FacadeAttributePlug<LocalMatrixTag>(this),
        dom::FacadeAttributePlug<InverseGlobalMatrixTag>(this),
        dom::FacadeAttributePlug<LastActiveFrameTag>(this),
        _onRenderEventEnabled(false),
        _onFirstRenderEventEnabled(false)
    {}

    void
    TransformHierarchyFacade::registerDependenciesRegistrators() {
        AC_TRACE << "TransformHierarchyFacade::registerDependenciesRegistrators: this ="<<(void*)this;
        LocalMatrixTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForLocalMatrix);
        GlobalMatrixTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForGlobalMatrix);
        BoundingBoxTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForBoundingBox);
        InverseGlobalMatrixTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForInverseGlobalMatrix);
        EventsTag::Plug::setReconnectFunction(&TransformHierarchyFacade::registerDependenciesForEvents);
    }

    void
    TransformHierarchyFacade::registerDependenciesForEvents() {
        AC_TRACE << "TransformHierarchyFacade::registerDependenciesForEvents()";
        Node & myNode = getNode();
        if (myNode) {
            AC_TRACE << "TransformHierarchyFacade::registerDependenciesForEvents() - callback registered";
            EventsTag::Plug::getValuePtr()->setImmediateCallBack(dynamic_cast_Ptr<TransformHierarchyFacade>(getSelf()), &TransformHierarchyFacade::updateEventTrigger);
        }
    }

    void
    TransformHierarchyFacade::updateEventTrigger() {
        AC_TRACE << "TransformHierarchyFacade::updateEventTrigger()";
        Node & myNode = getNode();
        if (myNode) {
            const VectorOfString & myEvents = get<EventsTag>();
            _onRenderEventEnabled = false;
            _onFirstRenderEventEnabled = false;
            bool someEventEnabled = false;
            for (VectorOfString::size_type i = 0; i < myEvents.size();++i) {
                AC_TRACE << "TransformHierarchyFacade::updateEventTrigger() event="<<myEvents[i];
                if (myEvents[i] == "onRender") {
                    _onRenderEventEnabled = true;
                    someEventEnabled = true;
                } else if (myEvents[i] == "onFirstRender") {
                    _onFirstRenderEventEnabled = true;
                    someEventEnabled = true;
                } else {
                    AC_TRACE << "unknown or not builtin event trigger:" << myEvents[i];
                }
            }
            if (someEventEnabled) {
                LastActiveFrameTag::Plug::getValuePtr()->setImmediateCallBack(
                        dynamic_cast_Ptr<TransformHierarchyFacade>(getSelf()), &TransformHierarchyFacade::triggerRenderEvent);
            }
        }
    }

    void
    TransformHierarchyFacade::triggerRenderEvent() {
        AC_TRACE << "TransformHierarchyFacade::triggerRenderEvent()";
        Node & myNode = getNode();
        if (myNode) {
            if (_onFirstRenderEventEnabled) {
                AC_TRACE << "TransformHierarchyFacade::triggerRenderEvent(): dispatching dom event 'onFirstRender'";
                dom::EventPtr myEvent = dom::EventPtr(new jslib::GenericJSDomEvent("onFirstRender", false, false, true));
                myNode.dispatchEvent(myEvent);
                _onFirstRenderEventEnabled = false;
            }
            if (_onRenderEventEnabled) {
                AC_TRACE << "TransformHierarchyFacade::triggerRenderEvent(): dispatching dom event 'onRender'";
                //dom::EventPtr myEvent = dom::EventPtr(new RenderEvent("onRender", false, false, true));
                dom::EventPtr myEvent = dom::EventPtr(new jslib::GenericJSDomEvent("onRender", false, false, true));
                myNode.dispatchEvent(myEvent);
            }
        }
    }


    void
    TransformHierarchyFacade::registerDependenciesForLocalMatrix() {
        Node & myNode = getNode();
        if (myNode) {
            // local matrix
            LocalMatrixTag::Plug::dependsOn<PositionTag>(*this);
            LocalMatrixTag::Plug::dependsOn<OrientationTag>(*this);
            LocalMatrixTag::Plug::dependsOn<ScaleTag>(*this);
            LocalMatrixTag::Plug::dependsOn<PivotTag>(*this);

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
                    TransformHierarchyFacadePtr myChild = myChildNode->tryGetFacade<TransformHierarchyFacade>();
                    if (myChild) {
                        BoundingBoxTag::Plug::dependsOn<BoundingBoxTag>(*myChild);
                    }
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
        PRINTMESSAGE(getNode(), "Recalculating local matrix (copy local to global) @ " << (Field*)(LocalMatrixTag::Plug::getValuePtr().get()), TTYRED);
        asl::Matrix4f myMatrix = Matrix4f::Identity();
        myMatrix.scale(get<ScaleTag>());
        myMatrix.translate(-get<PivotTag>());
        Matrix4f myRotation(get<OrientationTag>());
        myMatrix.postMultiply(myRotation);
        myMatrix.translate(get<PivotTag>() + get<PositionTag>());
        set<LocalMatrixTag>(myMatrix);
    }

    void
    TransformHierarchyFacade::copyLocalToGlobalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating global matrix (copy local to global) @ " << (Field*)(GlobalMatrixTag::Plug::getValuePtr().get()), TTYRED);
        set<GlobalMatrixTag>(get<LocalMatrixTag>());
    }

    void
    TransformHierarchyFacade::recalculateGlobalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating global matrix @ " << (Field*)(GlobalMatrixTag::Plug::getValuePtr().get()), TTYRED);
        asl::Matrix4f myMatrix = get<LocalMatrixTag>();

        TransformHierarchyFacadePtr myParent = getNode().parentNode()->getFacade<TransformHierarchyFacade>();
        if (myParent) {
            myMatrix.postMultiply(myParent->get<GlobalMatrixTag>());
            set<GlobalMatrixTag>(myMatrix);
        }
    }

    void
    TransformHierarchyFacade::recalculateBoundingBox() {
        PRINTMESSAGE(getNode(), "Recalculating bounding matrix @ " << (Field*)(BoundingBoxTag::Plug::getValuePtr().get()), TTYGREEN);
        asl::Box3f myBoundingBox;
        myBoundingBox.makeEmpty();

        for (unsigned i = 0; i < getNode().childNodesLength(); ++i) {
            NodePtr myChildNode = getNode().childNode(i);
            if (myChildNode->nodeType() == Node::ELEMENT_NODE) {
                TransformHierarchyFacadePtr myChild = myChildNode->tryGetFacade<TransformHierarchyFacade>();
                if (myChild) {
                    myBoundingBox.extendBy(myChild->get<BoundingBoxTag>());
                }
            }
        }
        set<BoundingBoxTag>(myBoundingBox);
    }

    void
    TransformHierarchyFacade::recalculateInverseGlobalMatrix() {
        PRINTMESSAGE(getNode(), "Recalculating inverse global matrix @ " << (Field*)(InverseGlobalMatrixTag::Plug::getValuePtr().get()), TTYYELLOW);
        set<InverseGlobalMatrixTag>(asl::inverse(get<GlobalMatrixTag>()));
    }
}
