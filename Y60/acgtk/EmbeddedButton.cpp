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
