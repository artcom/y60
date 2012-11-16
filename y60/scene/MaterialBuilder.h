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
*/

#ifndef _ac_MaterialBuilder_h_
#define _ac_MaterialBuilder_h_

#include "y60_scene_settings.h"

#include "BuilderBase.h"
#include <y60/base/RankedFeature.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/typedefs.h>

#include <y60/base/typedefs.h>
#include <asl/base/Ptr.h>
#include <asl/math/Matrix4.h>

#include <vector>

namespace y60 {
    class SceneBuilder;

    class Y60_SCENE_DECL MaterialBuilder : public BuilderBase {
    public:
        DEFINE_EXCEPTION(MaterialBuilderException, asl::Exception);

        MaterialBuilder(const std::string & theName, bool theInlineTextureFlag = true);
        virtual ~MaterialBuilder();

        void needTextureFallback(bool needTextureFallback);
        void setType(const VectorOfRankedFeature & theType);

        unsigned getTextureCount();
        bool isBumpMap(unsigned theTextureIndex) const;
        bool isMovie(const std::string & theFileName) const;

        void addFeature(const std::string & theClass, const VectorOfRankedFeature & theValue);

        dom::NodePtr createTextureUnitNode(const std::string & theTextureId,
                const TextureApplyMode & theApplyMode,
                const TextureUsage & theUsage,
                const std::string & theMappingMode,
                const asl::Matrix4f & theMatrix,
                bool theSpriteFlag = false, float theRanking = 100.0f,
                bool  isFallback = false, float theFallbackRanking = 0.0f);

        dom::NodePtr createTextureNode(SceneBuilder & theSceneBuilder,
                const std::string & theName, const std::string & theImageId,
                const TextureWrapMode & theWrapMode,
                bool  theCreateMipmapsFlag,
                const asl::Matrix4f & theMatrix,
                const std::string & theInternalFormat = "",
                const asl::Vector4f & theColorScale = asl::Vector4f(1,1,1,1),
                const asl::Vector4f & theColorBias = asl::Vector4f(0,0,0,0),
                bool allowSharing = true);

        dom::NodePtr createMovieNode(SceneBuilder & theSceneBuilder,
                const std::string & theName, const std::string & theFileName,
                unsigned int theLoopCount = 1);

        dom::NodePtr createImageNode(SceneBuilder & theSceneBuilder,
                const std::string & theName, const std::string & theFileName,
                const TextureUsage & theUsage,
                ImageType theType = SINGLE,
                const std::string & theResizeMode = IMAGE_RESIZE_SCALE,
                unsigned theDepth = 1,
                bool allowSharing = true);

/*
        void appendTexture(SceneBuilder & theSceneBuilder,
                           const std::string & theName,
                           const std::string & theFileName,
                           const std::string & theApplyMode,
                           const std::string & theUsage,
                           const std::string & theWrapMode,
                           const asl::Matrix4f & theMatrix,
                           float theRanking,
                           bool  isFallback,
                           float theFallbackRanking,
                           bool  theCreateMipmapsFlag,
                           float theAlpha = - 1.0,
                           bool  theSpriteFlag = false,
                           unsigned theDepth = 1);*/

        void appendCubemap(SceneBuilder & theSceneBuilder,
                           const std::string & theName,
                           const TextureUsage & theUsage,
                           const std::string & theFrontFileName,
                           const std::string & theRightFileName,
                           const std::string & theBackFileName,
                           const std::string & theLeftFileName,
                           const std::string & theTopFileName,
                           const std::string & theBottomFileName,
                           const TextureApplyMode & theApplyMode);

        void setTextureUVRepeat(const std::string & theTextureName, double theRepeatU, double theRepeatV);

        const std::string & getName() const;
        void computeRequirements();

        void setTransparencyFlag(bool theFlag);
        bool getTransparencyFlag() const;

    private:
        void checkState();
        void setup();
    private:
        bool _myRequirementsAdded;
        bool _needTextureFallback;
        bool _myInlineTextureFlag;

        // Requirements
        VectorOfRankedFeature _myMappingRequirements;
        VectorOfRankedFeature _myTextureRequirements;
        VectorOfRankedFeature _myLightingRequirements;

        static unsigned _myNextId;
    };

    typedef asl::Ptr<MaterialBuilder>  MaterialBuilderPtr;

    Y60_SCENE_DECL void createLightingFeature(VectorOfRankedFeature & theRequirement,
                               LightingModel  theLightingModelType);
}

#endif
