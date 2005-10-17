//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: DemoModellingRendex.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.15 $
//
// Description:
//
//=============================================================================

#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/modelling_functions.h>
#include <y60/WorldBuilder.h>
#include <y60/SceneBuilder.h>
#include <y60/BodyBuilder.h>
#include <y60/JSScriptablePlugin.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace dom;
using namespace y60;

DEFINE_EXCEPTION(CubeNotFound, asl::Exception)

    struct Particle {
	asl::Vector3f startPosition;
	asl::Vector3f velocity;
	double birthTime;
    };


class DemoModellingRendex :
    public PlugInBase,
    public y60::IRendererExtension,
    public jslib::IScriptablePlugin
{
    public:
	DemoModellingRendex(asl::DLHandle theDLHandle);

	void onStartup(jslib::AbstractRenderWindow * theWindow);

	bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

	void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
	void onFrame(AbstractRenderWindow * theWindow , double t);

	void onPreRender(AbstractRenderWindow * theRenderer);
	void onPostRender(AbstractRenderWindow * theRenderer);

	const char * ClassName() {
	    static const char * myClassName = "demoModellingRendex";
	    return myClassName;
	}

	void onUpdateSettings(dom::NodePtr theSettings) {
	    cerr << "New Settings: " << *theSettings << endl;
	}

	void onGetProperty(const std::string & thePropertyName,
		PropertyValue & theReturnValue) const
	{
	    if (thePropertyName == "state") {
		//theReturnValue.set<string>(_myState);
	    } else if (thePropertyName == "position") {
		//theReturnValue.set<asl::Vector2i>(_myPosition);
	    } else {
		AC_WARNING << "Unknown property: " << thePropertyName << endl;
	    }
	}

	void onSetProperty(const std::string & thePropertyName,
		const PropertyValue & thePropertyValue)
	{
	    if (thePropertyName == "state") {
		//_myState = thePropertyValue.get<string>();
	    } else if (thePropertyName == "position") {
		//_myPosition = thePropertyValue.get<asl::Vector2i>();
	    } else if (thePropertyName == "node") {
		// dom::NodePtr myNode = thePropertyValue.get<dom::NodePtr>();
		//_myBody = myNode->getFacade<Body>();
	    } else {
		AC_WARNING << "Unknown property: " << thePropertyName << endl;
	    }
	}
    private:
	NodePtr _myCube;
	y60::BodyPtr _myCubeBody;
	NodePtr _myParticleShape;
	bool _myFirstTime;

	std::vector<std::vector<Particle> > _myParticleChunks;
	double _lastTime;
};

DemoModellingRendex :: DemoModellingRendex(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("DemoModellingRendex"),
    _myFirstTime(true)
{}

void
DemoModellingRendex :: onStartup(jslib::AbstractRenderWindow * theWindow) {
    cerr << "onStartup" << endl;
}

bool
DemoModellingRendex :: onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    cerr << "onSceneLoaded" << endl;

    y60::ScenePtr myScene;
    myScene = theWindow->getRenderer()->getCurrentScene();
    if (myScene) {
	NodePtr mySceneDom = myScene->getSceneDom();
	_myCube = mySceneDom->getElementById("b0");
	if (!_myCube) {
	    throw CubeNotFound("Body 'b0' not found.", PLUS_FILE_LINE);
	}
	_myCubeBody = _myCube->getFacade<Body>();
    } else {
	cerr << "onSceneLoaded: no scene found." << endl;
    }

    dom::NodePtr myMaterial = y60::createUnlitTexturedMaterial(
	    myScene, "../../som/testfiles/test3dtexture.jpg", "myMaterial", false, true, 8);
    dom::NodePtr myPointSizeNode = dom::NodePtr(new dom::Node("<vector3f name='pointsize'>[50,1,30]</vector3f>"))->firstChild();
    myMaterial->childNode("properties")->appendChild(myPointSizeNode);

    dom::NodePtr myPointAttNode = dom::NodePtr(new dom::Node("<vector3f name='pointattenuation'>[1,0,0.01]</vector3f>"))->firstChild();
    myMaterial->childNode("properties")->appendChild(myPointAttNode);

    dom::NodePtr myShape = createPlane(
	    myScene, 50, 50,
	    "myTest",
	    (*myMaterial)["id"].nodeValue(),
	    PointBuilder(),
	    PlanePosition(asl::Point3<float>(-5.0f,-5.0f,0.0f),
		asl::Vector3<float>(0.05f, 0.0f, 0.0f),
		asl::Vector3<float>(0.0f, 0.05f, 0.0f)),
	    ConstNormal(),
	    PlaneUVCoord(),
	    ConstColor());

    std::string myShapeId = myShape->getAttributeString(y60::ID_ATTRIB);
    std::string myName("Body");

    dom::DocumentPtr mySceneDoc = myScene->getSceneDom();
    SceneBuilder mySceneBuilder(mySceneDoc);
    WorldBuilder myWorldBuilder(myScene->getWorldRoot());
    mySceneBuilder.appendWorld(myWorldBuilder);
    BodyBuilder myBodyBuilder(myShapeId, myName);
    myWorldBuilder.appendObject(myBodyBuilder);

    _myParticleShape = myShape;
    //  cerr << *myScene->getSceneDom();

    return true;
}

