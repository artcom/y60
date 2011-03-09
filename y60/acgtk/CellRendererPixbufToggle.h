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

#ifndef ACGTK_CELL_RENDERER_PIXBUF_TOGGLE_INCLUDED
#define ACGTK_CELL_RENDERER_PIXBUF_TOGGLE_INCLUDED

#include "y60_acgtk_settings.h"

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/cellrenderer.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4250 4275)
#endif //defined(_MSC_VER)

namespace acgtk {

class Y60_ACGTK_DECL CellRendererPixbufToggle : public Gtk::CellRenderer {
    public:
        CellRendererPixbufToggle();
        virtual ~CellRendererPixbufToggle();

        Glib::PropertyProxy<bool> property_activatable();
        Glib::PropertyProxy<bool> property_active();
        Glib::PropertyProxy<Glib::RefPtr<Gdk::Pixbuf> > property_active_icon();
        Glib::PropertyProxy<Glib::RefPtr<Gdk::Pixbuf> > property_inactive_icon();

        typedef sigc::signal<void, const Glib::ustring&> SignalToggled;
        SignalToggled& signal_toggled();

    protected:
        virtual void get_size_vfunc(Gtk::Widget& widget,
                const Gdk::Rectangle* cell_area,
                int* x_offset, int* y_offset,
                int* width,    int* height) const;

        virtual void render_vfunc(const Glib::RefPtr<Gdk::Drawable>& window,
                Gtk::Widget& widget,
                const Gdk::Rectangle& background_area,
                const Gdk::Rectangle& cell_area,
                const Gdk::Rectangle& expose_area,
                Gtk::CellRendererState flags);

        virtual bool activate_vfunc(GdkEvent* event,
                Gtk::Widget& widget,
                const Glib::ustring& path,
                const Gdk::Rectangle& background_area,
                const Gdk::Rectangle& cell_area,
                Gtk::CellRendererState flags);

    private:
        Glib::Property<bool> property_activatable_;
        Glib::Property<bool> property_active_;
        Glib::Property<Glib::RefPtr<Gdk::Pixbuf> > property_active_icon_;
        Glib::Property<Glib::RefPtr<Gdk::Pixbuf> > property_inactive_icon_;

        SignalToggled signal_toggled_;
};

} // end of namespace

#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#endif // ACGTK_CELL_RENDERER_PIXBUF_TOGGLE_INCLUDED
