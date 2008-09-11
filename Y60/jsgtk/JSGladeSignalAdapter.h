//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <asl/base/Ptr.h>

#include <y60/jsbase/jssettings.h>
#include <js/spidermonkey/jsapi.h>

#include <list>
#include <string>
#include <vector>

#include <glib-object.h>
#include <gtkmm/widget.h>

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
