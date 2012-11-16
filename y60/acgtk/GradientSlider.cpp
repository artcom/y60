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
#include "GradientSlider.h"

using namespace std;

namespace acgtk {

const static int MIN_WINDOW_WIDTH(10);

GradientSlider::GradientSlider() :
    Gtk::DrawingArea(),
    _myTransferFunction(),
    _mySelectedColor(),
    _mySelectionCandidate()
{
    set_size_request(256, 10);
    Gdk::EventMask myFlags = get_events();
    myFlags |= Gdk::POINTER_MOTION_MASK;
    myFlags |= Gdk::BUTTON_PRESS_MASK;
    myFlags |= Gdk::BUTTON_RELEASE_MASK;
    set_events(myFlags);
}

GradientSlider::~GradientSlider() {
}

bool
GradientSlider::on_expose_event(GdkEventExpose * theEvent) {
    DrawingArea::on_expose_event(theEvent);
    _myWindow->clear();
    if (_myTransferFunction) {
        int mySelectedCenter = 0;
        int mySelectedWidth = 0;
        if (_mySelectedColor) {
            mySelectedCenter = _mySelectedColor->getAttributeValue<int>("value");
            mySelectedWidth = _mySelectedColor->getAttributeValue<int>("width");
            int myXStart = convertValueToScreenPos(float(mySelectedCenter) - 0.25f * float(mySelectedWidth));
            int myXEnd = convertValueToScreenPos(float(mySelectedCenter) + 0.25f * float(mySelectedWidth));

            _myWindow->draw_rectangle(get_style()->get_dark_gc(get_state()), true, myXStart, 0,
                    myXEnd - myXStart, get_allocation().get_height());

        }
        int myCenter;
        for (unsigned i = 0; i < _myTransferFunction->childNodesLength(); ++i) {
            if (_myTransferFunction->childNode(i) != _mySelectedColor) {
                myCenter = _myTransferFunction->childNode(i)->getAttributeValue<int>("value");
                drawMarker(float(myCenter), get_style()->get_black_gc());
            }
        }
        if (_mySelectedColor) {
            drawMarker(float(mySelectedCenter), get_style()->get_black_gc(), get_style()->get_white_gc());
            drawMarker(float(mySelectedCenter) + 0.25f * float(mySelectedWidth), get_style()->get_white_gc(),
                    get_style()->get_black_gc());
            drawMarker(float(mySelectedCenter) - 0.25f * float(mySelectedWidth), get_style()->get_white_gc(),
                    get_style()->get_black_gc());
        }
    }
    return true;
}

void
GradientSlider::drawMarker(float thePosition, Glib::RefPtr<Gdk::GC> theBackgroundGC,
                    Glib::RefPtr<Gdk::GC> theOutlineGC )
{

    int myHeight = get_allocation().get_height();
    int myXPos = convertValueToScreenPos(thePosition);
    std::vector<Gdk::Point> myPoints;

    int myTriangleSize = myHeight;
    myPoints.push_back(Gdk::Point(myXPos, 0));
    myPoints.push_back(Gdk::Point(myXPos + int(0.5 * myTriangleSize), myTriangleSize ));
    myPoints.push_back(Gdk::Point(myXPos - int(0.5 * myTriangleSize), myTriangleSize ));
    _myWindow->draw_polygon(theBackgroundGC, true, myPoints);
    if (theOutlineGC) {
        _myWindow->draw_polygon(theOutlineGC, false, myPoints);
    }
}

bool
GradientSlider::on_button_press_event(GdkEventButton * theEvent) {
    if (_mySelectedColor) {
        int myCenter = _mySelectedColor->getAttributeValue<int>("value");
        int myWidth = _mySelectedColor->getAttributeValue<int>("width");
        int myCenterMarkerPos = convertValueToScreenPos(float(myCenter));
        int myLeftMarkerPos = convertValueToScreenPos(float(myCenter) -  0.25f * float(myWidth));
        int myRightMarkerPos = convertValueToScreenPos(float(myCenter) + 0.25f * float(myWidth));
        if (intersectWithMarker(theEvent, myLeftMarkerPos)) {
            _myState = CHANGE_WIDTH_LEFT;
        } else if (intersectWithMarker(theEvent, myRightMarkerPos)) {
            _myState = CHANGE_WIDTH_RIGHT;
        } else if (intersectWithMarker(theEvent, myCenterMarkerPos)) {
            _myState = CHANGE_CENTER;
        }
    }

    if (_myState == IDLE) { // nothing hit yet
        for (unsigned i = 0; i < _myTransferFunction->childNodesLength("color"); ++i) {
            if (_myTransferFunction->childNode(i) != _mySelectedColor) {
                int myCenter = _myTransferFunction->childNode(i)->getAttributeValue<int>("value");
                if (intersectWithMarker(theEvent, convertValueToScreenPos(float(myCenter)))) {
                    _myState = CHANGE_SELECTION;
                    _mySelectionCandidate = _myTransferFunction->childNode(i);
                }
            }
        }
    }
    return true;
}

bool
GradientSlider::on_button_release_event(GdkEventButton * theEvent) {
    if (_myState == CHANGE_SELECTION && _mySelectionCandidate) {
        int myCenter = _mySelectionCandidate->getAttributeValue<int>("value");
        if (intersectWithMarker(theEvent, convertValueToScreenPos(float(myCenter)))) {
            _mySelectedColor = _mySelectionCandidate;
            _mySelectionChangedSignal.emit();
            queue_draw();
        }

        _mySelectionCandidate = dom::NodePtr();
    }
    _myState = IDLE;
    return true;
}

bool
GradientSlider::on_motion_notify_event(GdkEventMotion * theEvent) {
    if (_mySelectedColor) {
        int myValue = int( convertScreenPosToValue(int(theEvent->x)));
        int myCenter = _mySelectedColor->getAttributeValue<int>("value");
        int myWidth = _mySelectedColor->getAttributeValue<int>("width");
        switch (_myState) {
            case CHANGE_CENTER:
                {
                    myValue = min(max(myValue, int(_myValueRange[0])), int(_myValueRange[1]));
                    if (int(myValue) != myCenter) {
                        _mySelectedColor->getAttribute("value")->nodeValueAssign<int>(int(myValue));
                        _myValuesChangedSignal.emit();
                        queue_draw();
                    }
                }
                break;
            case CHANGE_WIDTH_LEFT:
                {
                    myValue = max(min(4 * (myCenter - myValue),
                                4 * (myCenter - int(_myValueRange[0]))), MIN_WINDOW_WIDTH);
                    if (myValue != myWidth) {
                        _mySelectedColor->getAttribute("width")->nodeValueAssign<int>(int(myValue));
                        _myValuesChangedSignal.emit();
                        queue_draw();
                    }
                }
                break;
            case CHANGE_WIDTH_RIGHT:
                {
                    myValue = max(min(4 * (myValue - myCenter),
                                4 * (int(_myValueRange[1]) - myCenter)), MIN_WINDOW_WIDTH);
                    if (myValue != myWidth) {
                        _mySelectedColor->getAttribute("width")->nodeValueAssign<int>(int(myValue));
                        _myValuesChangedSignal.emit();
                        queue_draw();
                    }
                }
                break;
            case IDLE:
            case CHANGE_SELECTION:
            default:
                break;
        }
    }
    return true;
}

void
GradientSlider::on_realize() {
    Gtk::DrawingArea::on_realize();
    _myWindow = get_window();
    _myWindow->clear();
}

void
GradientSlider::setSelectedColor(dom::NodePtr theColor) {
    if ( theColor != _mySelectedColor) {
        _mySelectedColor = theColor;
        _mySelectionChangedSignal.emit();
    }
    queue_draw();
}

dom::NodePtr
GradientSlider::getSelectedColor() const {
    return _mySelectedColor;
}

void
GradientSlider::setTransferFunction(dom::NodePtr theTransferFunction) {
    _myTransferFunction = theTransferFunction;
    setSelectedColor(dom::NodePtr());
    queue_draw();
}

dom::NodePtr
GradientSlider::getTransferFunction() const {
    return _myTransferFunction;
}

void
GradientSlider::setValueRange(const asl::Vector2f & theValueRange) {
    _myValueRange = theValueRange;
    queue_draw();
}

const asl::Vector2f &
GradientSlider::getValueRange() const {
    return _myValueRange;
}

int
GradientSlider::convertValueToScreenPos(const float & theValue) {
    int myWidth = get_allocation().get_width();
    return int((theValue - _myValueRange[0])* float(myWidth) / (_myValueRange[1] - _myValueRange[0]));
}

float
GradientSlider::convertScreenPosToValue(const int & theScreenPos) {
    int myWidth = get_allocation().get_width();
    return (theScreenPos * (_myValueRange[1] - _myValueRange[0]) / myWidth) + _myValueRange[0];
}

bool
GradientSlider::intersectWithMarker(GdkEventButton * theEvent, int theMarkerPos) {
    float myHTriangleBase = 0.5f * get_allocation().get_height();
    if (theEvent->x > (theMarkerPos - myHTriangleBase) &&
        theEvent->x < (theMarkerPos + myHTriangleBase))
    {
        return true;
    }
    return false;
}


} // end of namespace
