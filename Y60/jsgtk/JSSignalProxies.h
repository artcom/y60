//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include "JSSignalProxy0.h"
#include "JSSignalProxy1.h"
#include "JSSignalProxy2.h"

#include "JSGdkEvent.h"

#include <gtkmm/notebook.h>
#include <gtkmm/treeiter.h>
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
DEFINE_SIGNALPROXY1_CLASS_TRAIT(bool, GdkEventAny*)

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


