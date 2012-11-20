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
#include "EmbeddedButton.h"

#include <iostream>

using namespace std;

namespace acgtk {

EmbeddedButton::EmbeddedButton() :
    _myClickAheadFlag(false)
{
}

EmbeddedButton::EmbeddedButton(const std::string & theIconFile) :
    _myClickAheadFlag(false)
{
    _myIcon = Gdk::Pixbuf::create_from_file(theIconFile);
    _myImage.set( _myIcon );
    // setup(); // [TS] Removed in favor for explicit call as a workaround for win32
}

void
EmbeddedButton::setup() {
    add(_myImage);

    Gdk::EventMask myFlags = get_events();
    myFlags |= Gdk::POINTER_MOTION_MASK;
    myFlags |= Gdk::BUTTON_PRESS_MASK;
    myFlags |= Gdk::BUTTON_RELEASE_MASK;
    myFlags |= Gdk::LEAVE_NOTIFY_MASK;
    myFlags |= Gdk::BUTTON_RELEASE_MASK;
    set_events(myFlags);

    modify_bg(Gtk::STATE_ACTIVE, Gdk::Color("#AAAAAA"));

    show_all();
}

bool
EmbeddedButton::on_leave_notify_event(GdkEventCrossing * /*theEvent*/) {
    leave();
    return true;
}

bool
EmbeddedButton::on_button_press_event(GdkEventButton * /*theEvent*/) {
    pressed();
    return true;
}

void
EmbeddedButton::pressed() {
    _myClickAheadFlag = true;
    set_state(Gtk::STATE_ACTIVE);
}

void
EmbeddedButton::released() {
    if(_myClickAheadFlag) {
        on_clicked();
    }
    set_state(Gtk::STATE_NORMAL);
    _myClickAheadFlag = false;
}

void
EmbeddedButton::leave() {
    _myClickAheadFlag = false;
    set_state(Gtk::STATE_NORMAL);
}

bool
EmbeddedButton::on_button_release_event(GdkEventButton * /*theEvent*/) {
    released();
    return true;
}

void
EmbeddedButton::on_clicked() {
    _myClickedSignal.emit();
}

void
EmbeddedButton::on_realize() {
    setup();
    Gtk::EventBox::on_realize();
}


} // end of namespace
