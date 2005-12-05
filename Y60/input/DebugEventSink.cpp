//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DebugEventSink.cpp,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/02/01 11:18:35 $
//
//
//=============================================================================

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
                AxisEvent * theAxEvent = dynamic_cast<AxisEvent *>(&*theEvent);
                cerr << "AXIS: device=" << theAxEvent->device << ", axis=" << theAxEvent->axis <<
                    ", value=" << theAxEvent->value;
                cerr << endl;
            }
            break;
        case Event::BUTTON_DOWN:
        case Event::BUTTON_UP:
            {
                ButtonEvent * theButtonEvent = dynamic_cast<ButtonEvent *>(&*theEvent);
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
    }    
}

}

