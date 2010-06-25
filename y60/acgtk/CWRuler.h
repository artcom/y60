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

#ifndef ACGTK_CW_RULER_INCLUDED
#define ACGTK_CW_RULER_INCLUDED

#include "y60_acgtk_settings.h"

#include <asl/math/Vector234.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
    //#pragma warning(disable:4413 4244 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace acgtk {

class Y60_ACGTK_DECL CWRuler : public Gtk::DrawingArea {
    public:
        enum Mode {
            MODE_THRESHOLD, // [DS] ... think this is deprecated
            MODE_CENTER_WIDTH,
            MODE_LOWER_UPPER
        };
        CWRuler(Mode theMode = MODE_CENTER_WIDTH);
        virtual ~CWRuler();

        void setValueRange(const asl::Vector2f & theValueRange);
        const asl::Vector2f & getValueRange() const;

        void setWindowCenter(float theCenter);
        float getWindowCenter() const;

        void setWindowWidth(float theWidth);
        float getWindowWidth() const;

        void setLower(float theLower);
        float getLower() const;

        void setUpper(float theUpper);
        float getUpper() const;

        void setMode(Mode theMode);
        Mode getMode() const;

        sigc::signal<void, float> signal_center_changed() const { return _myCenterChangedSignal;}
        sigc::signal<void, float> signal_width_changed() const { return _myWidthChangedSignal;}
        sigc::signal<void, float> signal_lower_changed() const { return _myLowerChangedSignal;}
        sigc::signal<void, float> signal_upper_changed() const { return _myUpperChangedSignal;}

    protected:
        virtual bool on_expose_event(GdkEventExpose * theEvent);
        virtual bool on_button_press_event(GdkEventButton * theEvent);
        virtual bool on_button_release_event(GdkEventButton * theEvent);
        virtual bool on_motion_notify_event(GdkEventMotion * theEvent);
        void on_realize();
    private:
        void drawMarker(float thePosition, Glib::RefPtr<Gdk::GC> theGC);
        int convertValueToScreenPos(const float & theValue);
        float convertScreenPosToValue(const int & theScreenPos);
        bool intersectWithMarker(GdkEventButton * theEvent, int theMarkerPos);

        enum State {
            IDLE,
            CHANGE_CENTER,
            CHANGE_WIDTH_LEFT,
            CHANGE_WIDTH_RIGHT
        };

        Mode          _myMode;
        State         _myState;
        asl::Vector2f _myValueRange;
        float        _myWindowCenter;
        float        _myWindowWidth;
        float        _myLower;
        float        _myUpper;

        Glib::RefPtr<Gdk::Window>  _myWindow;
        sigc::signal<void, float> _myCenterChangedSignal;
        sigc::signal<void, float> _myWidthChangedSignal;
        sigc::signal<void, float> _myLowerChangedSignal;
        sigc::signal<void, float> _myUpperChangedSignal;
};

}


#endif // ACGTK_CW_RULER_INCLUDED
