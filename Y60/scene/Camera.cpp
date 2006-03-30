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

#include "Camera.h"

namespace y60 {
    
    Camera::Camera(dom::Node & theNode) 
        : TransformHierarchyFacade(theNode),
        HfovTag::Plug(theNode),
        OrthoWidthTag::Plug(theNode),
        NearPlaneTag::Plug(theNode),
        FarPlaneTag::Plug(theNode)
    {}
    Camera::~Camera() {}
} 
