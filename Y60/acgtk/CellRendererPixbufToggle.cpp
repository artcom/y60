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

#include "CellRendererPixbufToggle.h"

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)
#include <sstream>


namespace acgtk {

CellRendererPixbufToggle::CellRendererPixbufToggle() :
    Glib::ObjectBase      (typeid(CellRendererPixbufToggle)),
    Gtk::CellRenderer     (),
    property_activatable_ (*this, "activatable", true),
    property_active_      (*this, "active",      false),
    property_active_icon_  (*this, "active_icon"),
    property_inactive_icon_(*this, "inactive_icon")
{
    property_mode() = Gtk::CELL_RENDERER_MODE_ACTIVATABLE;
    property_xpad() = 2;
    property_ypad() = 2;
}

CellRendererPixbufToggle::~CellRendererPixbufToggle()
{}

Glib::PropertyProxy<bool>
CellRendererPixbufToggle::property_activatable() {
    return property_activatable_.get_proxy();
}

Glib::PropertyProxy<bool>
CellRendererPixbufToggle::property_active() {
    return property_active_.get_proxy();
}

Glib::PropertyProxy<Glib::RefPtr<Gdk::Pixbuf> >
CellRendererPixbufToggle::property_active_icon() {
    return property_active_icon_.get_proxy();
}

Glib::PropertyProxy<Glib::RefPtr<Gdk::Pixbuf> >
CellRendererPixbufToggle::property_inactive_icon() {
    return property_inactive_icon_.get_proxy();
}

CellRendererPixbufToggle::SignalToggled &
CellRendererPixbufToggle::signal_toggled() {
    return signal_toggled_;
}

void
CellRendererPixbufToggle::get_size_vfunc(Gtk::Widget&,
        const Gdk::Rectangle* cell_area,
        int* x_offset, int* y_offset,
        int* width,    int* height) const
{
    Glib::RefPtr<Gdk::Pixbuf> myCurrentIcon;
    if (property_active_) {
        myCurrentIcon = property_active_icon_;
    } else {
        myCurrentIcon = property_inactive_icon_;
    }
    int myIconWidth = 0;
    int myIconHeight = 0;
    if (myCurrentIcon) {
        myIconWidth = myCurrentIcon->get_width();     
        myIconHeight = myCurrentIcon->get_height();     
    }

    const int calc_width  = property_xpad() * 2 + myIconWidth;
    const int calc_height = property_ypad() * 2 + myIconHeight;

    if(width)
        *width = calc_width;

    if(height)
        *height = calc_height;

    if(cell_area)
    {
        if(x_offset)
        {
            *x_offset = int(property_xalign() * (cell_area->get_width() - calc_width));
            *x_offset = std::max(0, *x_offset);
        }

        if(y_offset)
        {
            *y_offset = int(property_yalign() * (cell_area->get_height() - calc_height));
            *y_offset = std::max(0, *y_offset);
        }
    }
}

    void
CellRendererPixbufToggle::render_vfunc(const Glib::RefPtr<Gdk::Drawable>& window,
        Gtk::Widget& widget,
        const Gdk::Rectangle&,
        const Gdk::Rectangle& cell_area,
        const Gdk::Rectangle&,
        Gtk::CellRendererState /*flags*/)
{
    const unsigned int cell_xpad = property_xpad();
    const unsigned int cell_ypad = property_ypad();

    int x_offset = 0, y_offset = 0, width = 0, height = 0;
    get_size(widget, cell_area, x_offset, y_offset, width, height);

    width  -= cell_xpad * 2;
    height -= cell_ypad * 2;

    if(width <= 0 || height <= 0)
        return;

    Glib::RefPtr<Gdk::Window> window_casted = Glib::RefPtr<Gdk::Window>::cast_dynamic<>(window);
    if(window_casted)
    {
        Glib::RefPtr<Gdk::Pixbuf> myIcon = (property_active() ?
                property_active_icon() : property_inactive_icon());
        if (myIcon) {
            window_casted->draw_pixbuf(Glib::RefPtr<Gdk::GC>(0), myIcon, 0, 0,
                                       cell_area.get_x() + x_offset + cell_xpad,
                                       cell_area.get_y() + y_offset + cell_ypad,
                                       -1, -1, Gdk::RGB_DITHER_NORMAL, 0, 0);
        }
    }
}

bool
CellRendererPixbufToggle::activate_vfunc(GdkEvent*,
        Gtk::Widget&,
        const Glib::ustring& path,
        const Gdk::Rectangle&,
        const Gdk::Rectangle&,
        Gtk::CellRendererState)
{
    if(property_activatable_)
    {
        signal_toggled_(path);
        return true;
    }

    return false;
}

} // end of namespace

