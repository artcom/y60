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

// this is actually a wrapper of a combobox which holds only one column of text

// own header
#include "JSComboBox.h"

#include "JSContainer.h"
#include "JSTreeIter.h"
#include "JSCellRenderer.h"
#include "JSSignalProxy0.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/liststore.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::ComboBox, asl::Ptr<Gtk::ComboBox>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::ComboBox@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

// XXX clear_items appears in Gtk 2.8  [DS]
// until then, we dispatch to get_model()->clear
static JSBool
clear_items(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("calls get_model().clear(), removing all the rows");
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSComboBox::clear_items(): Wrong number of arguments, 0 expected.");
            return JS_FALSE;
        }

        Gtk::ComboBox * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        Glib::RefPtr<Gtk::ListStore> myList = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(mySelf->get_model());
        if (!myList) {
            JS_ReportError(cx, "JSComboBox::clear_items() can only be called on Gtk::ListStore-based ComboBoxes");
            return JS_FALSE;
        }
        myList->clear();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSComboBox::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        {"toString",                    toString,                      0},
        {"clear_items",                 clear_items,                   0},
        // name                      native                    nargs
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSComboBox::Properties() {
    static JSPropertySpec myProperties[] = {
        {"active_text",        PROP_active_text,         JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"active_index",        PROP_active_index,         JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_changed", PROP_signal_changed, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSComboBox::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSComboBox::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSComboBox::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_active_text:
            try {
                Gtk::TreeModel::iterator iter = theNative.get_active();
                if (iter) {
                    int myColCount = theNative.get_model()->get_n_columns();
                    if (myColCount > 1) {
                        cerr << "### WARNING unexpected number of columns > 1 in JSComboBox "
                             << endl;
                    }
                    Glib::ustring myString;
                    iter->get_value(0, myString);
                    *vp = as_jsval(cx, myString);
                } else {
                    *vp = JSVAL_NULL;
                }
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_active_index:
            *vp = as_jsval(cx, theNative.get_active_row_number());
            return JS_TRUE;
        case PROP_signal_changed:
            {
                JSSignalProxy0<void>::OWNERPTR mySignal(new
                        JSSignalProxy0<void>::NATIVE(theNative.signal_changed()));
                *vp = jslib::as_jsval(cx, mySignal);
                return JS_TRUE;
            }
            break;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSComboBox::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_active_text:
            try {
                Gtk::TreeModel::Children children = theNative.get_model()->children();
                Gtk::TreeModel::iterator myChild;
                Glib::ustring myText;
                convertFrom(cx, *vp, myText);

                for(myChild = children.begin();myChild != children.end(); ++myChild) {
                    Glib::ustring myChildText;
                    myChild->get_value(0, myChildText);
                    if (myChildText == myText) {
                        theNative.set_active(myChild);
                        return JS_TRUE;
                    }
                }
                JS_ReportError(cx,"ComboBox: '%s' is not a valid entry.",myText.c_str());
                return JS_FALSE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_active_index:
            {
                int myIndex = 0;
                convertFrom(cx, *vp, myIndex);
                theNative.set_active(myIndex);
                return JS_TRUE;
            }
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSComboBox::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSComboBox * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        myNewObject = new JSComboBox(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSComboBox::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSComboBox::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
            "TEXT",              PROP_CELL_RENDERER_TEXT,     PROP_CELL_RENDERER_TEXT,
            "PIXBUF",            PROP_CELL_RENDERER_PIXBUF,   PROP_CELL_RENDERER_PIXBUF,
            "PROGRESS",          PROP_CELL_RENDERER_PROGRESS, PROP_CELL_RENDERER_PROGRESS,
            "TOGGLE",            PROP_CELL_RENDERER_TOGGLE,   PROP_CELL_RENDERER_TOGGLE,
            "COMBO",             PROP_CELL_RENDERER_COMBO,    PROP_CELL_RENDERER_COMBO,
        {0}
    };
    return myProperties;
};
*/

void
JSComboBox::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), 0, 0, 0,
            JSBASE::ClassName());
}

JSObject *
JSComboBox::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0, 0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSComboBox::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSComboBox::OWNERPTR theOwner, JSComboBox::NATIVE * theNative) {
    JSObject * myReturnObject = JSComboBox::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

