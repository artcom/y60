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

// own header
#include "ImageBuilder.h"

#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>

#include <asl/dom/Nodes.h>
#include <asl/base/Block.h>
#include <asl/base/Dashboard.h>
#include <asl/base/RunLengthEncoder.h>

namespace y60 {
    ImageBuilder::ImageBuilder(const std::string & theName)
        : BuilderBase(IMAGE_NODE_NAME)
    {
        init(theName);

    }
    ImageBuilder::ImageBuilder(const std::string & theNodeName,
                               const std::string & theName) : BuilderBase(theNodeName)
    {
        init(theName);
    }

    ImageBuilder::~ImageBuilder() {
    }

    void
    ImageBuilder::init(const std::string & theName) {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            //TODO: the follwoing attributes are all appended as text and not as type
            // because there is no schema available at this time
            myNode->appendAttribute(NAME_ATTRIB, theName);
            myNode->appendAttribute(IMAGE_TYPE_ATTRIB, IMAGE_TYPE_SINGLE);
            myNode->appendAttribute(DEPTH_ATTRIB, (unsigned int)(1));
            myNode->appendAttribute(IMAGE_TILE_ATTRIB, asl::Vector2i(1,1));
        } else {
            myNode->getFacade<Image>()->set<NameTag>(theName);
        }
    }

    const std::string &
    ImageBuilder::getName() const {
        return getNode()->getAttribute(NAME_ATTRIB)->nodeValue();
    }

    void
    ImageBuilder::setImage(const std::string & theFileName, const std::string & theFilter,
                           const std::string & theResizeMode)
    {
        getNode()->getFacade<Image>()->set<ImageSourceTag>(theFileName);
        getNode()->getFacade<Image>()->set<ImageFilterTag>(theFilter);
    }

    void
    ImageBuilder::inlineImage(const std::string & theFileName, ImageFilter theFilter, const std::string & theResizeMode) {
        MAKE_SCOPE_TIMER(ImageBuilder_inlineImage);
        setImage(theFileName, asl::getStringFromEnum(theFilter, ImageFilterStrings), theResizeMode);

        getNode()->getFacade<Image>()->load();
    }

    void
    ImageBuilder::createFileReference(const std::string & theFileName, const std::string & theResizeMode) {
        setImage(theFileName, "", theResizeMode);
    }

    void
    ImageBuilder::setDepth(unsigned int theDepth) {
        getNode()->getFacade<Image>()->set<ImageDepthTag>(theDepth);
    }

    void
    ImageBuilder::setTiling(asl::Vector2i theTiling) {
        getNode()->getFacade<Image>()->set<ImageTileTag>(theTiling);
    }
}
