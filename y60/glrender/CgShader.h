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

#ifndef AC_Y60_RENDERER_CGSHADER_INCLUDED
#define AC_Y60_RENDERER_CGSHADER_INCLUDED

#include "y60_glrender_settings.h"

#include "GLShader.h"
#include "CgProgramInfo.h"
#include "ShaderLibrary.h"

#include <string>

namespace y60 {
    class Y60_GLRENDER_DECL CgShader : public GLShader {
        public:
            CgShader(const dom::NodePtr theNode,
                     const std::string & theVertexProfileName,
                     const std::string & theFragmentProfileName);
            virtual ~CgShader();
            virtual void compile(IShaderLibrary & theShaderLibrary);
            virtual void load(IShaderLibrary & theShaderLibrary);
            virtual void unload();
            virtual bool isLoaded() const;
            bool hasShader(const ShaderType &theShadertype) const;
            const ShaderDescription & getShader(const ShaderType & theShadertype) const;

            virtual const MaterialParameterVector & getVertexParameters() const;
            virtual const VertexRegisterFlags & getVertexRegisterFlags() const;

            virtual void activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial);
            virtual void deactivate(const MaterialBase & theMaterial);
            virtual void enableTextures(const MaterialBase & theMaterial);
            virtual void disableTextures(const MaterialBase & theMaterial);

            virtual unsigned getMaxTextureUnits() const;

            void bindBodyParams(const MaterialBase & theMaterial,
                    const Viewport & theViewport,
                    const LightVector & theLights,
                    const Body & theBody,
                    const Camera & theCamera);
            void bindOverlayParams(const MaterialBase & theMaterial);
            bool isSupported() const {
                return (((_myVertexShader._myType != NO_SHADER_TYPE)   ? (_myVertexShader._myProfile != NO_PROFILE)
                                                                       : true) &&
                        ((_myFragmentShader._myType != NO_SHADER_TYPE) ? (_myFragmentShader._myProfile != NO_PROFILE)
                                                                       : true));
            }

        protected:
            void loadShaderProperties(const dom::NodePtr theShaderNode,
                    ShaderDescription & theShader,
                    const std::string & theProfileName);

        private:
            void processCompilerArgs(std::vector<std::string> & theArgs, const std::string & theArgList);
            void bindMaterialParams(const MaterialBase & theMaterial);

            ShaderDescription     _myVertexShader;
            ShaderDescription     _myFragmentShader;
            CgProgramInfoPtr      _myVertexProgram;
            CgProgramInfoPtr      _myFragmentProgram;
    };
    typedef asl::Ptr<CgShader, dom::ThreadingModel> CgShaderPtr;
} // namespace y60

#endif // AC_Y60_RENDERER_GLSHADER_INCLUDED
