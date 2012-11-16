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
//   $RCSfile: GTKApp.cpp,v $
//   $Author: david $
//   $Revision: 1.5 $
//   $Date: 2005/01/25 11:58:15 $
//
//
//=============================================================================

// own header
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



