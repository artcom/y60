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
#include "JSTreeView.h"

#include "JSContainer.h"
#include "JSTreeIter.h"
#include "JSCellRenderer.h"
#include "JSCellRendererText.h"
#include "ACColumnRecord.h"
#include "JSSignalProxies.h"
#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JScppUtils.impl>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::TreeView, asl::Ptr<Gtk::TreeView>, StaticAccessProtocol>;

template jsval as_jsval(JSContext *cx, const std::vector<Glib::ustring> & theVector);

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::TreeView@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

  struct ModelColumns : public Gtk::TreeModelColumnRecord
  {
    Gtk::TreeModelColumn<bool>          fixed;
    Gtk::TreeModelColumn<unsigned int>  number;
    Gtk::TreeModelColumn<Glib::ustring> severity;
    Gtk::TreeModelColumn<Glib::ustring> description;

    ModelColumns() { add(fixed); add(number); add(severity); add(description); }
  };

static JSBool
SetModel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        // native method call
        Gtk::TreeView * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        Glib::RefPtr<Gtk::TreeModel> myModel;
        convertFrom(cx, argv[0], myModel);

        myNative->set_model(myModel);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Unselect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        // native method call
        Gtk::TreeView * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = myNative->get_selection();
        asl::Ptr<Gtk::TreeIter> myRow;
        convertFrom(cx, argv[0],myRow);
        refTreeSelection->unselect(*myRow);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Select(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        // native method call
        Gtk::TreeView * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = myNative->get_selection();
        asl::Ptr<Gtk::TreeIter> myRow;
        convertFrom(cx, argv[0],myRow);
        refTreeSelection->select(*myRow);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
AppendColumn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 3);
        // native method call
        Gtk::TreeView * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        std::string myColLabel;
        convertFrom(cx,argv[0],myColLabel);

        ACColumnRecord * myScheme = 0;
        convertFrom(cx,argv[1],myScheme);

        unsigned int myColNum = 0;
        convertFrom(cx, argv[2], myColNum);

        int myNewColNum = 0;
        if (myColNum == 0) {
            myNewColNum = myNative->append_column(myColLabel, myScheme->_myRecId);
        } else {
            myNewColNum = myNative->append_column(myColLabel, myScheme->_myColumns.at(myColNum-1));
        }
        *rval = as_jsval(cx, myNewColNum);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
AppendColumnEditable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 3);
        // native method call
        Gtk::TreeView * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        std::string myColLabel;
        convertFrom(cx,argv[0],myColLabel);

        ACColumnRecord * myScheme = 0;
        convertFrom(cx,argv[1],myScheme);

        unsigned int myColNum = 0;
        convertFrom(cx, argv[2], myColNum);

        int myNewColNum = 0;
        if (myColNum == 0) {
            myNewColNum = myNative->append_column_editable(myColLabel, myScheme->_myRecId);
        } else {
            myNewColNum = myNative->append_column_editable(myColLabel, myScheme->_myColumns.at(myColNum-1));
        }
        *rval = as_jsval(cx, myNewColNum);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
GetColumnCellRenderer(JSContext * cx, JSObject  * obj, uintN argc, jsval * argv, jsval  * rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        Gtk::TreeView * myNative(0);
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        int myColumnNumber(0);
        convertFrom(cx, argv[0], myColumnNumber);

        Gtk::CellRenderer * myCellRenderer = myNative->get_column_cell_renderer(myColumnNumber);
        if (argc > 1) {
            int myCellRendererType;
            convertFrom(cx, argv[1], myCellRendererType);
            switch (myCellRendererType) {
                case JSCellRenderer::PROP_CELL_RENDERER_TEXT:
                    {
                        Gtk::CellRendererText * myTextCellRenderer(0);
                        myTextCellRenderer = dynamic_cast<Gtk::CellRendererText * >( myCellRenderer );
                        if (myTextCellRenderer) {
                            *rval = as_jsval(cx, myTextCellRenderer);
                            return JS_TRUE;
                        } else {
                            break;
                        }
                    }

                /*
                case JSCellRenderer::PROP_CELL_RENDERER_PIXBUF:
                    {
                        Gtk::CellRendererPixbuf * myPixbufCellRenderer(0);
                        myPixbufCellRenderer = dynamic_cast<Gtk::CellRendererPixbuf * >( myCellRenderer );
                        if (myPixbufCellRenderer) {
                            *rval = as_jsval(cx, myPixbufCellRenderer);
                            return JS_TRUE;
                        } else {
                            break;
                        }
                    }
                */

                /*
                case JSCellRenderer::PROP_CELL_RENDERER_PROGRESS:
                    {
                        Gtk::CellRendererProgress * myProgressCellRenderer(0);
                        myProgressCellRenderer = dynamic_cast<Gtk::CellRendererProgress * >( myCellRenderer );
                        if (myProgressCellRenderer) {
                            *rval = as_jsval(cx, myProgressCellRenderer);
                            return JS_TRUE;
                        } else {
                            break;
                        }
                    }
                 */

                /*
                case JSCellRenderer::PROP_CELL_RENDERER_TOGGLE:
                    {
                        Gtk::CellRendererToggle * myToggleCellRenderer(0);
                        myToggleCellRenderer = dynamic_cast<Gtk::CellRendererToggle * >( myCellRenderer );
                        if (myToggleCellRenderer) {
                            *rval = as_jsval(cx, myToggleCellRenderer);
                            return JS_TRUE;
                        } else {
                            break;
                        }
                    }
                */

                /*
                case JSCellRenderer::PROP_CELL_RENDERER_COMBO:
                    {
                        Gtk::CellRendererCombo * myComboCellRenderer(0);
                        myComboCellRenderer = dynamic_cast<Gtk::CellRendererCombo * >( myCellRenderer );
                        if (myComboCellRenderer) {
                            *rval = as_jsval(cx, myComboCellRenderer);
                            return JS_TRUE;
                        } else {
                            break;
                        }
                    }
                 */

                default:
                    throw asl::Exception("Unknown cell renderer type.", PLUS_FILE_LINE);
            }
        } else {
            *rval = as_jsval(cx, myCellRenderer);
            return JS_TRUE;
        }

    } HANDLE_CPP_EXCEPTION
    return JS_FALSE;
}

