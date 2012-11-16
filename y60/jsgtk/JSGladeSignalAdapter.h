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
//   $RCSfile: JSGladeSignalAdapter.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/01/25 11:58:15 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSGLADE_SIGNALADAPTER_INCLUDED_
#define _Y60_ACGTKSHELL_JSGLADE_SIGNALADAPTER_INCLUDED_

#include "y60_jsgtk_settings.h"

#include <asl/base/Ptr.h>

#include <y60/jsbase/jssettings.h>

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#else
#include <js/jsapi.h>
#endif

#include <list>
#include <string>
#include <vector>

#include <glib-object.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
    #pragma warning(disable:4250)
#endif //defined(_MSC_VER)
#include <gtkmm/widget.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

struct JSAutoconnectInfo {
    JSObject * object;
    JSContext * context;
    Gtk::Widget * topWidget;
};

class JSGladeSignalAdapter {
    public:
        JSGladeSignalAdapter(const gchar *theHandlerName,
                GObject *theSignalSource,
                const gchar *theSignalName,
                const gchar *theSignalData,
                GObject *theConnectObject,
                gboolean theCallAfterFlag,
                JSContext  * theContext,
                JSObject * theTarget);
        virtual ~JSGladeSignalAdapter();
        bool trigger(GdkEvent * theEvent = 0);
    private:
        static gulong dispatchSignal(GObject * theSource, gpointer theUserData);
        static gulong dispatchEvent(GObject * theSource, GdkEvent * theEvent, gpointer theUserData);
    private:
        JSGladeSignalAdapter(); // hide default ctor
        JSGladeSignalAdapter(const JSGladeSignalAdapter &); // hide copy
        JSGladeSignalAdapter & operator=(const JSGladeSignalAdapter &); // hide assignment
        std::string _handlerName;
        GObject * _gtkSignalSource;
        gulong _myCallbackId;
        std::string _signalName;
        std::string _signalData;
        GObject * _connectObejct;
        bool _callAfterFlag;
        JSContext * _jsContext;
        JSObject * _jsTarget;
};
typedef asl::Ptr<JSGladeSignalAdapter> JSGladeSignalAdapterPtr;
typedef std::vector<JSGladeSignalAdapterPtr> JSGladeSignalAdapterVector;

#endif
