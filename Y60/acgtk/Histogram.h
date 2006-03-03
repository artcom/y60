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

#ifndef ACGTK_HISTOGRAM_INCLUDED
#define ACGTK_HISTOGRAM_INCLUDED

#include <asl/Vector234.h>

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>

namespace acgtk {

class Histogram : public Gtk::DrawingArea {
    public:
        Histogram();
        virtual ~Histogram();

        enum Mode {
            MODE_CENTER_WIDTH,
            MODE_LOWER_UPPER
        };

        void setHistogram(const std::vector<unsigned> & theSamples);

        void setShowWindow(bool theFlag);
        bool getShowWindow() const;

        void setShowWindowCenter(bool theFlag);
        bool getShowWindowCenter() const;

        void setWindowCenter(float theValue);
        void setWindowWidth(float theValue);

        void setLower(float theValue);
        void setUpper(float theValue);

        void setValueRange(const asl::Vector2f & theRange);
        const asl::Vector2f & getValueRange() const;

        void setLogarithmicScale(bool theFlag);
        bool getLogarithmicScale() const;

        void setMode(Mode theMode) {
            _myMode = theMode;
        }
        Mode getMode() const {
            return _myMode;
        }

    protected:
        virtual bool on_configure_event(GdkEventConfigure * theEvent);
        virtual bool on_expose_event(GdkEventExpose * theEvent);
        /*
        virtual bool on_button_press_event(GdkEventButton * theEvent);
        virtual bool on_button_release_event(GdkEventButton * theEvent);
        virtual bool on_motion_notify_event(GdkEventMotion * theEvent);
        */
        void on_realize();
        void on_size_request(Gtk::Requisition* requisition);
    private:
        unsigned findMaxCount();
        int convertSampleCountToScreenPos(int theSampleCount, int theMaxSampleCount);
        int convertValueToScreenPos(const float & theValue);
        void rebuildBins();
        
        Mode                      _myMode;
        Glib::RefPtr<Gdk::Window> _myWindow;
        std::vector<unsigned>     _mySampleData;
        std::vector<unsigned>     _myBins;
        bool                      _myLogarithmicScaleFlag;
        unsigned                  _myMaxCount;

        float                    _myWindowCenter;
        float                    _myWindowWidth;
        float                    _myLower;
        float                    _myUpper;
        bool                     _myDrawCenterFlag;
        bool                     _myDrawWindowFlag;

        asl::Vector2f            _myValueRange;
};

}

#endif // ACGTK_HISTOGRAM_INCLUDED
