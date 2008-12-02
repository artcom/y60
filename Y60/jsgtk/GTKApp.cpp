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
//   $RCSfile: GTKApp.cpp,v $
//   $Author: david $
//   $Revision: 1.5 $
//   $Date: 2005/01/25 11:58:15 $
//
//
//=============================================================================

#include "GTKApp.h"
#include "jsgtk.h"
#include "JSSignal1.h"
#include <y60/acgtk/GCObserver.h>
#include <y60/jsbase/Documentation.h>

#define DB(x) // x

using namespace std;
using namespace asl;

namespace jslib {
    
static JSFunctionSpec glob_functions[] = {
    {0}
};

GTKApp::GTKApp() 
{
}

bool 
GTKApp::initClasses(JSContext * theContext, JSObject * theGlobalObject) {
    GCObserver::get().attach(theContext);
    if (!JSApp::initClasses(theContext, theGlobalObject)) {
        return false;
    }
    if (!initGtkClasses(theContext, theGlobalObject)) {
        return false;
    }
    if (!JS_DefineFunctions(theContext, theGlobalObject, glob_functions)) {
        return false;
    }
    return true;
}


//
//   static functions
//
GTKApp::SignalAdapterMap GTKApp::_mySignalAdapterMap_;

gulong
GTKApp::deleteSignalHandler(GObject* theSource, gpointer theUserData) {
    SignalAdapterMap::iterator it = _mySignalAdapterMap_.find(theSource);
    if (it != _mySignalAdapterMap_.end()) {
        DB(std::cerr << "deleting signal adaptor" << endl);
        _mySignalAdapterMap_.erase(it);
    }
    return 0;
}

void 
GTKApp::addSignalHandler(const gchar *theHandlerName, GObject *theSource, const gchar *theSignalName,
                   const gchar *theSignalData, GObject *theConnectObject, gboolean theAfterFlag,
                   gpointer theAutoconnectInfo) 
{
    JSAutoconnectInfo * myInfo = static_cast<JSAutoconnectInfo*>(theAutoconnectInfo);
    GObject * mySignalSource = theSource;
    // Allow a different top-level widget to be connected (useful when reparent_widget was
    // used on a glade file)
    if (gtk_widget_get_parent(GTK_WIDGET(theSource)) == 0) {
        mySignalSource = G_OBJECT(myInfo->topWidget->gobj());
    }

    SignalAdapterMap::iterator it = _mySignalAdapterMap_.find(mySignalSource);
    if (it == _mySignalAdapterMap_.end()) {
        it = _mySignalAdapterMap_.insert(make_pair(mySignalSource, JSGladeSignalAdapterVector())).first;
        g_signal_connect(mySignalSource, "delete_event", 
                G_CALLBACK(GTKApp::deleteSignalHandler), mySignalSource);
    }

    JSGladeSignalAdapterPtr myAdapter(new JSGladeSignalAdapter(theHandlerName, mySignalSource, theSignalName, 
                theSignalData, theConnectObject, theAfterFlag, myInfo->context, myInfo->object));
    it->second.push_back(myAdapter);
    return;
}

}



