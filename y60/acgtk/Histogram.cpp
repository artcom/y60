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
#include "Histogram.h"

#include <iostream>
#include <asl/base/Logger.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/pldebug.h>
#include <paintlib/Filter/pl2passscale.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;

namespace acgtk {


class CData_UnsignedInt
{
public:
  typedef unsigned int _DataType[1];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val = 0;
      };
      void Accumulate (long long Weight, _DataType &value)
      {
        val += (Weight * (long long)(value[0]));
      };
      void Store (_DataType &value)
      {
        value [0] = (unsigned int) ((val + 128)/256);
      };
      long long val;
  };
};

const static float MIN_WINDOW_WIDTH(10.0);

Histogram::Histogram() :
    Gtk::DrawingArea(),
    _myMode(MODE_CENTER_WIDTH),
    _myLogarithmicScaleFlag(true),
    _myDrawCenterFlag(false),
    _myDrawWindowFlag(false)
{
    set_size_request(100, 50);
    Gdk::EventMask myFlags = get_events();
    myFlags |= Gdk::POINTER_MOTION_MASK;
    myFlags |= Gdk::BUTTON_PRESS_MASK;
    myFlags |= Gdk::BUTTON_RELEASE_MASK;
    set_events(myFlags);
}

Histogram::~Histogram() {
}


void
Histogram::on_realize() {
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  _myWindow = get_window();
  _myWindow->clear();
}

/*
bool
Histogram::on_button_press_event(GdkEventButton * theEvent) {
    //cerr << "Histogram::on_button_press_event()" << endl;
    _myLogarithmicScaleFlag = ! _myLogarithmicScaleFlag;
    queue_draw();
    return true;
}
bool
Histogram::on_button_release_event(GdkEventButton * theEvent) {
    //cerr << "Histogram::on_button_release_event()" << endl;
    return true;
}

bool
Histogram::on_motion_notify_event(GdkEventMotion * theEvent) {
    return true;
}
*/

void
Histogram::on_size_request(Gtk::Requisition* requisition)
{
  *requisition = Gtk::Requisition();
  // hack: since paintlib's resize filters have a problem with
  // minification higher than 1:256, set the minimum
  // width so that this rate will never be reached even
  // bit full 16-bit ranges
  requisition->width = 257;
}

bool
Histogram::on_configure_event(GdkEventConfigure * theEvent) {
    //cerr << "Histogram::on_configure_event()" << endl;
    DrawingArea::on_configure_event(theEvent);
    rebuildBins();
    return true;
}

bool
Histogram::on_expose_event(GdkEventExpose * theEvent) {
    //cerr << "Histogram::on_expose_event()" << endl;
    DrawingArea::on_expose_event(theEvent);

    _myWindow->clear();

    int myWidth    = get_allocation().get_width();
    int myHeight   = get_allocation().get_height();
    int myBinCount = _myBins.size();

    // TODO: handle myBarWidth < 1.0
    float myBarWidth = float(myWidth) / myBinCount;

    Glib::RefPtr<Gdk::GC> myGC = get_style()->get_black_gc();

    unsigned myCount;
    // AC_WARNING << "drawing " << _myBins.size() << " bins, value range is " << _myValueRange;
    for (unsigned i = 0; i < _myBins.size(); ++i) {
        myCount = convertSampleCountToScreenPos(_myBins[i], _myMaxCount);
        _myWindow->draw_rectangle(myGC, true, int(i * myBarWidth), myHeight - myCount,
                int(ceilf(myBarWidth)), myCount);
    }

    if (_myDrawWindowFlag) {
        int myXStart;
        int myXEnd;
        if (_myMode == MODE_CENTER_WIDTH) {
            myXStart = convertValueToScreenPos(_myWindowCenter - static_cast<float>(0.5 * _myWindowWidth));
            myXEnd   = convertValueToScreenPos(_myWindowCenter + static_cast<float>(0.5 * _myWindowWidth));
        } else {
            myXStart = convertValueToScreenPos(_myLower);
            myXEnd   = convertValueToScreenPos(_myUpper);
        }

        myGC = get_style()->get_dark_gc(get_state()),
             myGC->set_function(Gdk::XOR);
        _myWindow->draw_rectangle(myGC, true, myXStart, 0, myXEnd - myXStart, myHeight);
        myGC->set_function(Gdk::COPY);
    }

    if (_myDrawCenterFlag && _myMode == MODE_CENTER_WIDTH) {
        int myXPos = convertValueToScreenPos(_myWindowCenter);
        myGC = get_style()->get_white_gc();
        myGC->set_function(Gdk::XOR);
        _myWindow->draw_line(myGC, myXPos, 0, myXPos, myHeight);
        myGC->set_function(Gdk::COPY);
    }

    return true;
}

