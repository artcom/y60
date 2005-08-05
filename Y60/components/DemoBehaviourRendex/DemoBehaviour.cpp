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
//    $RCSfile: DemoBehaviour.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.8 $
//
// Description:
//
//=============================================================================

#include "DemoBehaviour.h"
#include <y60/KeyEvent.h>
#include <y60/MouseEvent.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace dom;
using namespace jslib;

#define DB(x) // x

namespace y60 {
    DemoBehaviour::DemoBehaviour() :
        IRendererExtension("DemoBehaviour")
    {
		AC_INFO << "DemoBehaviour::DemoBehaviour()" << endl;
    }

    void
    DemoBehaviour::onFrame(jslib::AbstractRenderWindow * theWindow, double theTime) {
		if (_myBody) {
			float myPeriod = 2.0f;
            Quaternionf myOrientation(Vector3f(0, 1, 0), (float)((theTime / myPeriod) * asl::PI));
			_myBody->set<OrientationTag>(myOrientation);
		}
    }

    void
    DemoBehaviour::onKey(y60::Event & theEvent) {
        y60::KeyEvent & myKeyEvent = dynamic_cast<y60::KeyEvent&>(theEvent);
        DB(
			cerr << "Key Event" << endl;
			cerr << "    string: " << myKeyEvent.keyString << endl;
			cerr << "    state:  " << (myKeyEvent.type == y60::Event::KEY_DOWN) << endl;
			cerr << "    shift:  " << ((myKeyEvent.modifiers & y60::KEYMOD_SHIFT) !=0) << endl;
			cerr << "    ctrl:   " << ((myKeyEvent.modifiers & y60::KEYMOD_CTRL) !=0) << endl;
			cerr << "    alt:    " << ((myKeyEvent.modifiers & y60::KEYMOD_ALT) !=0) << endl;
		);
    }

    void
    DemoBehaviour::onMouseButton(y60::Event & theEvent) {
        y60::MouseEvent & myMouseEvent = dynamic_cast<y60::MouseEvent&>(theEvent);
        DB(
			cerr << "MouseButton Event" << endl;
			cerr << "    button: " << myMouseEvent.button << endl;
			cerr << "    xpos:   " << myMouseEvent.xPosition << endl;
			cerr << "    ypos:   " << myMouseEvent.yPosition << endl;
		);
    }

    void
    DemoBehaviour::handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
        switch (theEvent->type) {
            case y60::Event::MOUSE_BUTTON_DOWN:
            case y60::Event::MOUSE_BUTTON_UP:
                onMouseButton(*theEvent);
                break;
            case y60::Event::KEY_UP:
            case y60::Event::KEY_DOWN:
                onKey(*theEvent);
                break;
        }
    }

    void
    DemoBehaviour::onGetProperty(const std::string & thePropertyName,
                     PropertyValue & theReturnValue) const
    {
        if (thePropertyName == "state") {
            theReturnValue.set<string>(_myState);
        } else if (thePropertyName == "position") {
            theReturnValue.set<asl::Vector2i>(_myPosition);
        } else {
            //cerr << "### WARNING: Unknown property: " << thePropertyName << endl;
        }
    }

    void
    DemoBehaviour::onSetProperty(const std::string & thePropertyName,
                     const PropertyValue & thePropertyValue)
    {
        if (thePropertyName == "state") {
            _myState = thePropertyValue.get<string>();
        } else if (thePropertyName == "position") {
            _myPosition = thePropertyValue.get<asl::Vector2i>();
        } else if (thePropertyName == "node") {
			dom::NodePtr myNode = thePropertyValue.get<dom::NodePtr>();
			_myBody = myNode->getFacade<Body>();
        } else {
            cerr << "### WARNING: Unknown property: " << thePropertyName << endl;
        }
    }

	void
	DemoBehaviour::onUpdateSettings(dom::NodePtr theSetting) {
		cerr << "Update Settings: \n" << *theSetting << endl;
	}

    extern "C"
    EXPORT PlugInBase * DemoBehaviour_instantiatePlugIn(DLHandle myDLHandle) {
        cerr << "DemoBehaviour_instantiatePlugIn" << endl;
        return new DemoBehaviourFactory(myDLHandle);
    }
}
