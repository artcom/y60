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

//own header
#include "CWRuler.h"

#include "CellRendererPixbufToggle.h"
#include "TNTMeasurementList.h"

#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>

#include <iostream>

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/main.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gdkmm/drawable.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

using namespace std;

namespace acgtk {

const static float MIN_WINDOW_WIDTH(10.0);

CWRuler::CWRuler(Mode theMode) :
    Gtk::DrawingArea(),
    _myMode(theMode),
    _myState(IDLE),
    _myValueRange(0.0, 1.0),
    _myWindowCenter(0.0),
    _myWindowWidth(0.0),
    _myLower(0.0),
    _myUpper(0.0)
{
    set_size_request(256, 10);
    Gdk::EventMask myFlags = get_events();
    myFlags |= Gdk::POINTER_MOTION_MASK;
    myFlags |= Gdk::BUTTON_PRESS_MASK;
    myFlags |= Gdk::BUTTON_RELEASE_MASK;
    set_events(myFlags);
}

CWRuler::~CWRuler() {
}


void
CWRuler::on_realize() {
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  _myWindow = get_window();
  _myWindow->clear();
}

bool
CWRuler::on_button_press_event(GdkEventButton * theEvent) {
    //cerr << "CWRuler::on_button_press_event()" << endl;

    float myHWindowWidth = 0.5f * _myWindowWidth;


    if (_myMode == MODE_CENTER_WIDTH) {
        int myCenterMarkerPos = convertValueToScreenPos(_myWindowCenter);
        int myLeftMarkerPos = convertValueToScreenPos(_myWindowCenter - myHWindowWidth);
        int myRightMarkerPos = convertValueToScreenPos(_myWindowCenter + myHWindowWidth);
        if (intersectWithMarker(theEvent, myLeftMarkerPos)) {
            _myState = CHANGE_WIDTH_LEFT;
        } else if (intersectWithMarker(theEvent, myRightMarkerPos)) {
            _myState = CHANGE_WIDTH_RIGHT;
        } else if (intersectWithMarker(theEvent, myCenterMarkerPos)) {
            _myState = CHANGE_CENTER;
        }
    } else if (_myMode == MODE_THRESHOLD) {
        int myCenterMarkerPos = convertValueToScreenPos(_myWindowCenter);
        if (intersectWithMarker(theEvent, myCenterMarkerPos)) {
            _myState = CHANGE_CENTER;
        }
    } else if (_myMode == MODE_LOWER_UPPER) {
        int myLeftMarkerPos = convertValueToScreenPos(_myLower);
        int myRightMarkerPos = convertValueToScreenPos(_myUpper);
        if (intersectWithMarker(theEvent, myLeftMarkerPos)) {
            _myState = CHANGE_WIDTH_LEFT;
        } else if (intersectWithMarker(theEvent, myRightMarkerPos)) {
            _myState = CHANGE_WIDTH_RIGHT;
        }
    }
    return true;
}

bool
CWRuler::on_button_release_event(GdkEventButton * theEvent) {
    //cerr << "CWRuler::on_button_release_event()" << endl;
    _myState = IDLE;
    return true;
}

bool
CWRuler::on_motion_notify_event(GdkEventMotion * theEvent) {
    if ( _myState != IDLE) {
        float myValue = convertScreenPosToValue(int(theEvent->x));
        if (_myMode == MODE_LOWER_UPPER) {
            switch (_myState) {
                case CHANGE_WIDTH_LEFT:
                    myValue = min(max(myValue, _myValueRange[0]), _myUpper - MIN_WINDOW_WIDTH);
                    if (myValue != _myLower) {
                        _myLower = myValue;
                        _myLowerChangedSignal.emit(_myLower);
                    }
                    break;
                case CHANGE_WIDTH_RIGHT:
                    myValue = max(min(myValue, _myValueRange[1]), _myLower + MIN_WINDOW_WIDTH);
                    if (myValue != _myUpper) {
                        _myUpper = myValue;
                        _myUpperChangedSignal.emit(_myUpper);
                    }
                    break;
                case IDLE:
                case CHANGE_CENTER:
                    break;
            }
        } else {
            switch (_myState) {
                case CHANGE_CENTER:
                    myValue = min(max(myValue, _myValueRange[0]), _myValueRange[1]);
                    if (myValue != _myWindowCenter) {
                        _myWindowCenter = myValue;
                        _myCenterChangedSignal.emit(_myWindowCenter);
                    }
                    break;
                case CHANGE_WIDTH_LEFT:
                    myValue = max(min(2.0f * (_myWindowCenter - myValue),
                                2.0f * (_myWindowCenter - _myValueRange[0])), MIN_WINDOW_WIDTH);
                    if (myValue != _myWindowWidth) {
                        _myWindowWidth = myValue;
                        _myWidthChangedSignal.emit(_myWindowWidth);
                    }
                    break;
                case CHANGE_WIDTH_RIGHT:
                    myValue = max(min(2.0f * (myValue - _myWindowCenter),
                                2.0f *(_myValueRange[1] - _myWindowCenter)), MIN_WINDOW_WIDTH);
                    if (myValue != _myWindowWidth) {
                        _myWindowWidth = myValue;
                        _myWidthChangedSignal.emit(_myWindowWidth);
                    }
                    break;
                case IDLE:
                    break;
            }
        }
        queue_draw();
    }
    return true;
}

bool
CWRuler::on_expose_event(GdkEventExpose * theEvent) {
    //cerr << "CWRuler::on_expose_event()" << endl;
    DrawingArea::on_expose_event(theEvent);

    _myWindow->clear();

    if (_myMode == MODE_CENTER_WIDTH) {
        float myHWindowWidth = 0.5f * _myWindowWidth;

        int myXStart = convertValueToScreenPos(_myWindowCenter - myHWindowWidth);
        int myXEnd = convertValueToScreenPos(_myWindowCenter + myHWindowWidth);

        _myWindow->draw_rectangle(get_style()->get_dark_gc(get_state()), true, myXStart, 0,
                myXEnd - myXStart, get_allocation().get_height());

        drawMarker(_myWindowCenter + myHWindowWidth, get_style()->get_white_gc());
        drawMarker(_myWindowCenter - myHWindowWidth, get_style()->get_white_gc());
        drawMarker(_myWindowCenter, get_style()->get_black_gc());
    } else if (_myMode == MODE_LOWER_UPPER) {
        int myXStart = convertValueToScreenPos(_myLower);
        int myXEnd = convertValueToScreenPos(_myUpper);

        _myWindow->draw_rectangle(get_style()->get_dark_gc(get_state()), true, myXStart, 0,
                myXEnd - myXStart, get_allocation().get_height());

        drawMarker(_myLower, get_style()->get_white_gc());
        drawMarker(_myUpper, get_style()->get_white_gc());

    } else if (_myMode == MODE_THRESHOLD) {
        drawMarker(_myWindowCenter, get_style()->get_black_gc());
    }

    return true;
}

void
CWRuler::drawMarker(float thePosition, Glib::RefPtr<Gdk::GC> theGC) {

    int myHeight = get_allocation().get_height();
    int myXPos = convertValueToScreenPos(thePosition);
    std::vector<Gdk::Point> myPoints;

    int myTriangleSize = myHeight;
    myPoints.push_back(Gdk::Point(myXPos, 0));
    myPoints.push_back(Gdk::Point(myXPos + int(0.5 * myTriangleSize), myTriangleSize ));
    myPoints.push_back(Gdk::Point(myXPos - int(0.5 * myTriangleSize), myTriangleSize ));
    _myWindow->draw_polygon(theGC, true, myPoints);
}

int
CWRuler::convertValueToScreenPos(const float & theValue) {
    int myWidth = get_allocation().get_width();
    return int((theValue - _myValueRange[0])* float(myWidth) / (_myValueRange[1] - _myValueRange[0]));
}

float
CWRuler::convertScreenPosToValue(const int & theScreenPos) {
    int myWidth = get_allocation().get_width();
    return (theScreenPos * (_myValueRange[1] - _myValueRange[0]) / myWidth) + _myValueRange[0];
}

bool
CWRuler::intersectWithMarker(GdkEventButton * theEvent, int theMarkerPos) {
    float myHTriangleBase = 0.5f * get_allocation().get_height();
    if (theEvent->x > (theMarkerPos - myHTriangleBase) &&
        theEvent->x < (theMarkerPos + myHTriangleBase))
    {
        return true;
    }
    return false;
}

void
CWRuler::setValueRange(const asl::Vector2f & theValueRange) {
    _myValueRange = theValueRange;
    queue_draw();
}

const asl::Vector2f &
CWRuler::getValueRange() const {
    return _myValueRange;
}

void
CWRuler::setWindowCenter(float theCenter) {
    if (theCenter != _myWindowCenter) {
        _myWindowCenter = theCenter;
        _myCenterChangedSignal.emit(_myWindowCenter);
    }
    queue_draw();
}

float
CWRuler::getWindowCenter() const {
    return _myWindowCenter;
}

void
CWRuler::setWindowWidth(float theWidth) {
    if (theWidth != _myWindowWidth) {
        _myWindowWidth = theWidth;
        _myWidthChangedSignal.emit(_myWindowWidth);
    }
    queue_draw();
}

float
CWRuler::getWindowWidth() const {
    return _myWindowWidth;
}

void
CWRuler::setLower(float theLower) {
    if (theLower != _myLower) {
        _myLower = theLower;
        _myLowerChangedSignal.emit(_myLower);
    }
    queue_draw();
}

float
CWRuler::getLower() const {
    return _myLower;
}

void
CWRuler::setUpper(float theUpper) {
    if (theUpper != _myUpper) {
        _myUpper = theUpper;
        _myUpperChangedSignal.emit(_myUpper);
    }
    queue_draw();
}

float
CWRuler::getUpper() const {
    return _myUpper;
}

void
CWRuler::setMode(CWRuler::Mode theMode) {
    _myMode = theMode;
    queue_draw();
}

CWRuler::Mode
CWRuler::getMode() const {
    return _myMode;
}

} // end of namespace

