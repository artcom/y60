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
#include "GrayScale.h"

#include <iostream>

using namespace std;

namespace acgtk {


GrayScale::GrayScale() :
    Gtk::DrawingArea()
{
    set_size_request(256, 10);
}

GrayScale::~GrayScale() {
}


void
GrayScale::on_realize() {
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  _myWindow = get_window();
  _myGC = Gdk::GC::create(_myWindow);
  _myWindow->clear();
}

bool
GrayScale::on_expose_event(GdkEventExpose * theEvent) {
    //cerr << "GrayScale::on_expose_event()" << endl;
    DrawingArea::on_expose_event(theEvent);

    _myWindow->clear();

    int myWidth = get_allocation().get_width();
    int myHeight = get_allocation().get_height();
    Gdk::Color myColor;
    float myBarWidth = float(myWidth) / 256;
    for (unsigned i = 0; i < 256; ++i) {
        gushort myValue = gushort((float(i) / 256) * 65536);
        myColor.set_red(myValue);
        myColor.set_green(myValue);
        myColor.set_blue(myValue);
        _myGC->set_rgb_fg_color(myColor);
        //_myWindow->draw_line(_myGC, i, 0, i, myHeight);
        _myWindow->draw_rectangle(_myGC, true, int(i * myBarWidth), 0, int(ceil(myBarWidth)), myHeight);
    }

    return true;
}

}

