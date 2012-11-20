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
#include "JSMessageDialog.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<Gtk::MessageDialog, asl::Ptr<Gtk::MessageDialog>, StaticAccessProtocol>;

typedef Gtk::MessageDialog NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSMessageDialog::getJSWrapper(cx,obj).getNative().get_title();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSMessageDialog::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSMessageDialog::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// property handling
JSBool
JSMessageDialog::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSMessageDialog::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSMessageDialog::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::getPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}
JSBool
JSMessageDialog::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JSBASE::setPropertySwitch(theNative, theID, cx, obj, id, vp);
    }
}


JSBool
JSMessageDialog::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    ensureParamCount(argc,1,5);

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSMessageDialog * myNewObject = 0;

    Glib::ustring myMessage;
    convertFrom(cx, argv[0], myMessage);

    bool myUseMarkupFlag = false;
    int  myMessageType = 0;
    int  myButtonsType = 0;
    bool myBeModalFlag = false;

    if (argc > 1) {
        convertFrom(cx, argv[1], myUseMarkupFlag);
    }
    if ( argc > 2) {
        convertFrom(cx, argv[2], myMessageType);
    }
    if ( argc > 3) {
        convertFrom(cx, argv[3], myButtonsType);
    }
    if ( argc > 4) {
        convertFrom(cx, argv[4], myBeModalFlag);
    }

    switch (argc) {
        case 1:
            newNative = new NATIVE(myMessage);
            break;
        case 2:
            newNative = new NATIVE(myMessage, myUseMarkupFlag);
            break;
        case 3:
            newNative = new NATIVE(myMessage, myUseMarkupFlag,
                    static_cast<Gtk::MessageType>(myMessageType));
            break;
        case 4:
            newNative = new NATIVE(myMessage, myUseMarkupFlag,
                    static_cast<Gtk::MessageType>(myMessageType),
                    static_cast<Gtk::ButtonsType>(myButtonsType));
            break;
        case 5:
            newNative = new NATIVE(myMessage, myUseMarkupFlag,
                    static_cast<Gtk::MessageType>(myMessageType),
                    static_cast<Gtk::ButtonsType>(myButtonsType),
                    myBeModalFlag);
            break;
    }
    myNewObject = new JSMessageDialog(OWNERPTR(newNative), newNative);
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSMessageDialog::Constructor: failed");
    return JS_FALSE;
}

#define DEFINE_MESSAGE_TYPE_PROP( NAME ) \
    {"MESSAGE_" # NAME, PROP_MESSAGE_TYPE_ ## NAME, Gtk::MESSAGE_ ## NAME}

#define DEFINE_BUTTONS_TYPE_PROP( NAME ) \
    {"BUTTONS_" # NAME, PROP_BUTTONS_TYPE_ ## NAME, Gtk::BUTTONS_ ## NAME}

JSConstIntPropertySpec *
JSMessageDialog::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_MESSAGE_TYPE_PROP( INFO ),
        DEFINE_MESSAGE_TYPE_PROP( WARNING ),
        DEFINE_MESSAGE_TYPE_PROP( QUESTION ),
        DEFINE_MESSAGE_TYPE_PROP( ERROR ),

        DEFINE_BUTTONS_TYPE_PROP( NONE ),
        DEFINE_BUTTONS_TYPE_PROP( OK ),
        DEFINE_BUTTONS_TYPE_PROP( CLOSE ),
        DEFINE_BUTTONS_TYPE_PROP( CANCEL ),
        DEFINE_BUTTONS_TYPE_PROP( YES_NO ),
        DEFINE_BUTTONS_TYPE_PROP( OK_CANCEL ),
        {0}
    };
    return myProperties;
};


void
JSMessageDialog::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSBASE::addClassProperties(cx, theClassProto);
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            ConstIntProperties(), 0, 0, JSBASE::ClassName());
}

JSObject *
JSMessageDialog::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSMessageDialog::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSMessageDialog::OWNERPTR theOwner, JSMessageDialog::NATIVE * theNative) {
    JSObject * myReturnObject = JSMessageDialog::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

