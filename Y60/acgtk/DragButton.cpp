#include "DragButton.h"
#include <asl/Logger.h>

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
    EmbeddedButton::on_button_press_event(theEvent);
    _myDragInProgressFlag = true;
    _myLastX = theEvent->x;
    _myLastY = theEvent->y;
    _myDragStartSignal.emit(_myLastX, _myLastY);
    return true;
}

bool
DragButton::on_button_release_event(GdkEventButton * theEvent) {
    EmbeddedButton::on_button_release_event(theEvent);
    _myDragInProgressFlag = false;
    _myDragDoneSignal.emit();
    return true;
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
        _myLastX = theEvent->x;
        _myLastY = theEvent->y;

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
