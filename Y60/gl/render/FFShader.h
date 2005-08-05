//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FFShader.h,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: 
//
//=============================================================================

#ifndef AC_Y60_RENDERER_FFSHADER_INCLUDED
#define AC_Y60_RENDERER_FFSHADER_INCLUDED

#include "GLShader.h"

namespace y60 {

    class FFShader : public GLShader {
        public:
            FFShader(const dom::NodePtr theNode);
            virtual ~FFShader();

            virtual const y60::MaterialParameterVectorPtr getVertexParameter() const;
            virtual const y60::VertexRegisterFlags & getVertexRegisterFlags() const;

            virtual void activate(y60::MaterialBase & theMaterial);
            virtual void deactivate(const y60::MaterialBase & theMaterial);
            virtual void enableTextures(const y60::MaterialBase & theMaterial);
            virtual void disableTextures(const y60::MaterialBase & theMaterial);
        private:
            ShaderDescription _myFixedFunctionShader;
    };
    typedef asl::Ptr<FFShader> FFShaderPtr;

} // namespace y60

#endif // AC_Y60_RENDERER_GLSHADER_INCLUDED
