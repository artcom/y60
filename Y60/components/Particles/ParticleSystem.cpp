//============================================================================
//
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ParticleSystem.h"

#include <y60/JSVector.h>
#include <y60/Shape.h>
#include <y60/modelling_functions.h>
#include <y60/SceneBuilder.h>
#include <y60/JSScene.h>
#include <y60/WorldBuilder.h>
#include <y60/BodyBuilder.h>
#include <y60/AnimationBuilder.h>

#include <asl/numeric_functions.h>
#include <dom/Nodes.h>

#define DB(x)  x

using namespace std;
using namespace jslib;
using namespace y60;
using namespace asl;

/*******************************************************************************/
// static helper functions

static
float
getRandomNumber(float min, float max) {
    return min + (rand() / float(RAND_MAX)) * (max-min);
}

static
float
getRandomNumber(const asl::Vector2f & theMinMax) {
    return getRandomNumber(theMinMax[0], theMinMax[1]);
}

static
asl::Vector3f
getRandomVector(float min, float max) {
     return asl::Vector3f(getRandomNumber(min, max),
                          getRandomNumber(min, max),
                          getRandomNumber(min, max));
}

static
asl::Vector3f
getRandomVector(const asl::Vector2f & theMinMaxX, const asl::Vector2f & theMinMaxY, const asl::Vector2f & theMinMaxZ) {
    return asl::Vector3f(getRandomNumber(theMinMaxX[0], theMinMaxX[1]),
                         getRandomNumber(theMinMaxY[0], theMinMaxY[1]),
                         getRandomNumber(theMinMaxZ[0], theMinMaxZ[1]));
}

static
asl::Vector3f
computeScattering(const asl::Vector3f & theDirection, float theScatteringAngle) {
    Vector3f myOrthoVector1(1.0, 1.0, 1.0);

    if ((theDirection[2] >= 0.5) || (theDirection[2] <= -0.5)) {
        myOrthoVector1[2] = - (theDirection[0] + theDirection[1]) / theDirection[2]; 
    } else if ((theDirection[0] >= 0.5) || (theDirection[0] <= -0.5) ) {
        myOrthoVector1[0] = - (theDirection[1] + theDirection[2]) / theDirection[0]; 
    } else {
        myOrthoVector1[1] = - (theDirection[2] + theDirection[0]) / theDirection[1]; 
    }

    myOrthoVector1 = myOrthoVector1.normalize();

    Vector3f myOrthoVector2 = cross(myOrthoVector1, theDirection);
    myOrthoVector2 = myOrthoVector2.normalize();
        
    Vector3f myScatteredVector;
    float myAxisRotation = getRandomNumber(0, PI*2);
    float myRadScatteringAngle = radFromDeg(theScatteringAngle); 
    
    myScatteredVector[1] = cos(-myRadScatteringAngle) * sin(myAxisRotation);
    myScatteredVector[2] = sin(-myRadScatteringAngle);
    myScatteredVector[0] = cos(-myRadScatteringAngle) * cos(myAxisRotation);

    // myScatteredVector = myScatteredVector[0] * theDirection +
    //                     myScatteredVector[1] * myOrthoVector1 +
    //                     myScatteredVector[2] * myOrthoVector2;
    myScatteredVector = myScatteredVector[0] * myOrthoVector1 +
                        myScatteredVector[1] * myOrthoVector2 +
                        myScatteredVector[2] * theDirection;
   
    myScatteredVector = myScatteredVector.normalize();
    return myScatteredVector;
}

#if 0
var myX = myTrackballRadius * Math.cos(- _myTrackballOrientation.x) * Math.sin(_myTrackballOrientation.y);
var myY = myTrackballRadius * Math.sin(- _myTrackballOrientation.x);
var myZ = myTrackballRadius * Math.cos(- _myTrackballOrientation.x) * Math.cos(_myTrackballOrientation.y);
#endif

/*******************************************************************************/

ParticleSystem::ParticleSystem(y60::ScenePtr theScene)
    : _myParticleCount(0)
    , _myPointSize(5.0f, 0.0f, 64.0f)
    , _myScene(theScene)
{}

ParticleSystem::~ParticleSystem()
{}


