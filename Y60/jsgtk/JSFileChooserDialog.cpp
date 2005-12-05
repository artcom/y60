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
//   $RCSfile: JSFileChooserDialog.cpp,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2005/04/21 16:25:02 $
//
//
//=============================================================================

#include "JSFileChooserDialog.h"
#include "jsgtk.h"
#include <y60/JScppUtils.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef Gtk::FileChooserDialog NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSFileChooserDialog::getJSWrapper(cx,obj).getNative().get_title();
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
Get_filename(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        // native method call
        Gtk::FileChooserDialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);
        string myRetVal = myNative->get_filename();
        *rval = as_jsval(cx, myRetVal);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
add_filter_pattern(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        Gtk::FileChooserDialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Glib::ustring myPattern;
        convertFrom(cx, argv[0], myPattern);

        Glib::ustring myName;
        convertFrom(cx, argv[1], myName);

        Gtk::FileFilter myFilter;
        myFilter.add_pattern( myPattern );
        myFilter.set_name( myName );

        myNative->add_filter( myFilter );
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
add_shortcut_folder(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    try {
        ensureParamCount(argc, 1);
        Gtk::FileChooserDialog * myNative=0;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        Glib::ustring myFolder;
        convertFrom(cx, argv[0], myFolder);

        myNative->add_shortcut_folder(myFolder);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}


JSFunctionSpec *
JSFileChooserDialog::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"get_filename",         Get_filename,            0},
        {"add_filter_pattern",   add_filter_pattern,      2},
        {"add_shortcut_folder",  add_shortcut_folder,     1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSFileChooserDialog::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSFileChooserDialog::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSFileChooserDialog::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSFileChooserDialog::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSFileChooserDialog::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}


JSBool
JSFileChooserDialog::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    ensureParamCount(argc,1,2);

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSFileChooserDialog * myNewObject = 0;

    string myTitle;
    convertFrom(cx, argv[0], myTitle);

    if (argc == 1) {
        newNative = new NATIVE(myTitle);
    } else {
        int myAction;
        convertFrom(cx, argv[1], myAction);
        newNative = new NATIVE(myTitle, (Gtk::FileChooserAction)myAction);
    }

    myNewObject = new JSFileChooserDialog(OWNERPTR(newNative), newNative);
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSFileChooserDialog::Constructor: failed");
    return JS_FALSE;
}

#define DEFINE_FILE_CHOOSER_ACTION_PROP( NAME ) \
    "ACTION_" # NAME, PROP_FILE_CHOOSER_ACTION_ ## NAME, Gtk::FILE_CHOOSER_ACTION_ ## NAME

JSConstIntPropertySpec *
JSFileChooserDialog::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_FILE_CHOOSER_ACTION_PROP( OPEN ),
        DEFINE_FILE_CHOOSER_ACTION_PROP( SAVE ),
        DEFINE_FILE_CHOOSER_ACTION_PROP( SELECT_FOLDER ),
        DEFINE_FILE_CHOOSER_ACTION_PROP( CREATE_FOLDER ),
#if 0
          "ACTION_OPEN", PROP_FILE_CHOOSER_ACTION_OPEN,
                Gtk::FILE_CHOOSER_ACTION_OPEN,
          "ACTION_SAVE", PROP_FILE_CHOOSER_ACTION_SAVE,
                Gtk::FILE_CHOOSER_ACTION_SAVE,
          "ACTION_SELECT_FOLDER", PROP_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER,
          "ACTION_CREATE_FOLDER", PROP_FILE_CHOOSER_ACTION_CREATE_FOLDER,
                Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER,
#endif
        {0}
    };
    return myProperties;
};


void
JSFileChooserDialog::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(), ConstIntProperties(), 0, 0, JSBASE::ClassName());
}

JSObject *
JSFileChooserDialog::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSFileChooserDialog::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSFileChooserDialog::OWNERPTR theOwner, JSFileChooserDialog::NATIVE * theNative) {
    JSObject * myReturnObject = JSFileChooserDialog::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

