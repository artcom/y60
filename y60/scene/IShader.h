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

#ifndef AC_Y60_SHADER_INTERFACE_INCLUDED
#define AC_Y60_SHADER_INTERFACE_INCLUDED

#include "y60_scene_settings.h"

#include "MaterialParameter.h"
#include <y60/base/DataTypes.h>
#include <y60/base/NodeValueNames.h>

#include <asl/base/Ptr.h>
#include <string>
#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(ShaderException, asl::Exception);

    class MaterialBase;
    typedef asl::Ptr<MaterialBase, dom::ThreadingModel> MaterialBasePtr;

    class IShaderLibrary;
    class Viewport;
    class Body;
    class Camera;
    class Light;
    typedef std::vector<asl::Ptr<Light, dom::ThreadingModel> > LightVector;

    class Y60_SCENE_DECL IShader {
        public:
            virtual const std::string & getName() const = 0;
            virtual unsigned getPropertyNodeCount() const =0;
            virtual const dom::NodePtr getPropertyNode(unsigned theIndex)=0;
            virtual const dom::NodePtr getDefaultTextures()=0;
            virtual const VectorOfString * getFeatures(const std::string & theFeatureClass) const = 0;
            virtual const VectorOfTextureUsage & getTextureFeature() const = 0;
            virtual bool hasFeature(const std::string & theFeatureClass) const = 0;
            virtual const MaterialParameterVector & getVertexParameters() const = 0;
            virtual const VertexRegisterFlags & getVertexRegisterFlags() const = 0;

            /**
             * Return maximum number of texture units available in the current GL context.
             * The value is dependant on the type of shader. Fixed function shader typically
             * can use 4 texture unites. Cg fragment shader can use 16.
             * @return Maximum number of texture units. Returns 0 on error.
             */
            virtual unsigned getMaxTextureUnits() const = 0;

            virtual void load(IShaderLibrary & theShaderLibrary) = 0;

            // will be called, when material is loaded
            virtual void setup(MaterialBase & theMaterial) {}

            // will be called once per material change
            virtual void activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {}
            virtual void enableTextures(const MaterialBase & theMaterial) {}
            virtual void disableTextures(const MaterialBase & theMaterial) {}
            virtual void deactivate(const y60::MaterialBase & theMaterial) {}

            // called on body change
            virtual void bindBodyParams(const y60::MaterialBase & theMaterial,
                                        const Viewport & theViewport,
                                        const LightVector & theLights,
                                        const Body & theBody,
                                        const Camera & theCamera) = 0;
            virtual void bindOverlayParams(const MaterialBase & theMaterial) = 0;
            virtual void enableTextureProjection(const MaterialBase & theMaterial,
                                         const Viewport & theViewport,
                                         const Camera & theCamera) = 0;
            virtual ~IShader() {}
    };
    typedef asl::Ptr<IShader, dom::ThreadingModel> IShaderPtr;

    class Y60_SCENE_DECL IShaderLibrary {
    public:
        virtual IShaderPtr findShader(MaterialBasePtr theMaterial) = 0;

        virtual const std::string & getVertexProfileName() = 0;
        virtual const std::string & getFragmentProfileName() = 0;
        virtual ~IShaderLibrary() {}
    };
    typedef asl::Ptr<IShaderLibrary> IShaderLibraryPtr;

} // namespace

#endif// AC_Y60_SHADER_INTERFACE_INCLUDED
