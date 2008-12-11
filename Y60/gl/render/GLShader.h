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

#ifndef AC_Y60_RENDERER_GLSHADER_INCLUDED
#define AC_Y60_RENDERER_GLSHADER_INCLUDED

#include "ShaderDescription.h"
#include "ShaderFeatureSet.h"
#include "ShaderTypes.h"

#include <y60/base/DataTypes.h>
#include <y60/scene/IShader.h>
#include <y60/scene/MaterialBase.h>
#include <y60/base/NodeValueNames.h>
#include <asl/dom/Nodes.h>
#include <asl/base/Ptr.h>

#include <string>

#include <GL/glew.h>

namespace y60 {

    class ShaderLibrary;

    class GLShader : public y60::IShader {
        public:
            enum ShaderMatch {
                FULL_MATCH, WILDCARD_MATCH, NO_MATCH
            };
 
            DEFINE_EXCEPTION(GLShaderException, asl::Exception);  
            GLShader(const dom::NodePtr theNode);
            virtual ~GLShader();

            const std::string & getId() const;
            virtual void load(y60::IShaderLibrary & theShaderLibrary);
            virtual void unload();
            virtual bool isLoaded() const { return false; };
            virtual bool isSupported() const {
                return true;
            }
            const ShaderFeatureSet & getFeatureSet() const;

            const MaterialType & getType() const;
            float getCosts() const;
            ShaderMatch matches(const std::string & theFeatureClass, 
                         const VectorOfString & theRequirementList) const; 

            // implementation of IShader:
            virtual const std::string & getName() const;
            virtual unsigned getPropertyNodeCount() const;
            virtual const dom::NodePtr getPropertyNode(unsigned theIndex);
            virtual const dom::NodePtr getDefaultTextures();
            virtual const VectorOfString * getFeatures(const std::string & theFeatureClass) const;
            const VectorOfTextureUsage & getTextureFeature() const;
            virtual bool hasFeature(const std::string & theFeatureClass) const;

            virtual void activateGroup1(MaterialBase & theMaterial, const Viewport & theViewport);
            
            virtual void activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial);
            virtual void deactivate(const MaterialBase & theMaterial);
            
            virtual void enableTextures(const MaterialBase & theMaterial);
            virtual void disableTextures(const MaterialBase & theMaterial);
            virtual unsigned getMaxTextureUnits() const;

            // called on body change
            virtual void bindBodyParams(const MaterialBase & theMaterial,
                    const Viewport & theViewport,
                    const LightVector & theLights,
                    const Body & theBody, 
                    const Camera & theCamera);
            virtual void bindOverlayParams(const MaterialBase & theMaterial) {};            

            void enableTextureProjection(const MaterialBase & theMaterial,
                                         const Viewport & theViewport,
                                         const Camera & theCamera);

       protected:
            virtual void loadShaderProperties(const dom::NodePtr theShaderNode, 
                                      ShaderDescription & theShader);
            void loadParameters(const dom::NodePtr theParameterListNode, 
                                      ShaderDescription & theShader); 
            MaterialType            _myType;

       private:
            GLShader();
            void loadFeatureSet(const dom::NodePtr theShaderNode);
            void setupProjectorMatrix( asl::Matrix4f & theMatrix, const MaterialBase & theMaterial,
                        const Camera & theCamera, const TextureUnit & theTextureUnit);

            std::string             _myId;
            std::string             _myName;
            float                   _myCost;
            dom::NodePtr            _myDefaultTextures;
            
            ShaderFeatureSet          _myFeatureSet;
            std::vector<dom::NodePtr> _myPropertyNodes;
    
            bool _myHasPointParmatersEXT;
            bool _myHasBlendMinMaxEXT;
    };

    typedef asl::Ptr<GLShader, dom::ThreadingModel> GLShaderPtr;
    typedef std::vector<GLShaderPtr> GLShaderVector;

} // namespace y60

#endif // AC_Y60_RENDERER_GLSHADER_INCLUDED
