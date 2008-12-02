#include "JSGladeSignalAdapter.h"
#include "jsgtk.h"

#include <y60/jsbase/JScppUtils.h>
#include <gtkmm.h>

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

