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

#ifndef ACGTK_GRADIENT_SLIDER_INCLUDED
#define ACGTK_GRADIENT_SLIDER_INCLUDED

#include "y60_acgtk_settings.h"

#include <asl/math/Vector234.h>
#include <asl/dom/Nodes.h>

#ifdef OSX
#undef check
#undef nil
#endif

#if defined(_MSC_VER)
    #pragma warning(push,1)
    //#pragma warning(disable:4413 4244 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/drawingarea.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <sigc++/sigc++.h>

namespace acgtk {

class Y60_ACGTK_DECL GradientSlider : public Gtk::DrawingArea {
    public:
        GradientSlider();
        virtual ~GradientSlider();

        void setValueRange(const asl::Vector2f & theValueRange);
        const asl::Vector2f & getValueRange() const;

        void setTransferFunction(dom::NodePtr theTransferFunction);
        dom::NodePtr getTransferFunction() const;

        void setSelectedColor(dom::NodePtr theColor);
        dom::NodePtr getSelectedColor() const;

        sigc::signal<void> getSignalSelectionChanged() const { return _mySelectionChangedSignal; }
        sigc::signal<void> getSignalValuesChanged() const { return _myValuesChangedSignal; }

    protected:
        virtual bool on_expose_event(GdkEventExpose * theEvent);
        virtual bool on_button_press_event(GdkEventButton * theEvent);
        virtual bool on_button_release_event(GdkEventButton * theEvent);
        virtual bool on_motion_notify_event(GdkEventMotion * theEvent);
        void on_realize();

    private:
        void drawMarker(float thePosition, Glib::RefPtr<Gdk::GC> theBackgroundGC,
                Glib::RefPtr<Gdk::GC> theOutlineGC = Glib::RefPtr<Gdk::GC>(0));
        int convertValueToScreenPos(const float & theValue);
        float convertScreenPosToValue(const int & theScreenPos);
        bool intersectWithMarker(GdkEventButton * theEvent, int theMarkerPos);

        enum State {
            IDLE,
            CHANGE_CENTER,
            CHANGE_WIDTH_LEFT,
            CHANGE_WIDTH_RIGHT,
            CHANGE_SELECTION
        };

        State         _myState;
        asl::Vector2f _myValueRange;

        Glib::RefPtr<Gdk::Window>  _myWindow;

        dom::NodePtr _myTransferFunction;
        dom::NodePtr _mySelectedColor;
        dom::NodePtr _mySelectionCandidate;

        sigc::signal<void> _mySelectionChangedSignal;
        sigc::signal<void> _myValuesChangedSignal;
};

} // end of namespace


#endif // ACGTK_GRADIENT_SLIDER_INCLUDED
