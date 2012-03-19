#include <string>
#include <map>
#include <assimp/assimp.h>
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>       // Output data structure
#include <assimp/aiPostProcess.h> // Post processing flags
#include <assimp/aiMaterial.h>

#include <y60/modelling/modelling_functions.h>
#include <y60/scene/Body.h>
#include <y60/jslib/JSScene.h>

namespace y60 {

    class AssimpLoader {
        
    public:

        AssimpLoader(y60::ScenePtr theScene);
        virtual ~AssimpLoader() {};

        void readFile(const std::string & theFilename, unsigned int thePostProcessingFlags);
        void createScene(dom::NodePtr theTransform); 
        void forceMaterial(dom::NodePtr theMaterial);
        void setBlacklist(const std::map<std::string, bool> & theBlacklist);

    private:
        void createRecursive(const struct aiScene * theScene, const struct aiNode * theNode, dom::NodePtr theParentNode);
        std::string createMaterial(aiMaterial * theMaterial, std::string & theName);
        dom::NodePtr createShape(std::string & thePrimitiveType, std::string & theMaterialId, 
                            std::string & theName, std::vector<asl::Vector3f> & thePositions, std::vector<asl::Vector3f> & theNormals, 
                            std::vector<std::vector<asl::Vector4f> > & theColors, std::vector<std::vector<asl::Vector2f> > & theTexCoords);
        void appendTextures(aiMaterial * theMaterial, y60::MaterialBuilder & theMaterialBuilder);
        void appendTexture(aiTextureType theType, unsigned int theIndex, 
                    aiMaterial * theMaterial, y60::MaterialBuilder & theMaterialBuilder);

    private:
        Assimp::Importer _importer;
        dom::NodePtr _forcedMaterial;
    
        dom::NodePtr _shapes;
        dom::NodePtr _materials;
        
        y60::ScenePtr _scene;
        std::map<std::string, bool> _blacklist;
    };

} // namespace y60 

