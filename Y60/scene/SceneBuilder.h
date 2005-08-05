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
//   $Id: SceneBuilder.h,v 1.2 2005/04/21 08:59:01 jens Exp $
//   $RCSfile: SceneBuilder.h,v $
//   $Author: jens $
//   $Revision: 1.2 $
//   $Date: 2005/04/21 08:59:01 $
//
//=============================================================================

#ifndef _ac_SceneBuilder_h_
#define _ac_SceneBuilder_h_

#include "BuilderBase.h"

#include <dom/typedefs.h>
#include <asl/Vector234.h>
#include <asl/Stream.h>

#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
    #include <strstream>
#else
    #include <sstream>
#endif

namespace asl {
    //class WriteableStream;
}

namespace y60 {

    DEFINE_EXCEPTION(Exception, asl::Exception);

    class ShapeBuilder;
    class MaterialBuilder;
    class TransformBuilder;
    class WorldBuilder;
    class LightSourceBuilder;
    class AnimationBuilder;
    class CharacterBuilder;
    class ImageBuilder;
    class MovieBuilder;
    class CanvasBuilder;

    /**
     * @ingroup y60builder
     * Builds a scene. Wraps an initial scene. Use the appendXYZ methods
     * to add stuff.
     */
    class SceneBuilder : public BuilderBase {
        public:
	    SceneBuilder(dom::DocumentPtr theDocument);

            virtual ~SceneBuilder();
            const std::string & appendShape(ShapeBuilder & theShape);
            // call appendMaterial only when materialbuilder is complete
            const std::string & appendMaterial(MaterialBuilder & theMaterial);
            const std::string & appendLightSource(LightSourceBuilder & theLightSource);
            const std::string & appendWorld(WorldBuilder & theWorld);
            const std::string & appendAnimation(AnimationBuilder & theAnimation);
            const std::string & appendCharacter(CharacterBuilder & theCharacter);
            const std::string & appendImage(ImageBuilder & theImage);
            const std::string & appendMovie(MovieBuilder & theMovie);
            const std::string & appendCanvas(CanvasBuilder & theCanvas);
            const std::string & createDefaultViewport();

            dom::NodePtr findMovieByFilename(const std::string & theMovieName);

            dom::NodePtr findImageByFilename(const std::string & theImageName);
            dom::NodePtr findImageByFilename_ColorScale_ColorBias(const std::string & theImageName,
                                                                  const asl::Vector4f & theColorScale,
                                                                  const asl::Vector4f & theColorBias);

            void binarize(asl::WriteableStream & theBlock) const;
            std::ostream & serialize(std::ostream & theOutStream) const;

            void SceneBuilder::setInitialCamera(const std::string & theCameraId);
            dom::NodePtr SceneBuilder::getMainViewportNode() const;
        private:
            dom::DocumentPtr _myDocument;

    };

    inline std::ostream & operator << (std::ostream & os, const SceneBuilder & f) {
        return f.serialize(os);
    }

    typedef asl::Ptr<SceneBuilder> SceneBuilderPtr;
}

#endif

