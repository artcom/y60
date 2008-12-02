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

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4413 4244 4127 4512)
#endif //defined(_MSC_VER)
#include <gtkmm/cellrenderer.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace acgtk {

class CellRendererPixbufToggle : public Gtk::CellRenderer {
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
