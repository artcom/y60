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
#include "JSNotebook.h"

#include "JSContainer.h"
#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Notebook, asl::Ptr<Gtk::Notebook>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Notebook@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
append_page(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
/*
int     append_page (Widget& child)
int     append_page (Widget& child, Widget& tab_label)
int     append_page (Widget& child, Widget& tab_label, Widget& menu_label)
int     append_page (Widget& child, const Glib::ustring& tab_label, bool use_mnemonic=false)
int     append_page (Widget& child, const Glib::ustring& tab_label, const Glib::ustring& menu_label, bool use_mnemonic=false)
*/
    try {
        ensureParamCount(argc, 1, 4);

        Gtk::Notebook * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int myRetVal = 0;
        // first argument must be a widget
        Gtk::Widget * myChild;
        if (!convertFrom(cx, argv[0], myChild)) {
            JS_ReportError(cx, "JSNotebook::append_page(): First argument must be a widget.");
            return JS_FALSE;
        }
        // second argument must be a widget or a string
        Glib::ustring myStringLabel;
        Gtk::Widget * myWidgetLabel = 0;
        if (argc >= 2) {
            if (!convertFrom(cx, argv[1], myWidgetLabel)) {
                if (!convertFrom(cx, argv[1], myStringLabel)) {
                    JS_ReportError(cx, "JSNotebook::append_page(): Second argument must be a widget or a string.");
                    return JS_FALSE;
                }
            }
        }

        switch (argc) {
            case 1:
                myRetVal = mySelf->append_page(*myChild);
                break;
            case 2:
                if (myWidgetLabel) {
                    myRetVal = mySelf->append_page(*myChild, *myWidgetLabel);
                } else {
                    myRetVal = mySelf->append_page(*myChild, myStringLabel);
                }
                break;
            case 3:
                if (myWidgetLabel) {
                    Gtk::Widget * myMenu = 0;
                    if (!convertFrom(cx, argv[2], myMenu)) {
                        JS_ReportError(cx, "JSNotebook::append_page(): Third argument must be a Widget.");
                        return JS_FALSE;
                    }
                    myRetVal = mySelf->append_page(*myChild, *myWidgetLabel, *myMenu);
                } else {
                    if (JSVAL_IS_BOOLEAN(argv[2])) {
                        bool myMnemonicFlag;
                        if (!convertFrom(cx, argv[2], myMnemonicFlag)) {
                            JS_ReportError(cx, "JSNotebook::append_page(): Third argument must be a bool or string.");
                            return JS_FALSE;
                        }
                        myRetVal = mySelf->append_page(*myChild, myStringLabel, myMnemonicFlag);
                    } else {
                        Glib::ustring myMenuLabel;
                        if (!convertFrom(cx, argv[2], myMenuLabel)) {
                            JS_ReportError(cx, "JSNotebook::append_page(): Third argument must be a bool or string.");
                            return JS_FALSE;
                        }
                        myRetVal = mySelf->append_page(*myChild, myStringLabel, myMenuLabel);
                    }
                }
                break;
            case 4:
                {
                    Glib::ustring myMenuLabel;
                    if (!convertFrom(cx, argv[2], myMenuLabel)) {
                        JS_ReportError(cx, "JSNotebook::append_page(): Third argument must be a bool or string.");
                        return JS_FALSE;
                    }
                    bool myMnemonicFlag;
                    if (!convertFrom(cx, argv[3], myMnemonicFlag)) {
                        JS_ReportError(cx, "JSNotebook::append_page(): fourth argument must be a bool.");
                        return JS_FALSE;
                    }
                    myRetVal = mySelf->append_page(*myChild, myStringLabel, myMenuLabel, myMnemonicFlag);
                }
                break;
        }
        *rval = as_jsval(cx, myRetVal);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
remove_page(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
/*
int     append_page (Widget& child)
int     append_page (Widget& child, Widget& tab_label)
int     append_page (Widget& child, Widget& tab_label, Widget& menu_label)
int     append_page (Widget& child, const Glib::ustring& tab_label, bool use_mnemonic=false)
int     append_page (Widget& child, const Glib::ustring& tab_label, const Glib::ustring& menu_label, bool use_mnemonic=false)
*/
    try {
        ensureParamCount(argc, 1);

        Gtk::Notebook * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        // first argument must be a widget or an int
        Gtk::Widget * myPageToRemove = 0;
        int myIndexToRemove = 0;

        if (!convertFrom(cx, argv[0], myPageToRemove)) {
            if (!convertFrom(cx, argv[0], myIndexToRemove)) {
                JS_ReportError(cx, "JSNotebook::remove_page(): argument must be a widget or an int.");
                return JS_FALSE;
            }
        }

        if (myPageToRemove) {
            mySelf->remove_page(*myPageToRemove);
        } else {
            mySelf->remove_page(myIndexToRemove);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_tab_label_text(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
/*
void    set_tab_label_text(Widget& child, const Glib::ustring& tab_label)
*/
    try {
        ensureParamCount(argc, 1);

        Gtk::Notebook * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        // first argument must be a widget or an int
        Gtk::Widget * myPage = 0;

        if (!convertFrom(cx, argv[0], myPage)) {
            JS_ReportError(cx, "JSNotebook::set_tab_label_text(): first argument must be a widget.");
            return JS_FALSE;
        }

        Glib::ustring myNewLabel;
        if (!convertFrom(cx, argv[1], myNewLabel)) {
            JS_ReportError(cx, "JSNotebook::set_tab_label_text(): second argument must be a string.");
            return JS_FALSE;
        }

        mySelf->set_tab_label_text(*myPage, myNewLabel);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSNotebook::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"append_page",          append_page,             4},
        {"remove_page",          remove_page,             1},
        {"set_tab_label_text",   set_tab_label_text,      2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSNotebook::Properties() {
    static JSPropertySpec myProperties[] = {
        {"current_page", PROP_current_page, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"n_pages",      PROP_n_pages, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_switch_page",  PROP_signal_switch_page, JSPROP_READONLY | JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSNotebook::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSNotebook::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSNotebook::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_current_page:
            *vp = as_jsval(cx, theNative.get_current_page());
            return JS_TRUE;
        case PROP_n_pages:
            *vp = as_jsval(cx, theNative.get_n_pages());
            return JS_TRUE;
        case PROP_signal_switch_page:
            {
                JSSignalProxy2<void, GtkNotebookPage*, guint>::OWNERPTR mySignal( new
                    JSSignalProxy2<void, GtkNotebookPage*, guint>::NATIVE(theNative.signal_switch_page()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSNotebook::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_current_page:
                {
                    int thePage;
                    convertFrom(cx, *vp, thePage);
                    theNative.set_current_page(thePage);
                    return JS_TRUE;
                }
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSNotebook::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSNotebook * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        myNewObject = new JSNotebook(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSNotebook::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSNotebook::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
            "POST",              PROP_HTTP_POST,         Request::HTTP_POST,
            "GET",               PROP_HTTP_GET,          Request::HTTP_GET,
            "PUT",               PROP_HTTP_PUT,          Request::HTTP_PUT,
        {0}
    };
    return myProperties;
};
*/

void
JSNotebook::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
                JSBASE::ClassName());
}

JSObject *
JSNotebook::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSNotebook::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSNotebook::OWNERPTR theOwner, JSNotebook::NATIVE * theNative) {
    JSObject * myReturnObject = JSNotebook::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

// NOTE: we don't expose Notebook Pages yet, and they are deprecated anyway, so we just wrap this as an int 
template<>
jsval as_jsval<GtkNotebookPage>(JSContext *cx, GtkNotebookPage * theNative) {
    return as_jsval(cx, reinterpret_cast<intptr_t>(theNative));
}


}
