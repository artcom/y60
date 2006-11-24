//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: IShader.h,v $
//   $Author: david $
//   $Revision: 1.8 $
//   $Date: 2005/04/04 14:40:15 $
//
//  Description:
//
//=============================================================================

#ifndef AC_Y60_SHADER_INTERFACE_INCLUDED
#define AC_Y60_SHADER_INTERFACE_INCLUDED

#include "MaterialParameter.h"
#include <y60/DataTypes.h>

#include <asl/Ptr.h>
#include <string>
#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(ShaderException, asl::Exception);
    class MaterialBase;
    typedef asl::Ptr<MaterialBase, dom::ThreadingModel>       MaterialBasePtr;
    class IShaderLibrary;
    class Viewport;
    class Body;
    class Camera;
    class Light;
    typedef std::vector<asl::Ptr<Light, dom::ThreadingModel> > LightVector;
    class IShader {
        public:
            virtual const std::string & getName() const = 0;
            virtual unsigned getPropertyNodeCount() const =0;
            virtual const dom::NodePtr getPropertyNode(unsigned theIndex)=0;
            virtual const dom::NodePtr getDefaultTextures()=0;
            virtual const VectorOfString * getFeatures(const std::string & theFeatureClass) const = 0;
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
            virtual void activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {};
            virtual void enableTextures(const MaterialBase & theMaterial) {};
            virtual void disableTextures(const MaterialBase & theMaterial) {};
            virtual void deactivate(const y60::MaterialBase & theMaterial) {};
            virtual bool isCGShader() { return false; }

            // called on body change
            virtual void bindBodyParams(const y60::MaterialBase & theMaterial,
                                        const Viewport & theViewport,
                                        const LightVector & theLights,
                                        const Body & theBody,
                                        const Camera & theCamera) = 0;
            virtual void bindOverlayParams(const MaterialBase & theMaterial) = 0;
    };
    typedef asl::Ptr<IShader, dom::ThreadingModel> IShaderPtr;

    class IShaderLibrary {
    public:
        virtual IShaderPtr findShader(MaterialBasePtr theMaterial) = 0;
    };
    typedef asl::Ptr<IShaderLibrary>   IShaderLibraryPtr;

} // namespace

#endif// AC_Y60_SHADER_INTERFACE_INCLUDED

