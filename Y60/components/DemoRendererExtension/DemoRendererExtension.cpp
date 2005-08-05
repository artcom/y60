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
//    $RCSfile: DemoRendererExtension.cpp,v $
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
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace dom;
using namespace y60;

DEFINE_EXCEPTION(CubeNotFound, asl::Exception)

    class DemoRendererExtension :
        public PlugInBase,
        public y60::IRendererExtension,
        public IScriptablePlugin
    {
    public:
        DemoRendererExtension(asl::DLHandle theDLHandle);

        void onStartup(jslib::AbstractRenderWindow * theWindow);

        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

        void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
        void onFrame(AbstractRenderWindow * theWindow , double t);

        void onPreRender(AbstractRenderWindow * theRenderer);
        void onPostRender(AbstractRenderWindow * theRenderer);

		const char * ClassName() {
		    static const char * myClassName = "DemoRendererExtension";
		    return myClassName;
		}

		void onUpdateSettings(dom::NodePtr theSettings) {
		    AC_PRINT << "New Settings: " << *theSettings << endl;
		}

    private:
        NodePtr _myCube;
        y60::BodyPtr _myCubeBody;
    };

DemoRendererExtension :: DemoRendererExtension(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("DemoRendererExtension")
{
}

void
DemoRendererExtension :: onStartup(jslib::AbstractRenderWindow * theWindow) {
}

bool
DemoRendererExtension :: onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    y60::ScenePtr myScene;
    AC_DEBUG << "onSceneLoaded...";
    myScene = theWindow->getRenderer()->getCurrentScene();
    if (myScene) {
        NodePtr mySceneDom = myScene->getSceneDom();
        _myCube = mySceneDom->getElementById("b0");
        if (!_myCube) {
            throw CubeNotFound("Body 'b0' not found.", PLUS_FILE_LINE);
        }
        _myCubeBody = _myCube->getFacade<Body>();
    } else {
        AC_ERROR << "onSceneLoaded: no scene found." << endl;
    }
    AC_DEBUG << "...onSceneLoaded";

    return true;
}

void
DemoRendererExtension :: handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}

void
DemoRendererExtension :: onFrame(AbstractRenderWindow * theWindow , double t) {
    float myPeriod = 2.0f;

    AC_DEBUG << "onFrame...";
    Quaternionf myOrientation(Vector3f(0, 1, 0), (float)((t / myPeriod) * asl::PI));
    _myCubeBody->set<OrientationTag>(myOrientation);

    //theWindow->getRenderer()->getCurrentScene()->updateGlobalMatrix(&*_myCube);
    AC_DEBUG << "...onFrame";
}

void
DemoRendererExtension :: onPreRender(AbstractRenderWindow * theRenderer) {
}

void
DemoRendererExtension :: onPostRender(AbstractRenderWindow * theRenderer) {
}

extern "C"
EXPORT PlugInBase* DemoRendererExtension_instantiatePlugIn(DLHandle myDLHandle) {
    return new DemoRendererExtension(myDLHandle);
}
