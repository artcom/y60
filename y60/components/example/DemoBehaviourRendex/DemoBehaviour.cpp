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
#include <y60/input/KeyEvent.h>
#include <y60/input/MouseEvent.h>

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
        (void)myKeyEvent;
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
        (void)myMouseEvent;
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
            default:
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
    EXPORT PlugInBase * DemoBehaviourRendex_instantiatePlugIn(DLHandle myDLHandle) {
        cerr << "DemoBehaviour_instantiatePlugIn" << endl;
        return new DemoBehaviourFactory(myDLHandle);
    }
}
