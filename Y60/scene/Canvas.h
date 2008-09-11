//============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _scene_Canvas_h_
#define _scene_Canvas_h_

#include "IFrameBuffer.h"

#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>
#include <asl/dom/Facade.h>
#include <asl/dom/AttributePlug.h>
namespace y60 {

    class Texture;
    class Scene;

    //                  theTagName                theType        theAttributeName                    theDefault
    DEFINE_ATTRIBUT_TAG(CanvasBackgroundColorTag, asl::Vector4f, CANVAS_BACKGROUNDCOLOR_ATTRIB, asl::Vector4f(0,0,0,1));
    DEFINE_ATTRIBUT_TAG(CanvasTargetTag,          std::string,   CANVAS_TARGET_ATTRIB, "");

    class Canvas :
        public dom::Facade,
		public IdTag::Plug,
		public NameTag::Plug,
		public CanvasBackgroundColorTag::Plug,
		public CanvasTargetTag::Plug
    {
        public:
            Canvas(dom::Node & theNode) :
              dom::Facade(theNode),
                  IdTag::Plug(theNode),
                  NameTag::Plug(theNode),
                  CanvasBackgroundColorTag::Plug(theNode),
                  CanvasTargetTag::Plug(theNode)
              {}
            IMPLEMENT_FACADE(Canvas);

            unsigned getWidth() const;
            unsigned getHeight() const;
            bool setFrameBuffer(asl::Ptr<IFrameBuffer> theFrameBuffer);

            bool hasRenderTarget() const;
            asl::Ptr<Texture, dom::ThreadingModel> getTarget(asl::Ptr<Scene, dom::ThreadingModel> theScene);

        private:
            asl::WeakPtr<IFrameBuffer> _myFrameBuffer;
    };

    typedef asl::Ptr<Canvas, dom::ThreadingModel> CanvasPtr;
}

#endif
