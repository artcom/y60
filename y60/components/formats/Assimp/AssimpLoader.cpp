#include "AssimpLoader.h"
#include <y60/base/property_functions.h>
#include <y60/modelling/modelling_functions.h>
#include <y60/scene/ShapeBuilder.h>

using namespace y60;

// Todo:
// - lights
// - cameras
// - materials 
//     - handle transparency correctly
// - texture loading
// - keyframe animations
// - material caching

AssimpLoader::AssimpLoader(y60::ScenePtr theScene) :
    _importer(),
    _forcedMaterial(0),
    _shapes(theScene->getShapesRoot()),
    _materials(theScene->getMaterialsRoot()),
    _scene(theScene),
    _blacklist(std::map<std::string, bool>())
{
}

void 
AssimpLoader::setBlacklist(const std::map<std::string, bool> & theBlacklist) {
    _blacklist = theBlacklist;
}

void 
AssimpLoader::readFile(const std::string & theFilename, unsigned int thePostProcessingFlags)
{
    _importer.ReadFile( theFilename, thePostProcessingFlags );
    
    const aiScene * scene = _importer.GetScene();
    if (!scene) {
        AC_ERROR << "could not load the scene: " << _importer.GetErrorString();    
        // EXCEPTION
        return;        
    }
}

void 
AssimpLoader::forceMaterial(dom::NodePtr theMaterial) {
    _forcedMaterial = theMaterial;
}

void 
AssimpLoader::createScene(dom::NodePtr theTransform) { 
    
    createRecursive(_importer.GetScene(), _importer.GetScene()->mRootNode, theTransform);
}

std::string 
AssimpLoader::createMaterial(aiMaterial * theMaterial, std::string & theName) {
    MaterialBuilder myMaterialBuilder(theName, false);
    std::string myId = _scene->getSceneBuilder()->appendMaterial(myMaterialBuilder);

    // myMaterialBuilder.setTransparencyFlag(true);
    VectorOfRankedFeature myLightingFeature;
    createLightingFeature(myLightingFeature, LAMBERT);
    myMaterialBuilder.setType(myLightingFeature);

    struct aiColor4D diffuse;
    struct aiColor4D specular;
    struct aiColor4D ambient;
    struct aiColor4D emission;
    float shininess, strength;

    if (AI_SUCCESS == aiGetMaterialColor(theMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f", DIFFUSE_PROPERTY, 
                asl::Vector4f(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
    }

    if (AI_SUCCESS == aiGetMaterialColor(theMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)) {
        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f", SPECULAR_PROPERTY, 
                asl::Vector4f(specular.r, specular.g, specular.b, specular.a));
    }

    if (AI_SUCCESS == aiGetMaterialColor(theMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f", AMBIENT_PROPERTY, 
                asl::Vector4f(ambient.r, ambient.g, ambient.b, ambient.a));
    }

    if(AI_SUCCESS == aiGetMaterialColor(theMaterial, AI_MATKEY_COLOR_EMISSIVE, &emission)) {
        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f", EMISSIVE_PROPERTY, 
                asl::Vector4f(emission.r, emission.g, emission.b, emission.a));
    }

    if ( AI_SUCCESS != aiGetMaterialFloat(theMaterial, AI_MATKEY_SHININESS, &shininess) ) {
        shininess = 15.0f;
    }
    
    if ( AI_SUCCESS != aiGetMaterialFloat(theMaterial, AI_MATKEY_SHININESS_STRENGTH, &strength) ) {
        strength = 1.0f;
    }
    
    setPropertyValue<float>(myMaterialBuilder.getNode(), "float", y60::SHININESS_PROPERTY, shininess * strength);

    setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f", y60::SURFACE_COLOR_PROPERTY, 
                asl::Vector4f(1.0, 1.0, 0.0, 1.0));

    appendTextures(theMaterial, myMaterialBuilder);

    myMaterialBuilder.computeRequirements();
    // std::string myId = myMaterialBuilder.getNode()->getFacade<MaterialBase>()->get<IdTag>();
    return myId;
}