void
DemoModellingRendex :: handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}

inline
float random(float min, float max) {
    return min + rand()/float(RAND_MAX) * (max - min);
}
inline Vector3f randomVector3f(float min, float max) {
    return asl::Vector3f(random(min,max), random(min,max), random(min,max));
}

DEFINE_EXCEPTION(NoShapeFacade, asl::Exception)

    void
    DemoModellingRendex :: onFrame(AbstractRenderWindow * theWindow , double t) {
	// cerr << "onFrame t=" << t << endl;
	float myPeriod = 2.0f;

	Quaternionf myOrientation = Quaternionf::createFromEuler(Vector3f(0, (float)((t / myPeriod) * asl::PI), 0));
	_myCubeBody->set<OrientationTag>(myOrientation);

	if (_myParticleShape) {
	    ShapePtr myShape = _myParticleShape->getFacade<Shape>();
	    if (!myShape) {
		throw NoShapeFacade(JUST_FILE_LINE);
	    }
	    PrimitiveVector & myPrimitives = myShape->getPrimitives();

	    if (_myFirstTime) {
		_myParticleChunks.resize(myPrimitives.size());
	    }

	    for (unsigned i = 0; i < myPrimitives.size(); ++i) {
#ifdef OLD
		VertexData3f::VertexDataVector * myVertexPositionsPtr = myPrimitives[i].getPositions();
		VertexData2f::VertexDataVector * myUVCoordsPtr = myPrimitives[i].getTexCoords2f(0);
#else
		Ptr<VertexDataAccessor<Vector3f> > myPositionsAccessor = myPrimitives[i]->getLockingPositionsAccessor();
		VertexData3f * myVertexPositionsPtr = &myPositionsAccessor->get();

		Ptr<VertexDataAccessor<Vector2f> > myTexCoordsAccessor = myPrimitives[i]->getLockingTexCoord2fAccessor(0);
		VertexData2f * myUVCoordsPtr = &myTexCoordsAccessor->get();
#endif
		if (myVertexPositionsPtr && myUVCoordsPtr) {
		    VertexData3f & myVertexPositions = *myVertexPositionsPtr;
		    if (_myFirstTime) {
			std::vector<Particle> & myParticles = _myParticleChunks[i];
			myParticles.resize(myVertexPositions.size());
			for (unsigned v = 0; v < myVertexPositions.size(); ++v) {
			    myParticles[v].startPosition = myVertexPositions[v];
			    myParticles[v].birthTime = t + random(0.0f,3.0f);
			    myParticles[v].velocity = randomVector3f(0.0f,0.1f);;
			}
			_myFirstTime = false;
		    } else {
			glFlush();
			double dt = t - _lastTime;
			VertexData2f & myUVCoords = *myUVCoordsPtr;
			std::vector<Particle> & myParticles = _myParticleChunks[i];
			for (unsigned v = 0; v < myVertexPositions.size(); ++v) {
			    myVertexPositions[v] = myVertexPositions[v] + myParticles[v].velocity * dt;
			    if (t - myParticles[v].birthTime > 10) {
				// compact
				myParticles[v].velocity -= myVertexPositions[v] * random(0.0f,0.001f);
			    } else {
				// expand
				myParticles[v].velocity += randomVector3f(-0.1f,0.1f);
			    }
			    myUVCoords[v] = asl::Vector2f(float(myParticles[v].birthTime + t/3), 0.f);
			}
		    }
		}
	    }
	    myShape->setLastRenderVersion(myShape->getNode().nodeVersion()+1);
	}
	_lastTime = t;
    }

void
DemoModellingRendex :: onPreRender(AbstractRenderWindow * theRenderer) {
}

void
DemoModellingRendex :: onPostRender(AbstractRenderWindow * theRenderer) {
}

extern "C"
EXPORT PlugInBase* demoModellingRendex_instantiatePlugIn(DLHandle myDLHandle) {
    return new DemoModellingRendex(myDLHandle);
}
