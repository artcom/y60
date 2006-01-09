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
//   $Id: ImageBuilder.h,v 1.2 2005/04/21 08:59:01 jens Exp $
//   $RCSfile: ImageBuilder.h,v $
//   $Author: jens $
//   $Revision: 1.2 $
//   $Date: 2005/04/21 08:59:01 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_ImageBuilder_h_
#define _ac_ImageBuilder_h_

#include "BuilderBase.h"
#include <y60/Image.h>
#include <asl/Ptr.h>

namespace y60 {

    class ImageBuilder : public BuilderBase {
        public:
            DEFINE_EXCEPTION(ImageBuilderException, asl::Exception);
            ImageBuilder(const std::string & theName, bool theCreateMipmapFlag);
            virtual ~ImageBuilder();
		    void inlineImage(const std::string & theFileName, ImageFilter theFilter, const std::string & theResizeMode);
            void createFileReference(const std::string & myFileName, const std::string & theResizeMode);

            void setType(ImageType theType);
            void setColorScale(asl::Vector4f theColorScale);
            void setColorBias(asl::Vector4f theColorBias);
            void setTiling(asl::Vector2i theTiling);
            void setDepth(unsigned int theDepth);
            const std::string & getName() const;
			void setInternalFormat(const std::string & theType);

		protected:
            ImageBuilder(const std::string & theNodeName, const std::string & theName, bool theCreateMipmapFlag);
        private:
			void init(const std::string & theName, bool theCreateMipmapFlag);
    };

    typedef asl::Ptr<ImageBuilder, dom::ThreadingModel>  ImageBuilderPtr;
}

#endif