#if 0
//==== test executable ==========================
#include <gtkmm/window.h>
#include "Histogram.h"
#include "GrayScale.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>

class TWindow : public Gtk::Window {
    public:
        TWindow() :
            _myRuler(acgtk::CWRuler::MODE_CENTER_WIDTH),
            _myListView("/tmp/smallicon_visible.png",
                        "/tmp/smallicon_hidden.png",
                        "/tmp/smallicon_locked.png",
                        "/tmp/smallicon_editable.png")
        {
            set_default_size(256, 15);
            add(_myBox);

            _myBox.pack_start(_myHistogramFrame, Gtk::PACK_SHRINK);
            _myHistogramFrame.add(_myHistogram);
            _myHistogramFrame.set_shadow_type(Gtk::SHADOW_IN);

            _myBox.pack_start(_myRulerFrame, Gtk::PACK_SHRINK, 0);
            _myRulerFrame.add(_myRuler);
            _myRulerFrame.set_shadow_type(Gtk::SHADOW_IN);

            _myHistogram.set_size_request(256, 50);
            _myHistogram.setValueRange(asl::Vector2f(0.0, 255.0));
            _myHistogram.setShowWindowCenter(true);
            _myHistogram.setShowWindow(true);
            _myHistogram.setWindowCenter(128.0);
            _myHistogram.setWindowWidth(64.0);

            _myRuler.signal_center_changed().connect(
                    sigc::mem_fun(_myHistogram, &acgtk::Histogram::setWindowCenter) );
            _myRuler.signal_width_changed().connect(
                    sigc::mem_fun(_myHistogram, &acgtk::Histogram::setWindowWidth) );

            _myBox.pack_start(_myGrayScale, Gtk::PACK_SHRINK, 0);

            // ListView stuff
            _myScrolledWindow.add(_myListView);
            _myScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            _myBox.pack_start(_myScrolledWindow);
            show_all_children();
        }
        virtual ~TWindow() {}
    private:

        Gtk::VBox        _myBox;
        Gtk::Frame       _myRulerFrame;
        acgtk::CWRuler   _myRuler;
        Gtk::Frame       _myHistogramFrame;
        acgtk::Histogram _myHistogram;
        acgtk::GrayScale _myGrayScale;

        // ListView stuff
        Gtk::ScrolledWindow        _myScrolledWindow;
        acgtk::TNTMeasurementList  _myListView;

};

int main(int argc, char * argv[]) {

    Gtk::Main myApp(argc, argv);

    TWindow myWindow;
    Gtk::Main::run(myWindow);
}
#endif
