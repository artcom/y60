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
#include "JSTable.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::Table, asl::Ptr<Gtk::Table>, StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Gtk::Table@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
Attach(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 5, 9);
        Gtk::Widget * myWidgetToAttach;
        unsigned int leftAttach;
        unsigned int rightAttach;
        unsigned int topAttach;
        unsigned int bottomAttach;

        if (!convertFrom(cx, argv[0], myWidgetToAttach)) {
            JS_ReportError(cx, "JSTable::attach(): argument #1 must be a widget");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], leftAttach)) {
            JS_ReportError(cx, "JSTable::attach(): argument #2 must be an unsigned int (left-attach)");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[2], rightAttach)) {
            JS_ReportError(cx, "JSTable::attach(): argument #3 must be an unsigned int (right-attach)");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[3], topAttach)) {
            JS_ReportError(cx, "JSTable::attach(): argument #4 must be an unsigned int (top-attach)");
            return JS_FALSE;
        }

        if (!convertFrom(cx, argv[4], bottomAttach)) {
            JS_ReportError(cx, "JSTable::attach(): argument #5 must be an unsigned int (bottom-attach)");
            return JS_FALSE;
        }

        int xOptions = 0; // Gtk::AttachOptions
        if (argc > 5) {
            if (!convertFrom(cx, argv[5], xOptions)) {
                JS_ReportError(cx, "JSTable::attach(): argument #5 must be a Gtk.AttachOptions ");
                return JS_FALSE;
            }
        }

        int yOptions = 0; // Gtk::AttachOptions
        if (argc > 6) {
            if (!convertFrom(cx, argv[6], yOptions)) {
                JS_ReportError(cx, "JSTable::attach(): argument #6 must be a Gtk.AttachOptions ");
                return JS_FALSE;
            }
        }

        unsigned int xPadding = 0;
        if (argc > 7) {
            if (!convertFrom(cx, argv[7], xPadding)) {
                JS_ReportError(cx, "JSTable::attach(): argument #7 must be an unsigned int ");
                return JS_FALSE;
            }
        }

        unsigned int yPadding = 0;
        if (argc > 8) {
            if (!convertFrom(cx, argv[8], yPadding)) {
                JS_ReportError(cx, "JSTable::attach(): argument #8 must be an unsigned int ");
                return JS_FALSE;
            }
        }

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);
        switch (argc) {
            case 5 :
                mySelf->attach(*myWidgetToAttach, leftAttach, rightAttach, topAttach, bottomAttach);
                break;
            case 6 :
                mySelf->attach(*myWidgetToAttach, leftAttach, rightAttach, topAttach, bottomAttach,
                        static_cast<Gtk::AttachOptions> (xOptions));
                break;
            case 7 :
                mySelf->attach(*myWidgetToAttach, leftAttach, rightAttach, topAttach, bottomAttach,
                        static_cast<Gtk::AttachOptions> (xOptions),
                        static_cast<Gtk::AttachOptions> (yOptions));
                break;
            case 8 :
                mySelf->attach(*myWidgetToAttach, leftAttach, rightAttach, topAttach, bottomAttach,
                        static_cast<Gtk::AttachOptions> (xOptions),
                        static_cast<Gtk::AttachOptions> (yOptions),
                        xPadding);
                break;
            case 9 :
                mySelf->attach(*myWidgetToAttach, leftAttach, rightAttach, topAttach, bottomAttach,
                        static_cast<Gtk::AttachOptions> (xOptions),
                        static_cast<Gtk::AttachOptions> (yOptions),
                        xPadding, yPadding);
                break;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

}


static JSBool
set_spacings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int mySpacings;
        convertFrom(cx, argv[0], mySpacings);

        mySelf->set_spacings(mySpacings);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_row_spacings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int mySpacings;
        convertFrom(cx, argv[0], mySpacings);

        mySelf->set_row_spacings(mySpacings);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_col_spacings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int mySpacings;
        convertFrom(cx, argv[0], mySpacings);

        mySelf->set_col_spacings(mySpacings);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_col_spacing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int myIndex;
        convertFrom(cx, argv[0], myIndex);

        int mySpacings;
        convertFrom(cx, argv[1], mySpacings);

        mySelf->set_col_spacing(myIndex, mySpacings);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
set_row_spacing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int myIndex;
        convertFrom(cx, argv[0], myIndex);

        int mySpacings;
        convertFrom(cx, argv[1], mySpacings);

        mySelf->set_row_spacing(myIndex, mySpacings);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
get_row_spacing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int myIndex;
        convertFrom(cx, argv[0], myIndex);

        int mySpacing =  mySelf->get_row_spacing(myIndex);
        *rval = as_jsval(cx, mySpacing);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
get_col_spacing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        int myIndex;
        convertFrom(cx, argv[0], myIndex);

        int mySpacing =  mySelf->get_col_spacing(myIndex);
        *rval = as_jsval(cx, mySpacing);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool
resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);

        Gtk::Table * mySelf = 0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), mySelf);

        guint myRows;
        convertFrom(cx, argv[0], myRows);

        guint myCols;
        convertFrom(cx, argv[1], myCols);

        mySelf->resize(myRows, myCols);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}


JSFunctionSpec *
JSTable::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"attach",               Attach,                  9},
        {"set_spacings",         set_spacings,            1},
        {"set_row_spacings",     set_row_spacings,        1},
        {"set_col_spacings",     set_col_spacings,        1},
        {"set_row_spacing",      set_row_spacing,         2},
        {"get_row_spacing",      get_row_spacing,         1},
        {"set_col_spacing",      set_col_spacing,         2},
        {"get_col_spacing",      get_col_spacing,         1},
        {"resize",               resize,                  2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTable::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSTable::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTable::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSTable::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSTable::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}

JSBool
JSTable::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSTable * myNewObject = 0;

    if (argc == 0) {
        newNative = new Gtk::Table();
    } else if (argc == 3) {
        int myRowCount;
        if ( ! convertFrom(cx, argv[0], myRowCount)) {
            JS_ReportError(cx,"Constructor for %s: Argument 0 must be an int",ClassName());
            return JS_FALSE;
        }
        int myColumnCount;
        if ( ! convertFrom(cx, argv[1], myColumnCount)) {
            JS_ReportError(cx,"Constructor for %s: Argument 1 must be an int",ClassName());
            return JS_FALSE;
        }
        bool myHomogeneousFlag;
        if ( ! convertFrom(cx, argv[2], myHomogeneousFlag)) {
            JS_ReportError(cx,"Constructor for %s: Argument 2 must be a bool",ClassName());
            return JS_FALSE;
        }
        newNative = new NATIVE(myRowCount, myColumnCount, myHomogeneousFlag);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none or three () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSTable(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTable::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSTable::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, JSBASE::ClassName());
}

JSObject *
JSTable::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSTable::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSTable::OWNERPTR theOwner, JSTable::NATIVE * theNative) {
    JSObject * myReturnObject = JSTable::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

