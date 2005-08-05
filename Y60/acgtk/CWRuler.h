//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: SliceViewer.js,v $
//   $Author: danielk $
//   $Revision: 1.97 $
//   $Date: 2005/04/29 13:49:59 $
//
//=============================================================================

#ifndef ACGTK_CW_RULER_INCLUDED
#define ACGTK_CW_RULER_INCLUDED

#include <asl/Vector234.h>

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>

namespace acgtk {

class CWRuler : public Gtk::DrawingArea {
    public:
        enum Mode {
            MODE_THRESHOLD,
            MODE_CENTER_WIDTH
        };
        CWRuler(Mode theMode = MODE_CENTER_WIDTH);
        virtual ~CWRuler();

        void setValueRange(const asl::Vector2f & theValueRange);
        const asl::Vector2f & getValueRange() const;

        void setWindowCenter(float theCenter);
        float getWindowCenter() const;

        void setWindowWidth(float theWidth);
        float getWindowWidth() const;

        void setMode(Mode theMode);
        Mode getMode() const;

        sigc::signal<void, float> signal_center_changed() const { return _myCenterChangedSignal;}
        sigc::signal<void, float> signal_width_changed() const { return _myWidthChangedSignal;}

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
        
        Glib::RefPtr<Gdk::Window>  _myWindow;
        sigc::signal<void, float> _myCenterChangedSignal;
        sigc::signal<void, float> _myWidthChangedSignal;
};

}


#endif // ACGTK_CW_RULER_INCLUDED
