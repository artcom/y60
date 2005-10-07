/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: Facades.h,v $
//
//   $Revision: 1.20 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _scene_Facades_h_
#define _scene_Facades_h_

#include "TransformHierarchyFacade.h"
#include "LodFacade.h"
#include "Camera.h"
#include "Body.h"
#include "MaterialBase.h"
#include "Light.h"
#include "LightSource.h"
#include "Canvas.h"
#include "Viewport.h"
#include "Overlay.h"
#include "Shape.h"
#include "IncludeFacade.h"

#include <y60/Image.h>
#include <y60/Movie.h>
#include <y60/Capture.h>

#include <dom/Nodes.h>
#include <asl/Matrix4.h>

namespace y60 {

    //                  theTagName           theType           theAttributeName               theDefault
    DEFINE_ATTRIBUT_TAG(SkyBoxMaterialTag,   std::string,      SKYBOX_MATERIAL_ATTRIB,   "");
    DEFINE_ATTRIBUT_TAG(LodScaleTag,         float,            LODSCALE_ATTRIB,          1);
    DEFINE_ATTRIBUT_TAG(FogModeTag,          std::string,      FOGMODE_ATTRIB,           "");
    DEFINE_ATTRIBUT_TAG(FogColorTag,         asl::Vector4f,    FOGCOLOR_ATTRIB,          asl::Vector4f(0,0,0,0));
    DEFINE_ATTRIBUT_TAG(FogRangeTag,         asl::Vector2f,    FOGRANGE_ATTRIB,          asl::Vector2f(0, 1));
    DEFINE_ATTRIBUT_TAG(FogDensityTag,       float,            FOGDENSITY_ATTRIB,        1);

    class WorldFacade :
        public TransformHierarchyFacade,
        public SkyBoxMaterialTag::Plug,
        public LodScaleTag::Plug,
        public FogModeTag::Plug,
        public FogColorTag::Plug,
        public FogRangeTag::Plug,
        public FogDensityTag::Plug
    {
        public:
            WorldFacade(dom::Node & theNode) :
                TransformHierarchyFacade(theNode),
                SkyBoxMaterialTag::Plug(theNode),
                LodScaleTag::Plug(theNode),
                FogModeTag::Plug(theNode),
                FogColorTag::Plug(theNode),
                FogRangeTag::Plug(theNode),
                FogDensityTag::Plug(theNode)
        {}
        IMPLEMENT_FACADE(WorldFacade);
    };

    typedef asl::Ptr<WorldFacade, dom::ThreadingModel> WorldFacadePtr;

    class TransformFacade :
        public TransformHierarchyFacade
    {
    public:
        TransformFacade(dom::Node & theNode) : TransformHierarchyFacade(theNode) {}
        IMPLEMENT_FACADE(TransformFacade);
    };


    //                  theTagName           theType           theAttributeName               theDefault
    DEFINE_ATTRIBUT_TAG(JointOrientationTag, asl::Quaternionf, JOINT_ORIENTATION_ATTRIB, asl::Quaternionf(0,0,0,1));

    typedef asl::Ptr<TransformFacade, dom::ThreadingModel> TransformFacadePtr;

    class JointFacade :
        public TransformHierarchyFacade,
        public JointOrientationTag::Plug
    {
    public:
        JointFacade(dom::Node & theNode) :
            TransformHierarchyFacade(theNode),
            JointOrientationTag::Plug(theNode)
        {}

        IMPLEMENT_FACADE(JointFacade);

        void registerDependenciesRegistrators() {
            TransformHierarchyFacade::registerDependenciesRegistrators();
            LocalMatrixTag::Plug::setReconnectFunction(&JointFacade::registerDependenciesForLocalMatrix);
        }

        void registerDependenciesForLocalMatrix() {
            dom::Node & myNode = getNode();
            if (myNode) {
                // local matrix
                TransformHierarchyFacade::registerDependenciesForLocalMatrix();
                LocalMatrixTag::Plug::dependsOn<JointOrientationTag>(*this);

                LocalMatrixTag::Plug::setCalculatorFunction(&JointFacade::recalculateLocalMatrix);
            }
        }

    protected:
        void recalculateLocalMatrix() {
            asl::Matrix4f myMatrix = asl::Matrix4f::Identity();
            myMatrix.scale(get<ScaleTag>());
            myMatrix.translate(-get<PivotTag>());
            asl::Matrix4f myRotation(get<OrientationTag>());
            myMatrix.postMultiply(myRotation);
            asl::Matrix4f myJointRotation(get<JointOrientationTag>());
            myMatrix.postMultiply(myJointRotation);
            myMatrix.translate(get<PositionTag>() + get<PivotTag>() + get<PivotTranslationTag>());
            set<LocalMatrixTag>(myMatrix);
        }
    };
    typedef asl::Ptr<JointFacade, dom::ThreadingModel> JointFacadePtr;

