/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: Canvas.h,v $
//
//   $Revision: 1.3 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _scene_Canvas_h_
#define _scene_Canvas_h_

#include "IFrameBuffer.h"

#include <y60/CommonTags.h>
#include <y60/NodeNames.h>
#include <y60/DataTypes.h>
#include <dom/Facade.h>
#include <dom/AttributePlug.h>
namespace y60 {

    class Image;
    class Scene;

    //                  theTagName                theType        theAttributeName                    theDefault
    DEFINE_ATTRIBUT_TAG(CanvasBackgroundColorTag, asl::Vector4f, CANVAS_BACKGROUNDCOLOR_ATTRIB, asl::Vector4f(0,0,0,1));
    DEFINE_ATTRIBUT_TAG(CanvasTargetTag, std::string , CANVAS_TARGET_ATTRIB, "");

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
            unsigned getWidth() const;
            unsigned getHeight() const;
            bool setFrameBuffer(asl::Ptr<IFrameBuffer> theFrameBuffer);

            bool hasRenderTarget() const;
            asl::Ptr<Image, dom::ThreadingModel> getTarget(asl::Ptr<Scene, dom::ThreadingModel> theScene);

            IMPLEMENT_FACADE(Canvas);
        private:
            asl::WeakPtr<IFrameBuffer> _myFrameBuffer;
    };

    typedef asl::Ptr<Canvas, dom::ThreadingModel> CanvasPtr;
}

#endif
