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

#ifndef ACGTK_GRAY_SCALE_INCLUDED
#define ACGTK_GRAY_SCALE_INCLUDED

#include <asl/Vector234.h>

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>

namespace acgtk {

#undef GrayScale

class GrayScale : public Gtk::DrawingArea {
    public:
        GrayScale();
        virtual ~GrayScale();

    protected:
        virtual bool on_expose_event(GdkEventExpose * theEvent);
        void on_realize();
    private:
        Glib::RefPtr<Gdk::GC> _myGC;
        Glib::RefPtr<Gdk::Window> _myWindow;
};

}


#endif // ACGTK_GRAY_SCALE_INCLUDED
