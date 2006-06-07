//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef ACGTK_GRADIENT_SLIDER_INCLUDED
#define ACGTK_GRADIENT_SLIDER_INCLUDED

#include <asl/Vector234.h>
#include <dom/Nodes.h>

#ifdef OSX
#undef check
#undef nil
#endif

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>

namespace acgtk {

class GradientSlider : public Gtk::DrawingArea {
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
