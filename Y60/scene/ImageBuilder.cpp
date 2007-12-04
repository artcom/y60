//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ImageBuilder.h"
#include <y60/NodeNames.h>
#include <y60/NodeValueNames.h>

#include <dom/Nodes.h>
#include <asl/Block.h>
#include <asl/Dashboard.h>
#include <asl/RunLengthEncoder.h>

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
    ImageBuilder::setType(ImageType theType) {
        getNode()->getFacade<Image>()->set<ImageTypeTag>(theType);
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
