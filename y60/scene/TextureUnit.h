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

#ifndef Y60_TEXTUREUNIT_INCLUDED
#define Y60_TEXTUREUNIT_INCLUDED

#include "y60_scene_settings.h"

#include "TextureManager.h"
#include "IShader.h"
#include "Texture.h"

#include <y60/base/NodeValueNames.h>
#include <y60/image/ITextureManager.h>

#include <asl/base/Exception.h>
#include <asl/math/Matrix4.h>
#include <asl/base/Ptr.h>

#include <asl/dom/AttributePlug.h>
#include <asl/dom/Facade.h>

#include <string>


namespace y60 {
    //                  theTagName                 theType           theAttributeName              theDefault
    DEFINE_ATTRIBUTE_TAG(TextureUnitTextureIdTag,   std::string,      TEXTUREUNIT_TEXTURE_ATTRIB,   "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureUnitApplyModeTag,   TextureApplyMode, TEXTUREUNIT_APPLYMODE_ATTRIB, MODULATE, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureUnitCombinerTag,    std::string,      TEXTUREUNIT_COMBINER_ATTRIB,  "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureUnitEnvColorTag,    asl::Vector4f,    TEXTUREUNIT_ENVCOLOR_ATTRIB,  asl::Vector4f(1,1,1,1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureUnitSpriteTag,      bool,             TEXTUREUNIT_SPRITE_ATTRIB,    false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureUnitProjectorIdTag, std::string,      TEXTUREUNIT_PROJECTOR_ATTRIB, "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureUnitMatrixTag,      asl::Matrix4f,    MATRIX_ATTRIB,                asl::Matrix4f::Identity(), Y60_SCENE_DECL);
    DEFINE_EXCEPTION(TextureUnitException, asl::Exception);

    class Y60_SCENE_DECL TextureUnit :
        public dom::Facade,
        public TextureUnitTextureIdTag::Plug,
        public TextureUnitProjectorIdTag::Plug,
        public TextureUnitApplyModeTag::Plug,
        public TextureUnitCombinerTag::Plug,
        public TextureUnitEnvColorTag::Plug,
        public TextureUnitSpriteTag::Plug,
        public TextureUnitMatrixTag::Plug,
        public ResizePolicyTag::Plug
    {
        public:
            TextureUnit(dom::Node & theNode);
            IMPLEMENT_FACADE(TextureUnit);
            virtual ~TextureUnit();

            void setTextureManager(const TextureManagerPtr theTextureManager);

            TexturePtr getTexture() const;
            ICombinerPtr getCombiner() const;

            virtual void registerDependenciesRegistrators();

        protected:

        private:
            TextureUnit();

            void updateCombiner();

            TextureManagerPtr _myTextureManager;
    };

    typedef asl::Ptr<TextureUnit, dom::ThreadingModel> TextureUnitPtr;
}

#endif // Y60_TEXTUREUNIT_INCLUDED
