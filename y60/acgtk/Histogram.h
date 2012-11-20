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
//
//   $RCSfile: SliceViewer.js,v $
//   $Author: danielk $
//   $Revision: 1.97 $
//   $Date: 2005/04/29 13:49:59 $
//
//=============================================================================

#ifndef ACGTK_HISTOGRAM_INCLUDED
#define ACGTK_HISTOGRAM_INCLUDED

#include "y60_acgtk_settings.h"

#include <asl/math/Vector234.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/drawingarea.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <sigc++/sigc++.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4250 4275)
#endif //defined(_MSC_VER)

namespace acgtk {

class Y60_ACGTK_DECL Histogram : public Gtk::DrawingArea {
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
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#endif // ACGTK_HISTOGRAM_INCLUDED