void 
AssimpLoader::appendTexture(aiTextureType theType, unsigned int theIndex, 
                    aiMaterial * theMaterial, y60::MaterialBuilder & theMaterialBuilder) 
{
    aiReturn success;
    aiString path;
    aiTextureMapping mapping;
    unsigned int uvindex;
    float blend;
    aiTextureOp op;
    aiTextureMapMode mapmode;

    success = theMaterial->GetTexture(theType, theIndex, &path, &mapping, &uvindex, 
        &blend, &op, &mapmode);
 
    if (success != AI_SUCCESS) {
        // TODO: Exception
    }
    
    std::string strPath(path.data, path.length); 
    AC_PRINT << "trying to load texture from file: " << strPath << " uvindex: " << uvindex;
    
    // Texture code
    std::string myTexName        = theMaterialBuilder.getName() + "_tex";
    TextureApplyMode myApplyMode = MODULATE;
    std::string myUVMappingMode  = TEXCOORD_UV_MAP;
    TextureUsage myUsage         = PAINT; // TODO: change according to type
    TextureWrapMode myWrapMode   = CLAMP_TO_EDGE;
   
    
    AC_PRINT << "settings default";

    bool myCreateMipmapFlag      = true;
    float myRanking              = 100.0f;
    bool myIsFallback            = true;
    float myFallbackRanking      = 100.0f;
    asl::Vector4f myColorScale   = asl::Vector4f(1.0f,1.0f,1.0f,1.0f);
    asl::Vector4f myColorBias    = asl::Vector4f(0.0f,0.0f,0.0f,0.0f);
    bool  mySpriteFlag           = false;
    
    AC_PRINT << "creating node";

    dom::NodePtr myNode;
    if (theMaterialBuilder.isMovie(strPath)) {
        myNode = theMaterialBuilder.createMovieNode(*(_scene->getSceneBuilder()), myTexName,
                strPath, 0);
    } else {
        myNode = theMaterialBuilder.createImageNode(*(_scene->getSceneBuilder()), myTexName,
                strPath, myUsage,
                SINGLE, IMAGE_RESIZE_PAD, 1, false);
    }
    std::string myImageId = myNode->getAttributeString(ID_ATTRIB);
    
    AC_PRINT << "creating texture unit node";

    myNode = theMaterialBuilder.createTextureNode(*(_scene->getSceneBuilder()),
            myTexName, myImageId,
            myWrapMode, myCreateMipmapFlag,
            asl::Matrix4f::Identity(), // TODO: set texture matrix 
            "", myColorScale, myColorBias);
    std::string myTextureId = myNode->getAttributeString(ID_ATTRIB);
    
    AC_PRINT << "creating texture node";

    theMaterialBuilder.createTextureUnitNode(myTextureId,
            myApplyMode, myUsage, myUVMappingMode, asl::Matrix4f::Identity(), mySpriteFlag,
            myRanking, myIsFallback, myFallbackRanking);
}

void
AssimpLoader::appendTextures(aiMaterial * theMaterial, y60::MaterialBuilder & theMaterialBuilder) {
    // TODO: implement support for the various types
    aiTextureType  myTypes[] = { aiTextureType_NONE, aiTextureType_DIFFUSE, aiTextureType_SPECULAR, 	
                                 aiTextureType_AMBIENT, aiTextureType_EMISSIVE, aiTextureType_HEIGHT, 	 
                                 aiTextureType_NORMALS, aiTextureType_SHININESS, aiTextureType_OPACITY, 	 
                                 aiTextureType_DISPLACEMENT, aiTextureType_LIGHTMAP, aiTextureType_REFLECTION,
                                 aiTextureType_UNKNOWN }; 	 
    
    std::vector<aiTextureType> myTextureTypes(myTypes, myTypes+sizeof(myTypes) / sizeof(aiTextureType));

    for (unsigned int i=0; i<myTextureTypes.size(); i++) {
        
        unsigned int myNumberOfTextures = theMaterial->GetTextureCount(myTextureTypes[i]);    
        for (unsigned int t=0; t<myNumberOfTextures; t++) {
            appendTexture(myTextureTypes[i], t, theMaterial, theMaterialBuilder);
        }
    }
}

