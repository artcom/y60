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

#ifndef _scene_Overlay_h_
#define _scene_Overlay_h_

#include "y60_scene_settings.h"

#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>
#include <asl/math/Vector234.h>
#include <asl/dom/AttributePlug.h>

namespace y60 {

  //DEFINE_ATTRIBUTE_TAG(theTagName       theType        theAttributeName             theDefault,                theExportToken);
    DEFINE_ATTRIBUTE_TAG(WidthTag,        float,         WIDTH_ATTRIB,                0.0f,                      Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(HeightTag,       float,         HEIGHT_ATTRIB,               0.0f,                      Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(MaterialTag,     std::string,   MATERIAL_REF_ATTRIB,         "",                        Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(AlphaTag,        float,         OVERLAY_ALPHA_ATTRIB,        1.0f,                      Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(SrcOriginTag,    asl::Vector2f, OVERLAY_SRCORIGIN_ATTRIB,    asl::Vector2f(0.f,0.f),    Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(SrcSizeTag,      asl::Vector2f, OVERLAY_SRCSIZE_ATTRIB,      asl::Vector2f(1,1),        Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(BorderColorTag,  asl::Vector4f, OVERLAY_BORDERCOLOR_ATTRIB,  asl::Vector4f(0,0,0,1),    Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(BorderWidthTag,  float,         OVERLAY_BORDERWIDTH_ATTRIB,  1,                         Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TopBorderTag,    bool,          OVERLAY_TOPBORDER_ATTRIB,    false,                     Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(BottomBorderTag, bool,          OVERLAY_BOTTOMBORDER_ATTRIB, false,                     Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(LeftBorderTag,   bool,          OVERLAY_LEFTBORDER_ATTRIB,   false,                     Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(RightBorderTag,  bool,          OVERLAY_RIGHTBORDER_ATTRIB,  false,                     Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(Rotation2DTag,   float,         OVERLAY_ROTATION2D_ATTRIB,   0.0f,                      Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(Scale2DTag,      asl::Vector2f, OVERLAY_SCALE2D_ATTRIB,      asl::Vector2f(1.0f,1.0f),  Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(Pivot2DTag,      asl::Vector2f, OVERLAY_PIVOT2D_ATTRIB,      asl::Vector2f(0.0f,0.0f),  Y60_SCENE_DECL);

    class Overlay :
        public dom::Facade,
        public NameTag::Plug,
        public VisibleTag::Plug,
        public Position2DTag::Plug,
        public MaterialTag::Plug,
        public AlphaTag::Plug,
        public WidthTag::Plug,
        public HeightTag::Plug,
        public SrcOriginTag::Plug,
        public SrcSizeTag::Plug,
        public BorderWidthTag::Plug,
        public BorderColorTag::Plug,
        public TopBorderTag::Plug,
        public BottomBorderTag::Plug,
        public LeftBorderTag::Plug,
        public RightBorderTag::Plug,
        public Rotation2DTag::Plug,
        public Scale2DTag::Plug,
        public Pivot2DTag::Plug
    {
        public:
            Overlay(dom::Node & theNode) :
              dom::Facade(theNode),
                  NameTag::Plug(theNode),
                  VisibleTag::Plug(theNode),
                  Position2DTag::Plug(theNode),
                  MaterialTag::Plug(theNode),
                  AlphaTag::Plug(theNode),
                  WidthTag::Plug(theNode),
                  HeightTag::Plug(theNode),
                  SrcOriginTag::Plug(theNode),
                  SrcSizeTag::Plug(theNode),
                  BorderWidthTag::Plug(theNode),
                  BorderColorTag::Plug(theNode),
                  TopBorderTag::Plug(theNode),
                  BottomBorderTag::Plug(theNode),
                  LeftBorderTag::Plug(theNode),
                  RightBorderTag::Plug(theNode),
                  Rotation2DTag::Plug(theNode),
                  Scale2DTag::Plug(theNode),
                  Pivot2DTag::Plug(theNode)
              {}
            IMPLEMENT_FACADE(Overlay);
    };

    typedef asl::Ptr<Overlay, dom::ThreadingModel> OverlayPtr;
}

#endif
