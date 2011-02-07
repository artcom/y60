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

#ifndef _ac_renderer_CgProgramInfo_h_
#define _ac_renderer_CgProgramInfo_h_

#include "y60_glrender_settings.h"

#include "ShaderDescription.h"

#include <y60/scene/Viewport.h>
#include <y60/scene/Camera.h>
#include <y60/scene/Body.h>
#include <y60/scene/Light.h>
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <string>
#include <vector>

namespace y60 {
    DEFINE_EXCEPTION(CgParameterException, asl::Exception);

   //=== Cg-GL specific parameter names  ============================================
    // these can be used with a _I, _T or _IT suffix to get the
    // inverted, transposed or interverted-transposed matrix
    const char * const CG_GL_MODELVIEW_PARAMETER           = "GL_MODELVIEW";
    const char * const CG_GL_PROJECTION_PARAMETER          = "GL_PROJECTION";
    const char * const CG_GL_TEXTURE_PARAMETER             = "GL_TEXTURE";
    const char * const CG_GL_MODELVIEW_PROJECTION_PARAMETER = "GL_MODELVIEW_PROJECTION";

    //=== AC CG automatic paramters  ============================================
    enum CgAutoParameterID {
        OBJECTWORLD,OBJECTWORLD_I,OBJECTWORLD_T,OBJECTWORLD_IT,
        CAMERA_POSITION, CAMERA_I, CAMERA_T, VIEWPROJECTION, TEXTURE_MATRICES,
        POSITIONAL_LIGHTS, DIRECTIONAL_LIGHTS, SPOT_LIGHTS,
        POSITIONAL_LIGHTS_DIFFUSE_COLOR, DIRECTIONAL_LIGHTS_DIFFUSE_COLOR, SPOT_LIGHTS_DIFFUSE_COLOR,
        POSITIONAL_LIGHTS_SPECULAR_COLOR, DIRECTIONAL_LIGHTS_SPECULAR_COLOR, SPOT_LIGHTS_SPECULAR_COLOR,
        AMBIENT_LIGHT_COLOR,
        SPOT_LIGHTS_CUTOFF,
        SPOT_LIGHTS_EXPONENT,
        SPOT_LIGHTS_DIRECTION,
        SPOT_LIGHTS_ATTENUATION,
        MAX_AUTO_PARAMETER
    };
    const char * const CgAutoParameterString[] = {
        "AC_OBJECTWORLD",
        "AC_OBJECTWORLD_I",
        "AC_OBJECTWORLD_T",
        "AC_OBJECTWORLD_IT",
        "AC_CAMERA_POSITION",
        "AC_CAMERA_I",
        "AC_CAMERA_T",
        "AC_VIEWPROJECTION",
        "AC_TEXTURE_MATRICES",
        // light position
        "AC_POSITIONAL_LIGHTS",
        "AC_DIRECTIONAL_LIGHTS",
        "AC_SPOT_LIGHTS",
        // light diffuse color
        "AC_POSITIONAL_LIGHTS_DIFFUSE_COLOR",
        "AC_DIRECTIONAL_LIGHTS_DIFFUSE_COLOR",
        "AC_SPOT_LIGHTS_DIFFUSE_COLOR",
        // light specular color
        "AC_POSITIONAL_LIGHTS_SPECULAR_COLOR",
        "AC_DIRECTIONAL_LIGHTS_SPECULAR_COLOR",
        "AC_SPOT_LIGHTS_SPECULAR_COLOR",
        // ambient color
        "AC_AMBIENT_LIGHT_COLOR",
        // spot parameters
        "AC_SPOT_LIGHTS_CUTOFF",
        "AC_SPOT_LIGHTS_EXPONENT",
        "AC_SPOT_LIGHTS_DIRECTION",
        "AC_SPOT_LIGHTS_ATTENUATION",
        0
    };

    class Renderer;

    void assertCg(const std::string & theWhere, const CGcontext theCgContext);

    class Y60_GLRENDER_DECL CgProgramInfo {
        public:
            CgProgramInfo(const ShaderDescription & theShader,
                    const CGcontext theCgContext);
            virtual ~CgProgramInfo();
            void load();
            CGprogram getCgProgramID() { return _myCgProgramID; }

            void enableProfile();
            void disableProfile();
            void enableTextures(const MaterialBase & theMaterial);
            void disableTextures();
            CGprofile getCgProfile() const;

            void bindMaterialParams(const MaterialBase & theMaterial);
            void bindBodyParams(const MaterialBase & theMaterial,
                        const y60::LightVector & theLightInstances,
                        const Viewport & theViewport,
                        const y60::Body & theBody,
                        const y60::Camera & theCamera);
            void bindOverlayParams();

            void bind();
            bool reloadIfRequired(const y60::LightVector & theLightInstances,
                                  const MaterialBase & theMaterial);
        private:
            static void setCgMaterialParameter(CGparameter & theCgParameter,
                    const dom::Node & theNode, const std::string & thePropertyName,
                    const MaterialBase & theMaterial);

