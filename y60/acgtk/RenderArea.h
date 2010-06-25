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

#ifndef _tnt_renderarea_included_
#define _tnt_renderarea_included_

#include "y60_acgtk_settings.h"

#if defined(_MSC_VER)
    #pragma warning(push,1)
    #pragma warning(disable:4413 4244 4127 4512 4520 4251 4250)
#endif //defined(_MSC_VER)
#include <gtkmm.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)
#include "GtkEventAdapter.h"

#include <y60/jslib/AbstractRenderWindow.h>

#include <gdk/gdkgltypes.h>

#include <string>
#include <sigc++/connection.h>

namespace acgtk {

class RenderArea;
typedef asl::Ptr<RenderArea> RenderAreaPtr;

class Y60_ACGTK_DECL RenderArea : public Gtk::DrawingArea, public jslib::AbstractRenderWindow {
    public:
        RenderArea(RenderAreaPtr theContext = RenderAreaPtr());
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

