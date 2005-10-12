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

#ifndef _Y60_ACGTKSHELL_JSSIGNALS_INCLUDED_
#define _Y60_ACGTKSHELL_JSSIGNALS_INCLUDED_

#include "JSSignal0.h"
#include "JSSignal1.h"
#include "JSSignal2.h"

#include "JSGdkEvent.h"

#include <string>

namespace jslib {
// TODO: collect Gtk signal signatures and instanciate the required
//       signal proxies.
// XXX: if you add something here don't forget to add it to the initClass 
//      section in jsgtk.cpp

//=====================================================
// RVAL on_signal();
//=====================================================

DEFINE_SIGNAL0_CLASS_TRAIT(void)

//=====================================================
// RVAL on_signal(P1);
//=====================================================

DEFINE_SIGNAL1_CLASS_TRAIT(void, int)
DEFINE_SIGNAL1_CLASS_TRAIT(void, double)
DEFINE_SIGNAL1_CLASS_TRAIT(void, float)
DEFINE_SIGNAL1_CLASS_TRAIT(bool, double)

DEFINE_SIGNAL1_CLASS_TRAIT(void, Glib::ustring)

//=====================================================
// RVAL on_signal(P1, P2);
//=====================================================

DEFINE_SIGNAL2_CLASS_TRAIT(void, double, double)
DEFINE_SIGNAL2_CLASS_TRAIT(bool, double, Glib::ustring)

} // end of namespace jslib

#endif // _Y60_ACGTKSHELL_JSSIGNAL_PROXYS_INCLUDED_