            void appendUnsizedArrayBugWorkaroundCompilerArgs(std::vector<std::string> & theArgStrings, std::vector<const char*> & theArgs) const;
            void createAndCompileProgram();

            void setCGGLParameters();

            struct CgProgramGlParam {
                CgProgramGlParam(const std::string & myParamName,
                                 CGparameter myParameter,
                                 CGGLenum myStateMatrixType,
                                 CGGLenum myTransform)
                    : _myParamName(myParamName),
                      _myParameter(myParameter),
                      _myStateMatrixType(myStateMatrixType),
                      _myTransform(myTransform)
                    {};
                std::string _myParamName;
                CGparameter _myParameter;
                CGGLenum _myStateMatrixType;
                CGGLenum _myTransform;
            };
            typedef std::vector<CgProgramGlParam> CgProgramGlParamVector;

            struct CgProgramAutoParam {
                CgProgramAutoParam() {}
                CgProgramAutoParam(const std::string & myName,
                                 CGparameter myParameter,
                                 CgAutoParameterID myID,
                                 CGtype myType)
                    : _myName(myName),
                      _myParameter(myParameter),
                      _myID(myID),
                      _myType(myType)
                    {};
                std::string         _myName;
                CGparameter         _myParameter;
                CgAutoParameterID   _myID;
                CGtype              _myType;
            };
            typedef std::map<int,CgProgramAutoParam> CgProgramAutoParams;


            struct CgProgramNamedParam {
                CgProgramNamedParam(const std::string & myParamName,
                                 CGparameter myParameter)
                    : _myParamName(myParamName),
                      _myParameter(myParameter)
                    {};
                std::string         _myParamName;
                CGparameter         _myParameter;
            };
            typedef std::vector<CgProgramNamedParam> CgProgramNamedParamVector;

            class CgTextureParam : public CgProgramNamedParam {
                public:
                    CgTextureParam(const std::string & theParamName, CGparameter theParameter) :
                        CgProgramNamedParam(theParamName, theParameter),
                        _myTextureUnit(),
                        _myInUseFlag()
                    {}

                    void
                    updateTextureUnit(std::string const& theShaderName) {
                        _myTextureUnit = cgGLGetTextureEnum(_myParameter);
                        CGerror myError = cgGetError();
                        if (myError == CG_NO_ERROR) {
                            _myInUseFlag = true;
                        } else if (myError == CG_INVALID_PARAMETER_ERROR) {
                            _myInUseFlag = false;
                            AC_WARNING << "Shaderlibrary declares texture '"
                                       << _myParamName << "' but shader '"
                                       << theShaderName << "' does not use it.";
                        } else {
                            throw CgParameterException(
                                    std::string("Cg error while getting texture unit of '") +
                                    _myParamName + "': " + cgGetErrorString(myError),
                                    PLUS_FILE_LINE);
                        }
                    }

                    bool
                    isUsedByShader() const {
                        return _myInUseFlag;
                    }
                    GLenum
                    getTextureUnit() const {
                        return _myTextureUnit;
                    }

                private:
                    GLenum _myTextureUnit;
                    bool   _myInUseFlag;
            };
            typedef std::vector<CgTextureParam> CgTextureParamVector;

            CgProgramInfo();
            CgProgramInfo(const CgProgramInfo &); // disable copy constructor
            CgProgramInfo & operator=(const CgProgramInfo &);     // disable assignment op

            void processParameters();
            void setCgVectorParameter(const CgProgramAutoParam & theParam, const asl::Vector3f & theValue);
            void setCgVectorParameter(const CgProgramAutoParam & theParam, const asl::Vector4f & theValue);
            void setCgMatrixParameter(const CgProgramAutoParam & theParam, const asl::Matrix4f & theValue);
            void setCgArrayVector3fParameter(const CgProgramAutoParam & theParam, const std::vector<asl::Vector3f> & theValue);
            void setCgArrayVector4fParameter(const CgProgramAutoParam & theParam, const std::vector<asl::Vector4f> & theValue);

            void setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam, const std::vector<asl::Vector3f> & theValue);
            void setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam, const std::vector<asl::Vector4f> & theValue);
            void setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam, const std::vector<float> & theValue);
            void updateTextureUnits();

            ShaderDescription           _myShader;
            CGprogram                   _myCgProgramID;
            CGcontext                   _myContext;
            std::string                 _myPathName;
            CgProgramGlParamVector      _myGlParams;       // GL-State to CG parameters
            CgProgramAutoParams         _myAutoParams;     // other automatic CG parameters (e.g. CameraPosition)
            CgTextureParamVector        _myTextureParams;  // Texture parameters
            CgProgramNamedParamVector   _myMiscParams;     // material parameter
            std::string                 _myCWD;            // CWD on CG Compilation
            std::map<int,int> _myUnsizedArrayAutoParamSizes;

            std::vector<const char *> _myCachedCompilerArgs;
            std::string _myCgProgramString;
            bool                        _myUsesLights;
    };

    typedef asl::Ptr<CgProgramInfo> CgProgramInfoPtr;
    typedef std::vector<CgProgramInfoPtr> CgProgramVector;
}

#endif
