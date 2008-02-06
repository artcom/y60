//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_TextureBuilder_h_
#define _ac_TextureBuilder_h_

#include "BuilderBase.h"
#include "Texture.h"

namespace y60 {

    class TextureBuilder : public BuilderBase {
        public:
            DEFINE_EXCEPTION(TextureBuilderException, asl::Exception);

            TextureBuilder(const std::string & theName, const std::string & theImageId);
            virtual ~TextureBuilder();

            void setType(TextureType theType);
            void setMipmapFlag(bool theFlag);
            void setWrapMode(const TextureWrapMode & theWrapMode);
            void setColorScale(const asl::Vector4f & theColorScale);
            void setColorBias(const asl::Vector4f & theColorBias);
            void setMatrix(const asl::Matrix4f & theMatrix);
            void setTextureFormat(const std::string & theInternalFormat);

		protected:

        private:
    };

    typedef asl::Ptr<TextureBuilder, dom::ThreadingModel>  TextureBuilderPtr;
}

#endif
