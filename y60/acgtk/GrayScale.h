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
//
//   $RCSfile: SliceViewer.js,v $
//   $Author: danielk $
//   $Revision: 1.97 $
//   $Date: 2005/04/29 13:49:59 $
//
//=============================================================================

#ifndef ACGTK_GRAY_SCALE_INCLUDED
#define ACGTK_GRAY_SCALE_INCLUDED

#include "y60_acgtk_settings.h"

#include <asl/math/Vector234.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)

#include <gtkmm/drawingarea.h>

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

#include <sigc++/sigc++.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4250 4275)
#endif //defined(_MSC_VER)

namespace acgtk {

#undef GrayScale

class Y60_ACGTK_DECL GrayScale : public Gtk::DrawingArea {
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

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

#endif // ACGTK_GRAY_SCALE_INCLUDED
