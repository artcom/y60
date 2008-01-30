//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_ImageBuilder_h_
#define _ac_ImageBuilder_h_

#include "BuilderBase.h"
#include <y60/Image.h>
#include <asl/Ptr.h>

namespace y60 {

    class ImageBuilder : public BuilderBase {
        public:
            DEFINE_EXCEPTION(ImageBuilderException, asl::Exception);

            ImageBuilder(const std::string & theName);
            virtual ~ImageBuilder();

		    void inlineImage(const std::string & theFileName, ImageFilter theFilter, const std::string & theResizeMode);
            void createFileReference(const std::string & myFileName, const std::string & theResizeMode);

            void setTiling(asl::Vector2i theTiling);
            void setDepth(unsigned int theDepth);

            const std::string & getName() const;
			void setInternalFormat(const std::string & theType);

		protected:
            ImageBuilder(const std::string & theNodeName, const std::string & theName);

        private:
			void init(const std::string & theName);
            void setImage(const std::string & theFileName, const std::string & theFilter, 
                const std::string & theResizeMode);
    };

    typedef asl::Ptr<ImageBuilder, dom::ThreadingModel>  ImageBuilderPtr;
}

#endif
