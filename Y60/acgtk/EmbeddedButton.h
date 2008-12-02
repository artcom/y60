//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef ACGTK_EMBEDDED_BUTTON_INCLUDED
#define ACGTK_EMBEDDED_BUTTON_INCLUDED

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4413 4244 4512)
#endif //defined(_MSC_VER)
#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace acgtk {

class EmbeddedButton : public Gtk::EventBox {
    public:
        EmbeddedButton();
        EmbeddedButton(const std::string & theIconFile);

        sigc::signal<void> signal_clicked() const { return _myClickedSignal; }
        virtual void released();
        virtual void leave();
        virtual void pressed();

    protected:
        void setup();
        virtual bool on_leave_notify_event(GdkEventCrossing * theEvent);
        virtual bool on_button_press_event(GdkEventButton * theEvent);
        virtual bool on_button_release_event(GdkEventButton * theEvent);
        virtual void on_clicked();
        virtual void on_realize();

        Gtk::Image                _myImage;
        Glib::RefPtr<Gdk::Pixbuf> _myIcon;
    private:

        bool                      _myClickAheadFlag;
        sigc::signal<void>        _myClickedSignal;

};

};

#endif // ACGTK_EMBEDDED_BUTTON_INCLUDED



