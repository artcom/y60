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
#include "RenderArea.h"

#ifdef OSX
#include <y60/glutil/GLUtils.h>

#undef check
#undef notify
#undef verify
#undef nil
#undef DestroyNotify
#endif

#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>
#include <asl/base/Exception.h>
#include <y60/jslib/JSApp.h>
#include <y60/glutil/ScopedGLContext.h>
#include <y60/input/EventDispatcher.h>
#include <y60/glrender/GLResourceManager.h>
#include <y60/glutil/GLUtils.h>

//#include <glade/glade.h>
#if defined(_MSC_VER)
#pragma warning(push,1)
    //#pragma warning(disable:4413 4244 4512 4250)
#endif //defined(_MSC_VER)
#include <gtk/gtkgl.h>
#include <gtkmm/drawingarea.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <iostream>
#include <math.h>

#ifdef WIN32
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

#define DB(x)// x;

using namespace std;
using namespace asl;
using namespace y60;

namespace acgtk {

RenderArea::RenderArea(RenderAreaPtr theContext) : AbstractRenderWindow(jslib::JSApp::ShellErrorReporter),
    _myContextRefCount(0),
    _isFirstFrame(true)
{
    GdkGLConfig * myGLConfig = gdk_gl_config_new_by_mode(
                GdkGLConfigMode(GDK_GL_MODE_RGBA | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_ALPHA));
    if (myGLConfig == 0) {
        throw asl::Exception("can't init GL",PLUS_FILE_LINE);
    }

    // If another render area is supplied as constructor paramter, this render area is uses as
    // source for a shared y60-gl-context and gdk-gl-context.
    GdkGLContext * myGdkGLContext = 0;
    if (theContext) {
        myGdkGLContext = theContext->getGdkGlContext();
        if (!myGdkGLContext) {
            throw asl::Exception("RenderArea: Failed to get gdk GL context from shared render area.", PLUS_FILE_LINE);
        }
        setGLContext(theContext->getGLContext());
    } else {
        setGLContext(GLContextPtr(new GLContext()));
    }

    /* Set OpenGL-capability to the widget. */
    DB(cerr << "RenderArea::RenderArea() sharing with " << myGdkGLContext << endl);
    if (!gtk_widget_set_gl_capability (GTK_WIDGET(gobj()),
                myGLConfig,
                myGdkGLContext,
                true,
                GDK_GL_RGBA_TYPE))
    {
        throw asl::Exception("RenderArea: could not create GL context!", PLUS_FILE_LINE);
    }

    // enable mouse events
    Gdk::EventMask flags = get_events();
    flags |= Gdk::BUTTON_PRESS_MASK;
    flags |= Gdk::POINTER_MOTION_MASK;
    flags |= Gdk::BUTTON_RELEASE_MASK;
    flags |= Gdk::SCROLL_MASK;
    flags |= Gdk::ENTER_NOTIFY_MASK;
    flags |= Gdk::LEAVE_NOTIFY_MASK;
    set_events(flags);

    y60::EventDispatcher::get().addSource(&_myEventAdapter);
    y60::EventDispatcher::get().addSink(this);
    // TODO: createRenderer(theOtherRenderer);

    ShaderLibrary::setGLisReadyFlag(true);
}

RenderArea::~RenderArea() {
    y60::EventDispatcher::get().removeSource(&_myEventAdapter);
    y60::EventDispatcher::get().removeSink(this);
    idle_remove();
}

int
RenderArea::getWidth() const {
    return Gtk::DrawingArea::get_width();
};

int
RenderArea::getHeight() const {
    return Gtk::DrawingArea::get_height();
}

void
RenderArea::activateGLContext() {
    DB(cerr << "RenderArea::activateGLContext()" << endl);
    if (_myContextRefCount++ == 0) {
        GdkGLContext *glcontext = gtk_widget_get_gl_context (GTK_WIDGET(gobj()));
        GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (GTK_WIDGET(gobj()));
        gdk_gl_drawable_gl_begin (gldrawable, glcontext);

        // init glew
        unsigned int myGlewError = glewInit();
        if (GLEW_OK != myGlewError) {
            throw RendererException(std::string("Glew Initialization Error: ") +
                    std::string(reinterpret_cast<const char*>(glewGetErrorString(myGlewError))),
                    PLUS_FILE_LINE);
        }
        GLResourceManager::get().initCaps();
    }
}

void
RenderArea::deactivateGLContext() {
    DB(cerr << "RenderArea::deactivateGLContext()" << endl);
    if (--_myContextRefCount == 0) {
        GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (GTK_WIDGET(gobj()));
        gdk_gl_drawable_gl_end (gldrawable);
    }
}

GdkGLContext *
RenderArea::getGdkGlContext() const {
    DB(cerr << "RenderArea::getGdkGlContext()" << endl);
    return gtk_widget_get_gl_context (GTK_WIDGET(gobj()));
}

//virtual
void
RenderArea::swapBuffers() {
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (GTK_WIDGET(gobj()));
    if (gdk_gl_drawable_is_double_buffered (gldrawable)) {
        gdk_gl_drawable_swap_buffers (gldrawable);
    } else {
        glFlush ();
    }
}

bool
RenderArea::on_expose_event (GdkEventExpose *event) {
    try {
        /*GdkGLDrawable *gldrawable =*/ (void)gtk_widget_get_gl_drawable (GTK_WIDGET(gobj()));
        ScopedGLContext myContext(this);

        if (_myRenderer && _myScene) {
            if (_isFirstFrame) {
                _myRenderer->getCurrentScene()->updateAllModified();
                _isFirstFrame = false;
            }

            STOP_TIMER(frames);
            asl::getDashboard().cycle();
            START_TIMER(frames);

            onFrame();

            //START_TIMER(dispatchEvents);
            //y60::EventDispatcher::get().dispatch();
            //STOP_TIMER(dispatchEvents);

            START_TIMER(handleRequests);
            _myRequestManager.handleRequests();
            STOP_TIMER(handleRequests);

            renderFrame();
            swapBuffers();

            /** done rendering **/
        } else {
            // nothing to render... just clear the buffer to avoid pixel garbage
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            swapBuffers();
        }

        if (_myJSContext) {
            MAKE_SCOPE_TIMER(gc);
            JS_GC(_myJSContext);
        }
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

/* new window size or exposure */
bool
RenderArea::on_configure_event(GdkEventConfigure *event) {
    Gtk::DrawingArea::on_configure_event(event);
    try {
        _myEventAdapter.addWindowEvent(event);

        queue_draw();
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

void
RenderArea::on_realize() {
    try {
        DB(cerr << "on_realize called" << endl);
        queue_draw();
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    Gtk::DrawingArea::on_realize();
}

void
RenderArea::handle(y60::EventPtr theEvent) {
    if (theEvent->source == &_myEventAdapter) {
        AbstractRenderWindow::handle(theEvent);
    }
}

// ===================================================
//
//              Mouse Events
//
// ===================================================
bool
RenderArea::on_scroll_event (GdkEventScroll* event) {
    try {
        _myEventAdapter.addMouseScrollEvent(event);
        queue_draw();
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

bool
RenderArea::on_motion_notify_event(GdkEventMotion *event) {
    try {
        _myEventAdapter.addMouseMotionEvent(event);
        //queue_draw();
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

bool
RenderArea::on_button_release_event(GdkEventButton* event) {
    try {
        _myEventAdapter.addMouseButtonEvent(event);
        //queue_draw();
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

bool
RenderArea::on_button_press_event(GdkEventButton* event) {
    try {
        _myEventAdapter.addMouseButtonEvent(event);
        //queue_draw();
    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

bool
RenderArea::on_idle () {
    try {
        /* Invalidate the whole window. */
        queue_draw();

        /* Update synchronously (fast). */
        gdk_window_process_updates (GTK_WIDGET(gobj())->window, FALSE);

    } catch (const asl::Exception & ex) {
        AC_FATAL << "Exception caught: " << ex;
    } catch (const exception & ex) {
        AC_FATAL << "Exception caught: " << ex.what();
    } catch (...) {
        AC_FATAL << "Unknown exception";
    }
    return true;
}

void
RenderArea::idle_add ()
{
  if (_myIdleId.empty())
    {
        _myIdleId = Glib::signal_idle().connect( sigc::mem_fun(*this, &RenderArea::on_idle) , GDK_PRIORITY_REDRAW);
    }
}

void
RenderArea::idle_remove ()
{
  if (_myIdleId.connected())
    {
        _myIdleId.disconnect();
    }
}

void
RenderArea::setIdleMode(bool theEnabledFlag) {
    if (theEnabledFlag) {
        idle_add();
    } else {
        idle_remove();
    }
}

bool
RenderArea::on_map_event (GdkEventAny *event) {
    //try {
    //    idle_add ();

    //} catch (const asl::Exception & ex) {
    //    AC_FATAL << "Exception caught: " << ex;
    //} catch (const exception & ex) {
    //    AC_FATAL << "Exception caught: " << ex.what();
    //} catch (...) {
    //    AC_FATAL << "Unknown exception";
    //}
    return true;
}

bool
RenderArea::on_unmap_event (GdkEventAny *event) {
    //try {
    //    idle_remove ();

    //} catch (const asl::Exception & ex) {
    //    AC_FATAL << "Exception caught: " << ex;
    //} catch (const exception & ex) {
    //    AC_FATAL << "Exception caught: " << ex.what();
    //} catch (...) {
    //    AC_FATAL << "Unknown exception";
    //}
    return true;
}

bool
RenderArea::on_visibility_notify_event (GdkEventVisibility *event) {
    //try {
    //
    //    if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
    //    idle_remove ();
    //    else
    //    idle_add ();
    //


    //} catch (const asl::Exception & ex) {
    //    AC_FATAL << "Exception caught: " << ex;
    //} catch (const exception & ex) {
    //    AC_FATAL << "Exception caught: " << ex.what();
    //} catch (...) {
    //    AC_FATAL << "Unknown exception";
    //}
    return true;
}


TTFTextRendererPtr
RenderArea::createTTFRenderer() {
    return TTFTextRendererPtr();
}

void
RenderArea::initDisplay() {
    GLResourceManager::get().loadShaderLibrary();
}

}
