//=============================================================================
// Copyright (C) 2003-2007 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _scene_Overlay_h_
#define _scene_Overlay_h_

#include <y60/CommonTags.h>
#include <y60/NodeNames.h>
#include <y60/DataTypes.h>
#include <asl/Vector234.h>
#include <dom/AttributePlug.h>

namespace y60 {
    
  //DEFINE_ATTRIBUT_TAG(theTagName       theType        theAttributeName             theDefault);
    DEFINE_ATTRIBUT_TAG(WidthTag,        float,         WIDTH_ATTRIB,                0.0f);
    DEFINE_ATTRIBUT_TAG(HeightTag,       float,         HEIGHT_ATTRIB,               0.0f);
    DEFINE_ATTRIBUT_TAG(MaterialTag,     std::string,   MATERIAL_REF_ATTRIB,         "");
    DEFINE_ATTRIBUT_TAG(AlphaTag,        float,         OVERLAY_ALPHA_ATTRIB,        1.0f);
    DEFINE_ATTRIBUT_TAG(SrcOriginTag,    asl::Vector2f, OVERLAY_SRCORIGIN_ATTRIB,    asl::Vector2f(0.f,0.f));
    DEFINE_ATTRIBUT_TAG(SrcSizeTag,      asl::Vector2f, OVERLAY_SRCSIZE_ATTRIB,      asl::Vector2f(1,1));
    DEFINE_ATTRIBUT_TAG(BorderColorTag,  asl::Vector4f, OVERLAY_BORDERCOLOR_ATTRIB,  asl::Vector4f(0,0,0,1));
    DEFINE_ATTRIBUT_TAG(BorderWidthTag,  float,         OVERLAY_BORDERWIDTH_ATTRIB,  1);
    DEFINE_ATTRIBUT_TAG(TopBorderTag,    bool,          OVERLAY_TOPBORDER_ATTRIB,    false);
    DEFINE_ATTRIBUT_TAG(BottomBorderTag, bool,          OVERLAY_BOTTOMBORDER_ATTRIB, false);
    DEFINE_ATTRIBUT_TAG(LeftBorderTag,   bool,          OVERLAY_LEFTBORDER_ATTRIB,   false);
    DEFINE_ATTRIBUT_TAG(RightBorderTag,  bool,          OVERLAY_RIGHTBORDER_ATTRIB,  false);
    DEFINE_ATTRIBUT_TAG(Rotation2DTag,   float,         OVERLAY_ROTATION2D_ATTRIB,   0.0f);
    DEFINE_ATTRIBUT_TAG(Scale2DTag,      asl::Vector2f, OVERLAY_SCALE2D_ATTRIB,      asl::Vector2f(1.0f,1.0f));
    DEFINE_ATTRIBUT_TAG(Pivot2DTag,      asl::Vector2f, OVERLAY_PIVOT2D_ATTRIB,      asl::Vector2f(0.0f,0.0f));

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
