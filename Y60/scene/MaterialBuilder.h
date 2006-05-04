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
//         $Id: MaterialBuilder.h,v 1.2 2005/04/21 08:59:01 jens Exp $
//    $RCSfile: MaterialBuilder.h,v $
//     $Author: jens $
//   $Revision: 1.2 $
//       $Date: 2005/04/21 08:59:01 $
//
//=============================================================================

#ifndef _ac_MaterialBuilder_h_
#define _ac_MaterialBuilder_h_

#include "BuilderBase.h"
#include <y60/RankedFeature.h>
#include <y60/NodeValueNames.h>
#include <y60/typedefs.h>

#include <dom/typedefs.h>
#include <asl/Ptr.h>
#include <asl/Matrix4.h>

#include <vector>

namespace y60 {
    class SceneBuilder;

    class MaterialBuilder : public BuilderBase {
    public:
        DEFINE_EXCEPTION(MaterialBuilderException, asl::Exception);
        MaterialBuilder(const std::string & theName, bool theInlineTextureFlag = true);
        virtual ~MaterialBuilder();

        void needTextureFallback(bool needTextureFallback);
        void setType(const VectorOfRankedFeature & theType);
        unsigned getTextureCount();
        bool isBumpMap(int theTextureIndex) const;

        void addFeature(const std::string & theClass, const VectorOfRankedFeature & theValue);

        dom::NodePtr createTextureNode(const std::string & theImageId,
                                       const std::string & theApplyMode,
                                       const std::string & theUsage,
                                       const std::string & theWrapMode,
                                       const std::string & theMappingMode,
                                       const asl::Matrix4f & theTextureMatrix,
                                       float theRanking,
                                       bool  isFallback,
                                       float theFallbackRanking,
                                       bool  theSpriteFlag = false);

        const std::string & createMovie(SceneBuilder & theSceneBuilder,
                                        const std::string & theName,
                                        const std::string & theFileName,
                                        unsigned theLoopCount,
                                        const asl::Vector4f theColorScale,
                                        const asl::Vector4f theColorBias,
										const std::string & theInternalFormat);

        const std::string & createImage(SceneBuilder & theSceneBuilder,
                                        const std::string & theName,
                                        const std::string & theFileName,
                                        const std::string & theUsage,
                                        bool  theCreateMipmapsFlag,
                                        const asl::Vector4f theColorScale,
                                        const asl::Vector4f theColorBias,
                                        ImageType theType,
										const std::string & theInternalFormat,
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
                           const asl::Matrix4f & theTextureMatrix,
                           float theRanking,
                           bool  isFallback,
                           float theFallbackRanking,
                           bool  theCreateMipmapsFlag,
                           float theAlpha = - 1.0,
                           bool  theSpriteFlag = false,
                           unsigned theDepth = 1);*/

        void appendCubemap(SceneBuilder & theSceneBuilder,
                           const std::string & theName,
                           const std::string & theFrontFileName,
                           const std::string & theRightFileName,
                           const std::string & theBackFileName,
                           const std::string & theLeftFileName,
                           const std::string & theTopFileName,
                           const std::string & theBottomFileName,
                           const std::string & theApplyMode,
                           const asl::Vector4f theColorScale);

        void setTextureUVRepeat(const std::string & theTextureName, double theRepeatU, double theRepeatV);

        const std::string & getName() const;
        void computeRequirements();

        void setTransparencyFlag(bool theFlag);
        bool getTransparencyFlag() const;

        bool isMovie(const std::string & theFileName);
    private:
        void checkState();
        void setup();
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

    void createLightingFeature(VectorOfRankedFeature & theRequirement,
                               LightingModel  theLightingModelType);
}

#endif