    inline void registerSceneFacades(dom::FacadeFactoryPtr theFactory) {
        theFactory->registerPrototype("world", dom::FacadePtr(new WorldFacade(dom::Node::Prototype)));
        theFactory->registerPrototype("body", dom::FacadePtr(new Body(dom::Node::Prototype)));
        theFactory->registerPrototype("transform", dom::FacadePtr(new TransformFacade(dom::Node::Prototype)));
        theFactory->registerPrototype("lod", dom::FacadePtr(new LodFacade(dom::Node::Prototype)));
        theFactory->registerPrototype("light", dom::FacadePtr(new Light(dom::Node::Prototype)));
        theFactory->registerPrototype("lightsource", dom::FacadePtr(new LightSource(dom::Node::Prototype)));
        theFactory->registerPrototype("properties", dom::FacadePtr(new LightPropertiesFacade(dom::Node::Prototype)), "lightsource");
        theFactory->registerPrototype("camera", dom::FacadePtr(new Camera(dom::Node::Prototype)));
        theFactory->registerPrototype("joint", dom::FacadePtr(new JointFacade(dom::Node::Prototype)));
        theFactory->registerPrototype("canvas", dom::FacadePtr(new Canvas(dom::Node::Prototype)));
        theFactory->registerPrototype("viewport", dom::FacadePtr(new Viewport(dom::Node::Prototype)));
        theFactory->registerPrototype("overlay", dom::FacadePtr(new Overlay(dom::Node::Prototype)));
        theFactory->registerPrototype("image", dom::FacadePtr(new Image(dom::Node::Prototype)));
        theFactory->registerPrototype("movie", dom::FacadePtr(new Movie(dom::Node::Prototype)));
        theFactory->registerPrototype("capture", dom::FacadePtr(new Capture(dom::Node::Prototype)));
        theFactory->registerPrototype("shape", dom::FacadePtr(new Shape(dom::Node::Prototype)));
        theFactory->registerPrototype("include", dom::FacadePtr(new IncludeFacade(dom::Node::Prototype)));
        theFactory->registerPrototype("material", dom::FacadePtr(new MaterialBase(dom::Node::Prototype)));
        theFactory->registerPrototype("properties", dom::FacadePtr(new MaterialPropertiesFacade(dom::Node::Prototype)), "material");
        theFactory->registerPrototype("requires", dom::FacadePtr(new MaterialRequirementFacade(dom::Node::Prototype)), "material");
    }

/*
    template <class T>
    inline asl::Ptr<T, dom::ThreadingModel>
    createFacade(dom::NodePtr theParent) {
        dom::NodePtr myNode = theParent.appendChild(dom::NodePtr(new dom::Element(T::name)));
        return myNode->getFacade<T>();
    }

    template <class Body>
    inline asl::Ptr<Body, dom::ThreadingModel>
    createFacade(dom::NodePtr theParent, const std::string & theShapeId) {
        dom::NodePtr myNode = dom::NodePtr(new dom::Element(Body::name()));
        myNode.appendAttribute(BODY_SHAPE_ATTRIB, theShapeId);
        myNode = theParent->appendChild(myNode);
        return myNode->getFacade<Body>();
    }
    */
}

#endif
