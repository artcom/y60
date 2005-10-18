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
//   $RCSfile: TransformHierarchyFacade.h,v $
//
//   $Revision: 1.18 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_TransformHierarchyFacades_h_
#define _xml_TransformHierarchyFacades_h_

#include <y60/CommonTags.h>
#include <y60/NodeNames.h>
#include <y60/DataTypes.h>

#include <dom/Facade.h>
#include <dom/AttributePlug.h>
#include <dom/PrivatePlug.h>

#include <asl/Matrix4.h>
#include <asl/Box.h>
#include <asl/Vector234.h>

namespace y60 {
    //                  theTagName           theType           theAttributeName               theDefault

    DEFINE_ATTRIBUT_TAG(PositionTag,         asl::Vector3f,    POSITION_ATTRIB,          asl::Vector3f(0,0,0));
    DEFINE_ATTRIBUT_TAG(OrientationTag,      asl::Quaternionf, ORIENTATION_ATTRIB,       asl::Quaternionf(0,0,0,1));
    DEFINE_ATTRIBUT_TAG(ScaleTag,            asl::Vector3f,    SCALE_ATTRIB,             asl::Vector3f(1,1,1));
    DEFINE_ATTRIBUT_TAG(ShearTag,            asl::Vector3f,    SHEAR_ATTRIB,             asl::Vector3f(0,0,0));
    DEFINE_ATTRIBUT_TAG(PivotTag,            asl::Vector3f,    PIVOT_ATTRIB,             asl::Vector3f(0,0,0));
    DEFINE_ATTRIBUT_TAG(PivotTranslationTag, asl::Vector3f,    PIVOT_TRANSLATION_ATTRIB, asl::Vector3f(0,0,0));
    DEFINE_ATTRIBUT_TAG(InsensibleTag,       bool,             INSENSIBLE_ATTRIB,        false);
    DEFINE_ATTRIBUT_TAG(LocalMatrixTag,      asl::Matrix4f,    LOCAL_MATRIX_ATTRIB,      asl::Matrix4f::Identity());
    DEFINE_ATTRIBUT_TAG(GlobalMatrixTag,     asl::Matrix4f,    GLOBAL_MATRIX_ATTRIB,     asl::Matrix4f::Identity());
    DEFINE_ATTRIBUT_TAG(InverseGlobalMatrixTag, asl::Matrix4f, INVERSE_GLOBAL_MATRIX_ATTRIB,     asl::Matrix4f::Identity());
    DEFINE_ATTRIBUT_TAG(BillboardTag,        std::string,      BILLBOARD_ATTRIB,         "");
    DEFINE_ATTRIBUT_TAG(SensorTag,           std::string,      SENSOR_ATTRIB,            "");
    DEFINE_ATTRIBUT_TAG(SensorRadiusTag,     float,            SENSOR_RADIUS_ATTRIB,     0);
    DEFINE_ATTRIBUT_TAG(CullableTag,         bool,             CULLABLE_ATTRIB,          true);
    DEFINE_ATTRIBUT_TAG(ClippingPlanesTag,   VectorOfString,   CLIPPING_PLANES_ATTRIB,   VectorOfString());

    class TransformHierarchyFacade :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public VisibleTag::Plug,
        public FrozenTag::Plug,
        public InsensibleTag::Plug,
        public PositionTag::Plug,
        public OrientationTag::Plug,
        public ScaleTag::Plug,
        public ShearTag::Plug,
        public PivotTag::Plug,
        public PivotTranslationTag::Plug,
        public BillboardTag::Plug,
        public SensorTag::Plug,
        public SensorRadiusTag::Plug,
        public CullableTag::Plug,
        public ClippingPlanesTag::Plug,
        public dom::FacadeAttributePlug<BoundingBoxTag>,
        public dom::FacadeAttributePlug<GlobalMatrixTag>,
        public dom::FacadeAttributePlug<LocalMatrixTag>,
        public dom::FacadeAttributePlug<InverseGlobalMatrixTag>
    {
        public:
            TransformHierarchyFacade(dom::Node & theNode);

            IMPLEMENT_FACADE(TransformHierarchyFacade);

            asl::Vector3f getViewVector() const;
            asl::Vector3f getUpVector() const;
            asl::Vector3f getRightVector() const;            
            virtual void registerDependenciesRegistrators();
        protected:
            void registerDependenciesForLocalMatrix();
            void registerDependenciesForGlobalMatrix();
            void registerDependenciesForBoundingBox();
            void registerDependenciesForInverseGlobalMatrix();

            void recalculateLocalMatrix();
            void recalculateGlobalMatrix();
            void copyLocalToGlobalMatrix();
            void recalculateBoundingBox();
            void recalculateInverseGlobalMatrix();
        private:
            void getLocalMatrix(asl::Matrix4f & theLocalMatrix) const;
        
    };

    typedef asl::Ptr<TransformHierarchyFacade, dom::ThreadingModel> TransformHierarchyFacadePtr;
}

#endif
