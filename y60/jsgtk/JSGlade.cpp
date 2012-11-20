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

// own header
#include "JSGlade.h"

#include "JSWindow.h"
#include "GTKApp.h"
#include "jsgtk.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#include <glade/glade.h>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gnome::Glade::Xml, Glib::RefPtr<Gnome::Glade::Xml>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("Glade@") +
        JSGlade::getJSWrapper(cx,obj).getOwner()->get_filename();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
reparent_widget(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        Gtk::Window * nativeWindow;
        std::string myChildId;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "Gtk::Main::run(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc == 2) {
            if (!convertFrom(cx, argv[0], myChildId)) {
                JS_ReportError(cx, "JSRequestWrapper::Constructor: argument #1 must be the childid");
                return JS_FALSE;
            }
            if (!convertFrom(cx, argv[1], nativeWindow)) {
                JS_ReportError(cx, "Gtk::Main::run(): argument #1 must be a Gtk::Window");
                return JS_FALSE;
            }
        } else  {
            JS_ReportError(cx, "Gtk::Main::run(): Wrong number of arguments, 1 expected.");
            return JS_FALSE;
        }
        JSGlade::getJSWrapper(cx,obj).getOwner()->reparent_widget(myChildId, *nativeWindow);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
autoconnect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "Gtk::Main::run(): Wrong number of arguments, 2 expected.");
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "Gtk::Main::run(): Argument #%d is undefined", 1);
            return JS_FALSE;
        }

        JSAutoconnectInfo myInfo;
        JS_ValueToObject(cx, argv[0], &myInfo.object);
        convertFrom(cx, argv[1], myInfo.topWidget);
        myInfo.context = cx;

        glade_xml_signal_autoconnect_full (JSGlade::getJSWrapper(cx,obj).getOwner()->gobj(),
                GTKApp::addSignalHandler, &myInfo);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
get_widget(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        // native method call
        string theName;
        convertFrom(cx, argv[0], theName);

        Gtk::Widget * myWidget = JSGlade::getJSWrapper(cx,obj).getOwner()->get_widget(theName);
        // if the widget is top-level (i.e. has no parent), the JS should take ownership
        // bool takeOwnership = (myWidget->get_parent() == 0);
        if (myWidget) {
            *rval = gtk_jsval(cx, myWidget, false);
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
/* not working
static JSBool
ensure_accel_group(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        // native method call
        GladeXML * myGlade = JSGlade::getJSWrapper(cx,obj).getOwner()->gobj();
        GtkAccelGroup * myAccelGroup = glade_xml_ensure_accel(myGlade);
        cerr << "GtkAccelGroup =" << myAccelGroup << endl;
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
*/
JSFunctionSpec *
JSGlade::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"reparent_widget",      reparent_widget,         2},
        {"get_widget",           get_widget,              1},
        {"autoconnect",          autoconnect,             2},
//        {"ensure_accel_group",   ensure_accel_group,      0},  // Not Working
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {
    PROP_filename = -100
};

JSPropertySpec *
JSGlade::Properties() {
    static JSPropertySpec myProperties[] = {
        {"filename", PROP_filename, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"responseString", PROP_responseString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"errorString", PROP_errorString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
//        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

JSBool
JSGlade::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSGlade::OWNERPTR myNewObject;

    std::string myGladeFilename;
    std::string myWidgetID;
    std::string myDomain;

    if (argc >= 1) {
        if (!convertFrom(cx, argv[0], myGladeFilename)) {
            JS_ReportError(cx, "JSRequestWrapper::Constructor: argument #1 must be the glade filename");
            return JS_FALSE;
        }
    }
    if (argc >= 2) {
        if (!convertFrom(cx, argv[1], myWidgetID)) {
            JS_ReportError(cx, "JSRequestWrapper::Constructor: argument #2 can be the widget id");
            return JS_FALSE;
        }
    }
    if (argc >= 3) {
        if (!convertFrom(cx, argv[2], myDomain)) {
            JS_ReportError(cx, "JSRequestWrapper::Constructor: argument #3 can be the domain");
            return JS_FALSE;
        }
    }

    std::string myGladeFileWithPath = searchFileRelativeToJSInclude(cx, obj, argc, argv, myGladeFilename);

    switch (argc) {
        case 1:
            myNewObject = Gnome::Glade::Xml::create(myGladeFileWithPath);
            break;
        case 2:
            myNewObject = Gnome::Glade::Xml::create(myGladeFileWithPath, myWidgetID);
            break;
        case 3:
            myNewObject = Gnome::Glade::Xml::create(myGladeFileWithPath, myWidgetID, myDomain);
            break;
        default:
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
            return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,new JSGlade(myNewObject));
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSGlade::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSGlade::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSGlade::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0);
    return myClassObject;
}

// getproperty handling
JSBool
JSGlade::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_filename:
            {
                // using the c++ API crashed on Win32-dbg.
                // don't know why, be using the C-API works
                string myFilename = string(JSClassTraits<JSGlade::NATIVE>::getNativeOwner(cx, obj)->gobj()->filename);
                *vp = as_jsval(cx, myFilename);
            }
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSRenderer::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}
bool convertFrom(JSContext *cx, jsval theValue, JSGlade::OWNERPTR & theGlade) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSGlade::NATIVE >::Class()) {
                theGlade = JSClassTraits<JSGlade::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSGlade::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSGlade::Construct(cx, theOwner);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

