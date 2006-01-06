//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: ImageBuilder.cpp,v 1.4 2005/04/29 13:37:56 martin Exp $
//   $RCSfile: ImageBuilder.cpp,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2005/04/29 13:37:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "ImageBuilder.h"
#include <y60/NodeNames.h>
#include <y60/NodeValueNames.h>

#include <dom/Nodes.h>
#include <asl/Block.h>
#include <asl/Dashboard.h>
#include <asl/RunLengthEncoder.h>

namespace y60 {
    ImageBuilder::ImageBuilder(const std::string & theName, bool theCreateMipmapFlag)
        : BuilderBase(IMAGE_NODE_NAME)
    {
		init(theName, theCreateMipmapFlag);

    }
	ImageBuilder::ImageBuilder(const std::string & theNodeName, 
		                       const std::string & theName, 
							   bool theCreateMipmapFlag) : BuilderBase(theNodeName) 
	{
		init(theName, theCreateMipmapFlag);
	}

    ImageBuilder::~ImageBuilder() {
    }

	void
    ImageBuilder::init(const std::string & theName, bool theCreateMipmapFlag) {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            //TODO: the follwoing attributes are all appended as text and not as type
            // because there is no schema available at this time
            myNode->appendAttribute(NAME_ATTRIB, theName);
            myNode->appendAttribute(IMAGE_TYPE_ATTRIB, IMAGE_TYPE_SINGLE);
            myNode->appendAttribute(IMAGE_MIPMAP_ATTRIB, theCreateMipmapFlag);
            myNode->appendAttribute(DEPTH_ATTRIB, (unsigned int)(1));
            myNode->appendAttribute(IMAGE_COLOR_SCALE_ATTRIB, asl::Vector4f(1.0f,1.0f,1.0f,1.0f));
            myNode->appendAttribute(IMAGE_COLOR_BIAS_ATTRIB, asl::Vector4f(0.0f,0.0f,0.0f,0.0f));
            myNode->appendAttribute(IMAGE_TILE_ATTRIB, asl::Vector2i(1,1));
            myNode->appendAttribute(IMAGE_INTERNAL_FORMAT_ATTRIB, "");
		} else {
            myNode->getFacade<Image>()->set<NameTag>(theName);
		}
	}

    const std::string &
    ImageBuilder::getName() const {
        return getNode()->getAttribute(NAME_ATTRIB)->nodeValue();
    }


    void
    ImageBuilder::inlineImage(const std::string & theFileName, ImageFilter theFilter) {
        MAKE_SCOPE_TIMER(ImageBuilder_inlineImage);

        ImagePtr myImage = getNode()->getFacade<Image>();
        myImage->set<ImageSourceTag>(theFileName);
        myImage->set<ImageFilterTag>(asl::getStringFromEnum(theFilter, ImageFilterStrings));
        myImage->load();
    }

    void
    ImageBuilder::createFileReference(const std::string & theFileName) {
        ImagePtr myImage = getNode()->getFacade<Image>();
        myImage->set<ImageSourceTag>(theFileName);
    }

    void
    ImageBuilder::setType(ImageType theType) {
        getNode()->getAttribute(IMAGE_TYPE_ATTRIB)->nodeValue(asl::getStringFromEnum(theType, ImageTypeStrings));
    }

    void
    ImageBuilder::setInternalFormat(const std::string & theType) {		
        getNode()->getAttribute(IMAGE_INTERNAL_FORMAT_ATTRIB)->nodeValue(theType);
    }

    void
    ImageBuilder::setColorScale(asl::Vector4f theColorScale) {
        getNode()->getAttribute(IMAGE_COLOR_SCALE_ATTRIB)->nodeValueAssign(theColorScale);
    }

    void
    ImageBuilder::setColorBias(asl::Vector4f theColorBias) {
        getNode()->getAttribute(IMAGE_COLOR_BIAS_ATTRIB)->nodeValueAssign(theColorBias);
    }

    void
    ImageBuilder::setDepth(unsigned int theDepth) {
        getNode()->getAttribute(DEPTH_ATTRIB)->nodeValueAssign(theDepth);
    }
    void
    ImageBuilder::setTiling(asl::Vector2i theTiling) {
        getNode()->getAttribute(IMAGE_TILE_ATTRIB)->nodeValueAssign(theTiling);
    }
}