void
Histogram::rebuildBins() {
    std::vector<unsigned>::size_type myWidth = get_allocation().get_width();
    if (myWidth < _mySampleData.size()) {
        _myBins.clear();
        _myBins.resize(myWidth);

        // set up some pointers to simulate paintlib's linearray
        unsigned int * mySrcLinePtr = (&_mySampleData[0]);
        unsigned int * myDestLinePtr = (&_myBins[0]);
        // PLGaussianContribDef f;
        PLBilinearContribDef f(2.0);
        C2PassScale <CData_UnsignedInt> sS(f);
        // and start it
        sS.Scale( (CData_UnsignedInt::_RowType*)(&mySrcLinePtr), _mySampleData.size(), 1,
                  (CData_UnsignedInt::_RowType*)(&myDestLinePtr), _myBins.size(), 1);
    } else {
        _myBins = _mySampleData;
    }
    _myMaxCount = findMaxCount();
}

unsigned
Histogram::findMaxCount() {
    unsigned myMax = 0;
    for (unsigned i = 0; i < _myBins.size(); ++i) {
        if (_myBins[i] > myMax) {
            myMax = _myBins[i];
        }
    }
    return int( 1.1 * myMax); // make 10% headroom
}

int
Histogram::convertSampleCountToScreenPos(int theSampleCount, int theMaxSampleCount) {
    int myHeight   = get_allocation().get_height();
    if (theMaxSampleCount == 0) {
        return myHeight / 2;
    }
    if (_myLogarithmicScaleFlag) {
        return int( log( float(theSampleCount)) * myHeight / log(float(theMaxSampleCount)));
    } else {
        return theSampleCount * myHeight / theMaxSampleCount;
    }
}

void
Histogram::setHistogram(const std::vector<unsigned> & theSamples) {
    _mySampleData = theSamples;
    rebuildBins();
    queue_draw();
}
void
Histogram::setShowWindow(bool theFlag) {
    _myDrawWindowFlag = theFlag;
    queue_draw();
}

bool
Histogram::getShowWindow() const {
    return _myDrawWindowFlag;
}

void
Histogram::setShowWindowCenter(bool theFlag) {
    _myDrawCenterFlag = theFlag;
    queue_draw();
}

bool
Histogram::getShowWindowCenter() const {
    return _myDrawCenterFlag;
}

void
Histogram::setWindowCenter(float theValue) {
    _myWindowCenter = theValue;
    queue_draw();
}

void
Histogram::setWindowWidth(float theValue) {
    _myWindowWidth = theValue;
    queue_draw();
}

void
Histogram::setLower(float theValue) {
    _myLower = theValue;
    queue_draw();
}

void
Histogram::setUpper(float theValue) {
    _myUpper = theValue;
    queue_draw();
}

void
Histogram::setValueRange(const asl::Vector2f & theRange) {
    _myValueRange = theRange;
}

const asl::Vector2f &
Histogram::getValueRange() const {
    return _myValueRange;
}

void
Histogram::setLogarithmicScale(bool theFlag) {
    _myLogarithmicScaleFlag = theFlag;
    queue_draw();
}

bool
Histogram::getLogarithmicScale() const {
    return _myLogarithmicScaleFlag;
}

int
Histogram::convertValueToScreenPos(const float & theValue) {
    int myWidth = get_allocation().get_width();
    return int((theValue - _myValueRange[0])* float(myWidth) / (_myValueRange[1] - _myValueRange[0]));
}


} // end of namespace
