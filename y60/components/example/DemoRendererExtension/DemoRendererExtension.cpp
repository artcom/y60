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

#include <asl/base/PlugInBase.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/scene/Scene.h>

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
