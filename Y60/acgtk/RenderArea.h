/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.13 $
//
// Description: Test for Request
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _tnt_renderarea_included_
#define _tnt_renderarea_included_

#include <gtkmm.h>

#include "GtkEventAdapter.h"

#include <y60/AbstractRenderWindow.h>
#include <gdk/gdkgltypes.h>
#include <string>
#include <sigc++/connection.h>

namespace acgtk {

class RenderArea;
typedef asl::Ptr<RenderArea> RenderAreaPtr;

class RenderArea : public Gtk::DrawingArea, public jslib::AbstractRenderWindow {
    public:
        RenderArea(RenderAreaPtr theContext = RenderAreaPtr(0));
        virtual ~RenderArea();
        // Y60 AbstractWindowArea
        void initDisplay();
        virtual void activateGLContext();
        virtual void deactivateGLContext();
        y60::TTFTextRendererPtr createTTFRenderer();
        int getWidth() const;
        int getHeight() const;
        void setIdleMode(bool theEnabledFlag);
        GdkGLContext * getGdkGlContext() const;

        virtual void swapBuffers();
    
    protected:
        // Gtk callbacks
        bool on_expose_event (GdkEventExpose *event);
        bool on_configure_event(GdkEventConfigure *event);
        void on_realize();
        bool on_idle();
        bool on_map_event (GdkEventAny *event);
        bool on_unmap_event (GdkEventAny *event);
        bool on_visibility_notify_event (GdkEventVisibility *event);
        bool on_motion_notify_event(GdkEventMotion *event);
        bool on_scroll_event (GdkEventScroll* event);
        bool on_button_press_event(GdkEventButton *event);
        bool on_button_release_event(GdkEventButton *event);

        void handle(y60::EventPtr theEvent);
    private:
        void idle_add();
        void idle_remove();
        //Glib::RefPtr<Gnome::Glade::Xml> _myGladeXml;
        sigc::connection _myIdleId;
        int _myContextRefCount;
        GtkEventAdapter _myEventAdapter;
        bool _isFirstFrame;
};

}


#endif

