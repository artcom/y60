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

