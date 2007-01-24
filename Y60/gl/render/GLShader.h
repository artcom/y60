#ifndef AC_Y60_RENDERER_GLSHADER_INCLUDED
#define AC_Y60_RENDERER_GLSHADER_INCLUDED

#include "ShaderDescription.h"
#include "ShaderFeatureSet.h"
#include "ShaderTypes.h"

#include <y60/DataTypes.h>
#include <y60/IShader.h>
#include <y60/MaterialBase.h>
#include <y60/NodeValueNames.h>
#include <dom/Nodes.h>
#include <asl/Ptr.h>

#include <string>
#include <GL/gl.h>

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
                        const Camera & theCamera, const Texture & theTexture);

            std::string             _myId;
            std::string             _myName;
            float                   _myCost;
            dom::NodePtr            _myDefaultTextures;
            
            ShaderFeatureSet          _myFeatureSet;
            std::vector<dom::NodePtr> _myPropertyNodes;
    };

    typedef asl::Ptr<GLShader, dom::ThreadingModel> GLShaderPtr;
    typedef std::vector<GLShaderPtr> GLShaderVector;

} // namespace y60

#endif // AC_Y60_RENDERER_GLSHADER_INCLUDED
