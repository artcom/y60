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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _scene_Canvas_h_
#define _scene_Canvas_h_

#include "y60_scene_settings.h"

#include "IFrameBuffer.h"

#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>
#include <asl/dom/Facade.h>
#include <asl/dom/AttributePlug.h>
namespace y60 {

    class Texture;
    class Scene;
    class Viewport;
    typedef asl::Ptr<Viewport, dom::ThreadingModel> ViewportPtr;

    //                  theTagName                theType        theAttributeName                    theDefault
    DEFINE_ATTRIBUTE_TAG(CanvasBackgroundColorTag, asl::Vector4f, CANVAS_BACKGROUNDCOLOR_ATTRIB, asl::Vector4f(0,0,0,1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(CanvasTargetsTag,          y60::VectorOfString,   CANVAS_TARGETS_ATTRIB, y60::VectorOfString(), Y60_SCENE_DECL);

    class Y60_SCENE_DECL Canvas :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public CanvasBackgroundColorTag::Plug,
        public CanvasTargetsTag::Plug
    {
        public:
            Canvas(dom::Node & theNode) :
              dom::Facade(theNode),
                  IdTag::Plug(theNode),
                  NameTag::Plug(theNode),
                  CanvasBackgroundColorTag::Plug(theNode),
                  CanvasTargetsTag::Plug(theNode)
              {}
            IMPLEMENT_FACADE(Canvas);

            unsigned getWidth() const;
            unsigned getHeight() const;
            bool setFrameBuffer(asl::Ptr<IFrameBuffer> theFrameBuffer);

            bool hasRenderTarget() const;
            std::vector<asl::Ptr<Texture, dom::ThreadingModel> > getTargets(asl::Ptr<Scene, dom::ThreadingModel> theScene) const;
            ViewportPtr getViewportAt(const unsigned int theX, const unsigned int theY) const;
        private:
            asl::WeakPtr<IFrameBuffer> _myFrameBuffer;
    };

    typedef asl::Ptr<Canvas, dom::ThreadingModel> CanvasPtr;
}

#endif
