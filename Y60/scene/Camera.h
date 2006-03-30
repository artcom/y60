//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Camera.h,v $
//   $Author: danielk $
//   $Revision: 1.16 $
//   $Date: 2005/03/17 18:22:42 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _scene_Camera_h_
#define _scene_Camera_h_

#include "TransformHierarchyFacade.h"

namespace y60 {

    //                  theTagName        theType  theAttributeName      theDefault
    DEFINE_ATTRIBUT_TAG(HfovTag,          float,   HORIZONTAL_FOV_ATTRIB, 54.0f);
    DEFINE_ATTRIBUT_TAG(OrthoWidthTag,    float,   ORTHO_WIDTH_ATTRIB,    0.0f);
    DEFINE_ATTRIBUT_TAG(NearPlaneTag,     float,   NEAR_PLANE_ATTRIB,     0.1f);
    DEFINE_ATTRIBUT_TAG(FarPlaneTag,      float,   FAR_PLANE_ATTRIB,      10000.0f);

    class Camera : 
        public TransformHierarchyFacade,
        public HfovTag::Plug,
        public OrthoWidthTag::Plug,
        public NearPlaneTag::Plug,
        public FarPlaneTag::Plug
    {
    public:
        Camera(dom::Node & theNode); 
        ~Camera(); 
            IMPLEMENT_FACADE(Camera);
   };

    typedef asl::Ptr<Camera, dom::ThreadingModel> CameraPtr;
} 

#endif 
