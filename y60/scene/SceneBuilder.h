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

#ifndef _ac_SceneBuilder_h_
#define _ac_SceneBuilder_h_

#include "y60_scene_settings.h"

#include "BuilderBase.h"

#include <y60/base/typedefs.h>
#include <asl/math/Vector234.h>
#include <asl/base/Stream.h>

#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
    #include <strstream>
#else
    #include <sstream>
#endif


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
    class TextureBuilder;

    /**
     * @ingroup y60builder
     * Builds a scene. Wraps an initial scene. Use the appendXYZ methods
     * to add stuff.
     */
    class Y60_SCENE_DECL SceneBuilder : public BuilderBase {
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
            const std::string & appendTexture(TextureBuilder & theTexture);
            const std::string & createDefaultViewport();

            dom::NodePtr findMovieByFilename(const std::string & theMovieName);
            dom::NodePtr findImageByFilename(const std::string & theImageName);

            void binarize(asl::WriteableStream & theBlock) const;
            std::ostream & serialize(std::ostream & theOutStream) const;

            void setInitialCamera(const std::string & theCameraId);
            dom::NodePtr getMainViewportNode() const;
        private:
            dom::DocumentPtr _myDocument;

    };

    inline std::ostream & operator << (std::ostream & os, const SceneBuilder & f) {
        return f.serialize(os);
    }

    typedef asl::Ptr<SceneBuilder> SceneBuilderPtr;
}

#endif