void
ParticleSystem::create(dom::NodePtr theParentNode,
                       unsigned theParticleCount,
                       string theTextureName,
                       asl::Vector3f theInitialDirection,
                       asl::Vector2f theScatteringRange,
                       asl::Vector2f theSpeedRange)                            
{
    DB(AC_DEBUG << "ParticleSystem::create: fill vertexbuffer with particles data");

    _myParticleCount = theParticleCount;
    _myTextureName = theTextureName;
    _myInitialDirection = theInitialDirection.normalize();
    _mySpeedRange = theSpeedRange;
    _myParentNode = theParentNode;

    theScatteringRange[0] = (theScatteringRange[0]+180)/2;
    theScatteringRange[1] = (theScatteringRange[1]+180)/2;

    asl::Vector3f myPosition(0.0f, 0.0f, 0.0f);

    float theSize = 8.0f; // cg

    // create material
    _myMaterialNode = createUnlitTexturedColoredMaterial(_myScene, theTextureName, "ParticleSystemBaseTexture", true, 64);
    
    MaterialBaseFacadePtr myMaterial = _myMaterialNode->getFacade<MaterialBase>();
    MaterialPropertiesFacadePtr myPropFacade = myMaterial->getChild<MaterialPropertiesTag>();
    myPropFacade->set<PointSizeTag>(_myPointSize);

    dom::NodePtr myPointAttenuationProp = myPropFacade->getProperty(POINTATTENUATION_PROPERTY);
    dom::NodePtr myProperties = _myMaterialNode->childNode("properties");
    if (!myPointAttenuationProp) {
        dom::Node myNewNode(dom::Node::ELEMENT_NODE,"vector3f",0);
        myNewNode.appendAttribute("name",POINTATTENUATION_PROPERTY);
        myNewNode.appendChild(dom::Text("[1,0,100]"));
        myProperties->appendChild(myNewNode);
    }

    myProperties->appendChild(dom::Node("<float name='time'>0</float>").firstChild());
    myProperties->appendChild(dom::Node(string("<float name='stoptime'>") + asl::as_string(NumericTraits<float>::max()) + "</float>").firstChild());
    myProperties->appendChild(dom::Node("<float name='timetolive'>3.0</float>").firstChild());
    myProperties->appendChild(dom::Node("<vector3f name='gravity'>[0.0, 0.0, 0.0]</vector3f>").firstChild());
    myProperties->appendChild(dom::Node("<vector3f name='size'>[8.0, 2.0, 640.0]</vector3f>").firstChild());

    string myMaterialNodeId = _myMaterialNode->getAttribute("id")->nodeValue(); 
    
    TargetBuffers & myTargetBuffers = myPropFacade->getProperty(TARGETBUFFERS_PROPERTY)->nodeValueRefOpen<TargetBuffers>();
    myTargetBuffers = TargetBuffers((1<<RED_MASK)|(1<<GREEN_MASK)|(1<<BLUE_MASK)|(1<<ALPHA_MASK));
    myPropFacade->getProperty(TARGETBUFFERS_PROPERTY)->nodeValueRefClose<TargetBuffers>();

    // setting point attenuation
    asl::Vector3f & myValue = myPropFacade->getProperty(POINTATTENUATION_PROPERTY)->nodeValueRefOpen<asl::Vector3f>();
    myValue = asl::Vector3f(1.0, 1.0, 32.0);
    myPropFacade->getProperty(POINTATTENUATION_PROPERTY)->nodeValueRefClose<asl::Vector3f>();
    myMaterial->set<TransparencyTag>(true);            

    // set shader into vertex params
    dom::Node & myRequiresNode = *(_myMaterialNode->childNode("requires"));
    if (myRequiresNode.hasChildNodes()) {
        myRequiresNode.firstChild()->firstChild()->nodeValue("[100[particles]]");
    }

    // turn on sprite in texture
    dom::NodePtr myTextures = _myMaterialNode->childNode("textures"); 
    dom::NodePtr myTexture = myTextures->childNode("texture"); 
    myTexture->getAttribute("sprite")->nodeValue("1"); 

    // turn off mipmap in image
    _myImageNode = _myScene->getWorldRoot()->getElementById(myTexture->getAttributeString("image")); 
    _myImageNode->getAttribute("mipmap")->nodeValue("0"); 

    // setup vertices 
    ShapeBuilder myShapeBuilder("Particles"); 
    _myScene->getSceneBuilder()->appendShape(myShapeBuilder); 

    myShapeBuilder.createVertexDataBin<asl::Vector3f>(POSITION_ROLE, _myParticleCount); 
    myShapeBuilder.createVertexDataBin<asl::Vector4f>(COLOR_ROLE, _myParticleCount); 
    myShapeBuilder.createVertexDataBin<asl::Vector3f>(NORMAL_ROLE, 1); 
    myShapeBuilder.createVertexDataBin<asl::Vector4f>(TEXCOORD0_ROLE, 1); // texture 
    myShapeBuilder.createVertexDataBin<asl::Vector4f>(TEXCOORD1_ROLE, _myParticleCount); // start direction (xyz) time (w) 
    myShapeBuilder.createVertexDataBin<asl::Vector4f>(TEXCOORD2_ROLE, _myParticleCount); // additional parameters (lifetime, size, speed)

    asl::Vector3f myNormal(1.0f,0.0f,0.0f); 
    myShapeBuilder.appendVertexData(NORMAL_ROLE, myNormal);

    asl::Vector4f myUvCoord(0.0f, 0.0f, 0.0f, 0.0f);
    myShapeBuilder.appendVertexData(TEXCOORD0_ROLE, myUvCoord);

    for (unsigned myIndex = 0; myIndex < _myParticleCount; ++myIndex) {
        myShapeBuilder.appendVertexData(COLOR_ROLE, asl::Vector4f(1.0f,1.0f,1.0f,1.0f));          
        
        myPosition = asl::Vector3f(0.0f, 0.0f, 0.0f); // getRandomVector(0, 10);
        myShapeBuilder.appendVertexData(POSITION_ROLE, myPosition);

        float myScattering = getRandomNumber(theScatteringRange[0], theScatteringRange[1]);
        Vector3f myNewDirection = computeScattering(_myInitialDirection, myScattering);
        
        Vector4f myDirectionVector;
        myDirectionVector[0] = myNewDirection[0];
        myDirectionVector[1] = myNewDirection[1];
        myDirectionVector[2] = myNewDirection[2];
        myDirectionVector[3] = -(float(myIndex)/float(_myParticleCount)); 

        myShapeBuilder.appendVertexData(TEXCOORD1_ROLE, myDirectionVector);

        float mySpeed = getRandomNumber(_mySpeedRange[0], _mySpeedRange[1]);
        
        Vector4f myParameters(/*theTimeToLive*/ 0.0f, theSize, mySpeed, 0.0f);
        myShapeBuilder.appendVertexData(TEXCOORD2_ROLE, myParameters);
    }
    
    ElementBuilderPtr myElement = ElementBuilderPtr(new ElementBuilder("points", myMaterial->get<IdTag>()));
    myElement->createIndex(POSITION_ROLE, POSITIONS);
    myElement->createIndex(NORMAL_ROLE, NORMALS);
    myElement->createIndex(TEXCOORD0_ROLE, TEXCOORD0);
    myElement->createIndex(TEXCOORD1_ROLE, TEXCOORD1);
    myElement->createIndex(TEXCOORD2_ROLE, TEXCOORD2);
    myElement->createIndex(COLOR_ROLE, COLORS);
    
    for (unsigned myIndex = 0; myIndex < _myParticleCount; ++myIndex) {
        myElement->appendIndex(POSITIONS, myIndex);
        myElement->appendIndex(NORMALS, 0);
        myElement->appendIndex(TEXCOORD0, 0);
        myElement->appendIndex(TEXCOORD1, myIndex);
        myElement->appendIndex(TEXCOORD2, myIndex);
        myElement->appendIndex(COLORS, myIndex);
    }
    myShapeBuilder.appendElements(*myElement);
    
    dom::NodePtr myShapeNode = myShapeBuilder.getNode();
    _myShape = myShapeNode->getFacade<Shape>();

    // _myBodyNode = createBody(_myScene->getWorldRoot(), _myShape->get<IdTag>());
    _myBodyNode = createBody(_myParentNode, _myShape->get<IdTag>());
    
    dom::NodePtr myVertexDataNode = myShapeNode->childNode("vertexdata");
    _myParticleVertexDataPtr = myVertexDataNode->childNodeByAttribute("vectorofvector3f", "name", "position")->firstChild();    

    // build animation node
    y60::AnimationBuilder myAnimationBuilder;
    _myScene->getSceneBuilder()->appendAnimation(myAnimationBuilder);
    myAnimationBuilder.setName("particlesAnimation");
    myAnimationBuilder.setNodeRef(myMaterialNodeId);
    myAnimationBuilder.setProperty("time");
    myAnimationBuilder.setDuration(1000000000.0f);
    myAnimationBuilder.setEnable(true);
    
    VectorOfFloat myFloatValues;
    myFloatValues.push_back(0.0f);
    myFloatValues.push_back(1000000000.0f);
    myAnimationBuilder.appendValues(myFloatValues);
    _myAnimationNode = myAnimationBuilder.getNode();
}

void
ParticleSystem::remove() {
}

