#ifndef _ParticleSystem_h_
#define _ParticleSystem_h_

#include <asl/PlugInBase.h>

#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/JSNode.h>
#include <y60/JSVector.h>
#include <y60/MaterialBase.h>
#include <y60/modelling_functions.h>
#include <y60/ShapeBuilder.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Shape.h>

#include <iostream>
#include <vector>


class ParticleSystem {
public:
    ParticleSystem(const y60::ScenePtr & theScene);
    ~ParticleSystem();

    void create( const dom::NodePtr & theParentNode,
                 unsigned theParticleCount=1000,
                 const std::string & theTextureName="1.png",
                 const asl::Vector3f & theInitialDirection=asl::Vector3f(0.0f, 1.0f, 0.0f),
                 const asl::Vector2f & theScattering=asl::Vector2f(0.0f, 45.0f),
                 const asl::Vector2f & theSpeedRange=asl::Vector2f(10.0, 20.0) );

    void remove();
        
    dom::NodePtr getMaterialNode() const {
        return _myMaterialNode;
    }
    dom::NodePtr getBodyNode() const {
        return _myBodyNode;
    }
    dom::NodePtr getImageNode() const {
        return _myImageNode;
    }
    dom::NodePtr getAnimationNode() const {
        return _myAnimationNode;
    }

private:
    unsigned _myParticleCount;
    std::string _myTextureName;
    asl::Vector3f _myInitialDirection;
    asl::Vector2f _mySpeedRange;
    
    dom::NodePtr  _myParticleVertexDataPtr;
    
    y60::ScenePtr _myScene;
    y60::ShapePtr _myShape;
    dom::NodePtr  _myBodyNode;
    dom::NodePtr  _myMaterialNode;
    dom::NodePtr  _myImageNode;
    dom::NodePtr  _myAnimationNode;
    dom::NodePtr  _myParentNode;
    
    asl::Vector3f _myPointSize;
};

#endif // _ParticleSystem_h_