void
AssimpLoader::createRecursive(const struct aiScene * theScene, const struct aiNode * theNode, dom::NodePtr theParentNode) {
    unsigned int n = 0, t;
    struct aiMatrix4x4 localMatrix = theNode->mTransformation;

    std::string myName(theNode->mName.data, theNode->mName.length);
    
    if (_blacklist.find( myName ) != _blacklist.end()) {
        // ignore blacklisted nodes
        return;
    }

    dom::NodePtr myCurrentNode = createTransform(theParentNode, myName); 

    aiVector3D   scaling;
    aiQuaternion rotation;
    aiVector3D   position;

    localMatrix.Decompose(scaling, rotation, position);

    // Transform the node
    myCurrentNode->getFacade<TransformHierarchyFacade>()->set<ScaleTag>(asl::Vector3f(scaling.x, scaling.y, scaling.z));
    myCurrentNode->getFacade<TransformHierarchyFacade>()->set<OrientationTag>(asl::Quaternionf(rotation.x, rotation.y, rotation.z, -rotation.w));
    myCurrentNode->getFacade<TransformHierarchyFacade>()->set<PositionTag>(asl::Vector3f(position.x, position.y, position.z));
    

	// draw all meshes assigned to this node
	for (; n < theNode->mNumMeshes; ++n) {
		const struct aiMesh* mesh = theScene->mMeshes[theNode->mMeshes[n]];
        std::string myName = std::string(theNode->mName.data, theNode->mName.length); //  std::string(mesh->mName.data, mesh->mName.length); 

        std::string myMaterialId;
        if (!_forcedMaterial) {
		    std::string myMaterialName = myName + std::string("_material");
            myMaterialId = createMaterial(theScene->mMaterials[mesh->mMaterialIndex], myMaterialName );
        } else {
            myMaterialId = _forcedMaterial->getFacade<MaterialBase>()->get<IdTag>();
        }

        std::vector<asl::Vector3f> positions;
        std::vector<asl::Vector3f> normals;
        std::vector<std::vector<asl::Vector4f> > colors( mesh->GetNumColorChannels(), std::vector<asl::Vector4f>() );
        std::vector<std::vector<asl::Vector2f> > texCoords( mesh->GetNumUVChannels(), std::vector<asl::Vector2f>() );

        int index = 0;
        std::vector<unsigned int> indices;
		for (t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
            
            // we only support triangulate meshes right now. you can check
			// face->mNumIndices for the number of vertices per face. [sh]
            
            if (face->mNumIndices != 3) {
                AC_ERROR << "unsupport number of face elements: " << face->mNumIndices;
                continue;
            }
            
			for(unsigned int i = 0; i < face->mNumIndices; i++) {
				int idx = face->mIndices[i];
				indices.push_back(index);

                // positions
                aiVector3D vec = mesh->mVertices[idx];
                positions.push_back(asl::Vector3f(vec.x, vec.y, vec.z)); 
        
                // colors
                if (mesh->GetNumColorChannels() > 0) {
                    for (unsigned int colorChannel=0; colorChannel < mesh->GetNumColorChannels(); colorChannel++) {
                        if (mesh->HasVertexColors(colorChannel)) {
                            aiColor4D color = mesh->mColors[colorChannel][idx];
                            colors[colorChannel].push_back(asl::Vector4f(color.r, color.g, color.b, 1.0f)); // color.a)); 
                        }
                    }
                }
                
                // texCoords 
				if (mesh->HasTextureCoords(0)) { 
                    for (unsigned int texcoordChannel=0; texcoordChannel < mesh->GetNumUVChannels(); texcoordChannel++) {
                        if (mesh->HasTextureCoords(texcoordChannel)) {
                            aiVector3D texCoord = mesh->mTextureCoords[texcoordChannel][idx];
                            texCoords[texcoordChannel].push_back(asl::Vector2f(texCoord.x, texCoord.y)); 
                        }
                    }
                }
               
                // normals
                if (mesh->HasNormals()) {
                    aiVector3D normal = mesh->mNormals[idx];
                    normals.push_back(asl::Vector3f(normal.x, normal.y, normal.z));
                }

                index++; 
            }
		}
        
        std::string primitiveType = PrimitiveTypeStrings[TRIANGLES];
        dom::NodePtr newShape = createShape(primitiveType, myMaterialId, myName, 
                                    positions, normals, colors, texCoords);

        dom::NodePtr myBody = createBody(myCurrentNode, newShape->getFacade<Shape>()->get<IdTag>());
        myBody->getFacade<Body>()->set<NameTag>(myName);
    }

	// draw all children
	for (n = 0; n < theNode->mNumChildren; ++n) {
        createRecursive(theScene, theNode->mChildren[n], myCurrentNode);
    }
}
        
