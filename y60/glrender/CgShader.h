//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef AC_Y60_RENDERER_CGSHADER_INCLUDED
#define AC_Y60_RENDERER_CGSHADER_INCLUDED

#include "y60_glrender_settings.h"

#include "GLShader.h"
#include "CgProgramInfo.h"
#include "ShaderLibrary.h"

#include <string>

namespace y60 {
    class Y60_GLRENDER_EXPORT CgShader : public GLShader {
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
                return _myVertexShader._myProfile && _myFragmentShader._myProfile;
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
