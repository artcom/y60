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
//   $RCSfile: JSSignalProxies.h,v $
//   $Author: david $
//   $Revision: 1.3 $
//   $Date: 2005/04/13 15:24:13 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSIGNAL_PROXYS_INCLUDED_
#define _Y60_ACGTKSHELL_JSSIGNAL_PROXYS_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSSignalProxy0.h"
#include "JSSignalProxy1.h"
#include "JSSignalProxy2.h"

#include "JSGdkEvent.h"

#if defined(_MSC_VER)
    #pragma warning(push,1)
    #pragma warning(disable:4413 4244 4512 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/notebook.h>
#include <gtkmm/treeiter.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <string>

namespace jslib {
// TODO: collect Gtk signal signatures and instanciate the required
//       signal proxies.
// XXX: if you add something here don't forget to add it to the initClass
//      section in jsgtk.cpp

//=====================================================
// RVAL on_signal();
//=====================================================

// Button.signal_clicked()
// Range.signal_value_changed()
// Widget.signal_show()
// Widget.signal_hide()
// Widget.signal_map()
// Widget.signal_unmap()
// Widget.signal_realize()
// Widget.signal_unrealize()
DEFINE_SIGNALPROXY0_CLASS_TRAIT(void)

//=====================================================
// RVAL on_signal(P1);
//=====================================================

// Dialog.signal_response
DEFINE_SIGNALPROXY1_CLASS_TRAIT(void, int)
// Range.signal_adjust_bounds
DEFINE_SIGNALPROXY1_CLASS_TRAIT(void, double)
// Range.signal_change_value
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, double)
// Scale.signal_format_value
DEFINE_SIGNALPROXY1_CLASS_TRAIT(Glib::ustring, double)
// Scale.signal_format_value
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventButton*)
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventMotion*)
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventKey*)
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventCrossing*)
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventFocus*)
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventAny*)

DEFINE_SIGNALPROXY1_CLASS_TRAIT(std::string, const std::string &)

//=====================================================
// RVAL on_signal(P1, P2);
//=====================================================

// CellRendererText.signal_edited()
DEFINE_SIGNALPROXY2_CLASS_TRAIT(void, const Glib::ustring &, const Glib::ustring &)

// Statusbar.signal_text_pushed()
// Statusbar.signal_text_popped()
DEFINE_SIGNALPROXY2_CLASS_TRAIT(void, guint, const Glib::ustring &)

// Notebook.signal_switch_page
DEFINE_SIGNALPROXY2_CLASS_TRAIT(void, GtkNotebookPage*, guint)

} // end of namespace jslib

#endif // _Y60_ACGTKSHELL_JSSIGNAL_PROXYS_INCLUDED_


