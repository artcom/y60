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

#include "y60_jsgtk_settings.h"

#include "JSSignal0.h"
#include "JSSignal1.h"
#include "JSSignal2.h"
#include "JSSignal3.h"

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
DEFINE_SIGNAL0_CLASS_TRAIT(int)

//=====================================================
// RVAL on_signal(P1);
//=====================================================

DEFINE_SIGNAL1_CLASS_TRAIT(void, int)
DEFINE_SIGNAL1_CLASS_TRAIT(void, double)
DEFINE_SIGNAL1_CLASS_TRAIT(void, float)
DEFINE_SIGNAL1_CLASS_TRAIT(bool, double)

DEFINE_SIGNAL1_CLASS_TRAIT(void, Glib::ustring)
DEFINE_SIGNAL1_CLASS_TRAIT(void, const std::string &)

//=====================================================
// RVAL on_signal(P1, P2);
//=====================================================

DEFINE_SIGNAL2_CLASS_TRAIT(void, double, double)
DEFINE_SIGNAL2_CLASS_TRAIT(bool, double, Glib::ustring)
DEFINE_SIGNAL2_CLASS_TRAIT(void, Glib::ustring, Glib::ustring)

//=====================================================
// RVAL on_signal(P1, P2, P3);
//=====================================================

DEFINE_SIGNAL3_CLASS_TRAIT(void, double, double, unsigned int)
} // end of namespace jslib

#endif // _Y60_ACGTKSHELL_JSSIGNAL_PROXYS_INCLUDED_


