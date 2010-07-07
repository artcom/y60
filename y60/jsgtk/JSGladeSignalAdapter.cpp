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

// own header
#include "JSGladeSignalAdapter.h"

#include "jsgtk.h"

#include <y60/jsbase/JScppUtils.h>
#if defined(_MSC_VER)
    // supress warnings caused by gtk in vc++
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace jslib;
#define DB(x) x


JSGladeSignalAdapter::JSGladeSignalAdapter(const gchar *theHandlerName,
                GObject *theSignalSource,
                const gchar *theSignalName,
                const gchar *theSignalData,
                GObject *theConnectObject,
                gboolean theCallAfterFlag,
                JSContext  * theContext,
                JSObject * theTarget) :
        _handlerName(theHandlerName),
        _gtkSignalSource(theSignalSource),
        _connectObejct(theConnectObject),
        _callAfterFlag(0 != theCallAfterFlag),
        _jsContext(theContext),
        _jsTarget(theTarget)
{
    if (theSignalName) {
        _signalName = std::string(theSignalName);
    }
    if (theSignalData) {
        _signalData = std::string(theSignalData);
    }
    // if the signal name ends with "event", then it is an event signal
    bool isEvent = _signalName.length() >= 5 && _signalName.substr(_signalName.length()-5) == "event";

    GCallback myCallback = isEvent ? G_CALLBACK(dispatchEvent) : G_CALLBACK(dispatchSignal);

    if (theCallAfterFlag) {
        _myCallbackId = g_signal_connect_after(_gtkSignalSource, theSignalName,
                myCallback, this);
    } else {
        _myCallbackId = g_signal_connect(_gtkSignalSource, theSignalName,
                myCallback, this);
    }
    return;
};

JSGladeSignalAdapter::~JSGladeSignalAdapter() {
    g_signal_handler_disconnect( _gtkSignalSource, _myCallbackId);
}

gulong
JSGladeSignalAdapter::dispatchEvent(GObject* theSource, GdkEvent * theEvent, gpointer theUserData) {
    JSGladeSignalAdapter * myAdapter = static_cast<JSGladeSignalAdapter*>(theUserData);
    myAdapter->trigger(theEvent);
    return 0;
}

gulong
JSGladeSignalAdapter::dispatchSignal(GObject* theSource, gpointer theUserData) {
    JSGladeSignalAdapter * myAdapter = static_cast<JSGladeSignalAdapter*>(theUserData);
    myAdapter->trigger();
    return 0;
}

bool
JSGladeSignalAdapter::trigger(GdkEvent * theEvent) {
    // does the required function object exists?
    AC_DEBUG << "triggering " << _handlerName << endl;
    jsval myVal;
    /*JSBool bOK =*/ JS_GetProperty(_jsContext, _jsTarget, _handlerName.c_str(), &myVal);
    if (myVal == JSVAL_VOID) {
        AC_WARNING << "JS event handler for event '" << _handlerName << "' missing." << endl;
        return false;
    }

    // call the function
    jsval argv[1], rval;
    int myArgCount = 0;

    Gtk::Object * mySourceObject = Glib::wrap(GTK_OBJECT(_gtkSignalSource), true);
    Gtk::Widget * mySourceWidget = dynamic_cast<Gtk::Widget*>(mySourceObject);
    if (mySourceWidget) {
        argv[0] = gtk_jsval(_jsContext, mySourceWidget);
        myArgCount++;
    }
    JSBool ok = JSA_CallFunctionName(_jsContext, _jsTarget, _handlerName.c_str(), myArgCount, argv, &rval);
    AC_TRACE << "trigger " << _handlerName << " retval=" << ok << endl;
    return 0 != ok;
}