dom::NodePtr 
AssimpLoader::createShape(std::string & thePrimitiveType, std::string & theMaterialId, std::string & theName, 
        std::vector<asl::Vector3f> & thePositions, std::vector<asl::Vector3f> & theNormals,
        std::vector<std::vector<asl::Vector4f> > & theColors, std::vector<std::vector<asl::Vector2f> > & theTexCoords) 
{

    ElementBuilder elementBuilder(thePrimitiveType, theMaterialId);
    ShapeBuilder shapeBuilder(theName + std::string("_shape", false));
    
    shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, thePositions.size());
    elementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());

    // create bins
    if ( !theTexCoords.empty()) {
        for (unsigned int tc=0; tc<theTexCoords.size(); tc++) {
            std::string uvset = std::string("uvset") + asl::as_string(tc);
            shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector2f>(uvset, theTexCoords[tc].size());
            elementBuilder.createIndex(uvset, getTextureRole(tc), theTexCoords[tc].size());
        }
    }

    if (!theColors.empty()) {
        shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector4f>(COLOR_ROLE, theColors[0].size());
        elementBuilder.createIndex(COLOR_ROLE, COLORS, theColors[0].size());
    }
    
    if (!theNormals.empty()) {
        shapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(NORMAL_ROLE, theNormals.size());
        elementBuilder.createIndex(NORMAL_ROLE, NORMALS, theNormals.size());
    }

    // fill bins with vertex information
    for(unsigned i = 0; i < thePositions.size(); ++i) {
        shapeBuilder.appendVertexData(POSITION_ROLE, thePositions[i]);
        elementBuilder.appendIndex(POSITIONS, i);

        if (!theNormals.empty()) {
            shapeBuilder.appendVertexData(NORMAL_ROLE, theNormals[i]);
            elementBuilder.appendIndex(NORMALS, i);
        }
        
        if ( !theTexCoords.empty()) {
            for (unsigned int tc=0; tc<theTexCoords.size(); tc++) {
                if (theTexCoords[tc].size() > i) {
                    std::string uvset = std::string("uvset") + asl::as_string(tc);
                    shapeBuilder.appendVertexData(uvset, theTexCoords[tc][i]);
                    elementBuilder.appendIndex(getTextureRole(tc), i);
                }
            }
        }
        
        if (!theColors.empty()) {
            shapeBuilder.appendVertexData(COLOR_ROLE, theColors[0][i]);
            elementBuilder.appendIndex(COLORS, i);
        }
    }

    shapeBuilder.appendElements(elementBuilder);
    _scene->getSceneBuilder()->appendShape(shapeBuilder);
    
    return shapeBuilder.getNode();
}

