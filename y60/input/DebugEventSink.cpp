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
*/
//
//   $RCSfile: DebugEventSink.cpp,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/02/01 11:18:35 $
//
//
//=============================================================================

// own header
#include "DebugEventSink.h"

#include "AxisEvent.h"
#include "ButtonEvent.h"

#include <iostream>

using namespace std;

namespace y60 {

DebugEventSink::DebugEventSink()
{
}

void DebugEventSink::handle(EventPtr theEvent) {
    switch(theEvent->type) {
        case Event::KEY_UP:
            cerr << "KEY_UP" << endl;
            break;
        case Event::KEY_DOWN:
            cerr << "KEY_DOWN" << endl;
            break;
        case Event::MOUSE_MOTION:
//            cerr << "MOUSE_MOTION" << endl;
            break;
        case Event::MOUSE_BUTTON_UP:
            cerr << "MOUSE_BUTTON_UP" << endl;
            break;
        case Event::MOUSE_BUTTON_DOWN:
            cerr << "MOUSE_BUTTON_DOWN" << endl;
            break;
        case Event::AXIS:
            {
                AxisEvent * theAxEvent = dynamic_cast<AxisEvent *>(theEvent.get());
                cerr << "AXIS: device=" << theAxEvent->device << ", axis=" << theAxEvent->axis <<
                    ", value=" << theAxEvent->value;
                cerr << endl;
            }
            break;
        case Event::BUTTON_DOWN:
        case Event::BUTTON_UP:
            {
                ButtonEvent * theButtonEvent = dynamic_cast<ButtonEvent *>(theEvent.get());
                if (theEvent->type == Event::BUTTON_DOWN) {
                    cerr << "BUTTON_DOWN";
                } else {
                    cerr << "BUTTON_UP";
                }
                cerr << ": device=" << theButtonEvent->device
                        << ", button=" << theButtonEvent->button << endl;
            }
            break;
        case Event::RESIZE:
            cerr << "RESIZE" << endl;
            break;
        case Event::QUIT:
            cerr << "QUIT" << endl;
            break;
        //XXX provide more output for the following events:
        case Event::MOUSE_WHEEL:
            cerr << "MOUSE_WHEEL" << endl;
            break;
        case Event::TOUCH:
            cerr << "TOUCH" << endl;
            break;
        case Event::GENERIC:
            cerr << "GENERIC" << endl;
            break;
    }
}

}

