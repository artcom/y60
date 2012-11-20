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

//own header
#include "DragButton.h"

#include <asl/base/Logger.h>

#ifdef LINUX
#   include <gdk/gdkx.h>
#endif

namespace acgtk {

DragButton::DragButton() :
    EmbeddedButton(),
    _myDragInProgressFlag(false)
{
}

DragButton::DragButton(const std::string & theIconFile) :
    EmbeddedButton(theIconFile),
    _myDragInProgressFlag(false)
{
}
bool
DragButton::on_button_press_event(GdkEventButton * theEvent) {
    _myLastX = int(theEvent->x);
    _myLastY = int(theEvent->y);
    _myPressedButton = theEvent->button;
    EmbeddedButton::on_button_press_event(theEvent);
    return true;
}

void
DragButton::pressed() {
    EmbeddedButton::pressed();
    _myDragInProgressFlag = true;
    _myDragStartSignal.emit(_myLastX, _myLastY, _myPressedButton);
}

/* moved to released()
bool
DragButton::on_button_release_event(GdkEventButton * theEvent) {
    EmbeddedButton::on_button_release_event(theEvent);
    _myDragInProgressFlag = false;
    _myDragDoneSignal.emit();
    return true;
}
*/

void
DragButton::released() {
    EmbeddedButton::released();
    _myDragInProgressFlag = false;
    _myDragDoneSignal.emit();
}

bool
DragButton::on_leave_notify_event(GdkEventCrossing * theEvent) {
    // don't call base class
    return true;
}


bool
DragButton::on_motion_notify_event(GdkEventMotion * theEvent) {
    if (_myDragInProgressFlag) {
        double myDeltaX = theEvent->x - _myLastX;
        double myDeltaY = theEvent->y - _myLastY;
        _myDragSignal.emit(myDeltaX, myDeltaY);
#if WIN32
        POINT absMousePos;
        GetCursorPos(&absMousePos);
        SetCursorPos(absMousePos.x-int(myDeltaX), absMousePos.y-int(myDeltaY));
#else
        _myLastX = int( theEvent->x);
        _myLastY = int( theEvent->y);
#endif

/* TODO: find a way to filter the events ... [DS]
        GdkWindow * myGdkWindow = get_window()->gobj();
        Display * myXDisplay = GDK_WINDOW_XDISPLAY( myGdkWindow );
        XWarpPointer(myXDisplay, None, None, 0, 0, 0, 0,
                     int(- myDeltaX), int(- myDeltaY));
        */
    }
    return true;
}

}