JSFunctionSpec *
JSTreeView::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                      native                    nargs
        {"toString",                 toString,                 0},
        {"set_model",                SetModel,                 1},
        {"select",                   Select,                   1},
        {"unselect",                 Unselect,                 1},
        {"append_column",            AppendColumn,             3},
        {"append_column_editable",   AppendColumnEditable,     3},
        {"get_column_cell_renderer", GetColumnCellRenderer,    1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTreeView::Properties() {
    static JSPropertySpec myProperties[] = {
        {"selected_row",        PROP_selected_row,         JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"selected_row_values", PROP_selected_row_values,  JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"column_count",        PROP_column_count,         JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"headers_visible",     PROP_headers_visible,      JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"signal_cursor_changed",  PROP_signal_cursor_changed,  JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSTreeView::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTreeView::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTreeView::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_selected_row:
            try {
                Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = theNative.get_selection();
                Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
                if (iter) {
                    asl::Ptr<Gtk::TreeIter> myRow = asl::Ptr<Gtk::TreeIter>(new Gtk::TreeIter(iter));
                    *vp = as_jsval(cx, myRow, myRow.get());

                } else {
                    *vp = JSVAL_NULL;
                }
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_selected_row_values:
            try {
                Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = theNative.get_selection();
                Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
                if (iter) {
                    int myColCount = theNative.get_model()->get_n_columns();
                    vector<Glib::ustring> myStrings;
                    for (int i=1; i < myColCount; ++i) {
                        Glib::ustring myValue;
                        iter->get_value(i, myValue);
                        myStrings.push_back(myValue);
                    }
                    *vp = as_jsval(cx, myStrings);
                } else {
                    *vp = JSVAL_NULL;
                }
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_column_count:
            *vp = as_jsval(cx, theNative.get_columns().size());
            return JS_TRUE;
        case PROP_headers_visible:
            *vp = as_jsval(cx, theNative.get_headers_visible());
            return JS_TRUE;
        case PROP_signal_cursor_changed:
            {
            JSSignalProxy0<void>::OWNERPTR mySignal( new
                JSSignalProxy0<void>::NATIVE(theNative.signal_cursor_changed()));
            *vp = jslib::as_jsval(cx, mySignal);
            }
            return JS_TRUE;
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSTreeView::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_selected_row:
            try {
                Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = theNative.get_selection();
                asl::Ptr<Gtk::TreeIter> myRow;
                convertFrom(cx, *vp, myRow);
                refTreeSelection->select(*myRow);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_headers_visible:
            try {
                bool theFlag;
                convertFrom(cx, *vp, theFlag);
                theNative.set_headers_visible(theFlag);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_selected_row_values:
            try {
                Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = theNative.get_selection();
                Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
                if (iter) {
                    vector<Glib::ustring> myStrings;
                    convertFrom(cx, *vp, myStrings);

                    unsigned int myColCount = theNative.get_model()->get_n_columns();
                    if (myColCount != myStrings.size()+1) {
                        JS_ReportError(cx,"selected_row_values: expected %d strings, got %d",
                                myColCount-1, myStrings.size());
                        return JS_FALSE;
                    }
                    for (unsigned int i=1; i < myColCount; ++i) {
                        iter->set_value(i, myStrings[i-1]);
                    }
                }
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSTreeView::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSTreeView * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        myNewObject = new JSTreeView(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTreeView::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSTreeView::ConstIntProperties() {

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
JSTreeView::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSTreeView::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSTreeView::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSTreeView::OWNERPTR theOwner, JSTreeView::NATIVE * theNative) {
    JSObject * myReturnObject = JSTreeView::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

