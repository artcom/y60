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
#include "JSStockID.h"

#include "jsgtk.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

#ifdef DELETE
    #undef DELETE
#endif

namespace jslib {

template class JSWrapper<Gtk::StockID, asl::Ptr<Gtk::StockID>, StaticAccessProtocol>;

typedef Gtk::Dialog NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSStockID::getJSWrapper(cx,obj).getNative().get_string();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSStockID::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSStockID::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}


JSConstIntPropertySpec *
JSStockID::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};


struct JSStockIdPropertySpec {
    const char * name;
    int8         tinyId;
    Gtk::StockID value;
};


#define DEFINE_STOCK_PROP( NAME ) { #NAME, PROP_STOCK_ ## NAME, Gtk::Stock::NAME}

JSStockIdPropertySpec *
JSStockID::StockIdProperties() {

    static JSStockIdPropertySpec myProperties[] = {
        DEFINE_STOCK_PROP( DIALOG_AUTHENTICATION ),

        DEFINE_STOCK_PROP( DIALOG_INFO ),
        DEFINE_STOCK_PROP( DIALOG_WARNING ),
        DEFINE_STOCK_PROP( DIALOG_ERROR ),
        DEFINE_STOCK_PROP( DIALOG_QUESTION ),

        // DS: From gtkmm/stock.h: These aren't real stock items,
        //     because they provide only an icon.
        DEFINE_STOCK_PROP( DND ),
        DEFINE_STOCK_PROP( DND_MULTIPLE ),

        DEFINE_STOCK_PROP( ADD ),
        DEFINE_STOCK_PROP( APPLY ),
        DEFINE_STOCK_PROP( BOLD ),
        DEFINE_STOCK_PROP( CANCEL ),
        DEFINE_STOCK_PROP( CDROM ),
        DEFINE_STOCK_PROP( CLEAR ),
        DEFINE_STOCK_PROP( CLOSE ),
        DEFINE_STOCK_PROP( CONVERT ),
        DEFINE_STOCK_PROP( COPY ),
        DEFINE_STOCK_PROP( CUT ),
        DEFINE_STOCK_PROP( DELETE ),
        DEFINE_STOCK_PROP( EXECUTE ),
        DEFINE_STOCK_PROP( FIND ),
        DEFINE_STOCK_PROP( FIND_AND_REPLACE ),
        DEFINE_STOCK_PROP( FLOPPY ),
        DEFINE_STOCK_PROP( GOTO_BOTTOM ),
        DEFINE_STOCK_PROP( GOTO_FIRST ),
        DEFINE_STOCK_PROP( GOTO_LAST ),
        DEFINE_STOCK_PROP( GOTO_TOP ),
        DEFINE_STOCK_PROP( GO_BACK ),
        DEFINE_STOCK_PROP( GO_DOWN ),
        DEFINE_STOCK_PROP( GO_FORWARD ),
        DEFINE_STOCK_PROP( GO_UP ),
        DEFINE_STOCK_PROP( HELP ),
        DEFINE_STOCK_PROP( HOME ),
        DEFINE_STOCK_PROP( INDEX ),
        DEFINE_STOCK_PROP( ITALIC ),
        DEFINE_STOCK_PROP( JUMP_TO ),
        DEFINE_STOCK_PROP( JUSTIFY_CENTER ),
        DEFINE_STOCK_PROP( JUSTIFY_FILL ),
        DEFINE_STOCK_PROP( JUSTIFY_LEFT ),
        DEFINE_STOCK_PROP( JUSTIFY_RIGHT ),
        DEFINE_STOCK_PROP( MISSING_IMAGE ),
        DEFINE_STOCK_PROP( NEW ),
        DEFINE_STOCK_PROP( NO ),
        DEFINE_STOCK_PROP( OK ),
        DEFINE_STOCK_PROP( OPEN ),
        DEFINE_STOCK_PROP( PASTE ),
        DEFINE_STOCK_PROP( PREFERENCES ),
        DEFINE_STOCK_PROP( PRINT ),
        DEFINE_STOCK_PROP( PRINT_PREVIEW ),
        DEFINE_STOCK_PROP( PROPERTIES ),
        DEFINE_STOCK_PROP( QUIT ),
        DEFINE_STOCK_PROP( REDO ),
        DEFINE_STOCK_PROP( REFRESH ),
        DEFINE_STOCK_PROP( REMOVE ),
        DEFINE_STOCK_PROP( REVERT_TO_SAVED ),
        DEFINE_STOCK_PROP( SAVE ),
        DEFINE_STOCK_PROP( SAVE_AS ),
        DEFINE_STOCK_PROP( SELECT_COLOR ),
        DEFINE_STOCK_PROP( SELECT_FONT ),
        DEFINE_STOCK_PROP( SORT_ASCENDING ),
        DEFINE_STOCK_PROP( SORT_DESCENDING ),
        DEFINE_STOCK_PROP( SPELL_CHECK ),
        DEFINE_STOCK_PROP( STOP ),
        DEFINE_STOCK_PROP( STRIKETHROUGH ),
        DEFINE_STOCK_PROP( UNDELETE ),
        DEFINE_STOCK_PROP( UNDERLINE ),
        DEFINE_STOCK_PROP( UNDO ),
        DEFINE_STOCK_PROP( YES ),
        DEFINE_STOCK_PROP( ZOOM_100 ),
        DEFINE_STOCK_PROP( ZOOM_FIT ),
        DEFINE_STOCK_PROP( ZOOM_IN ),
        DEFINE_STOCK_PROP( ZOOM_OUT ),
        {0}
    };
    return myProperties;
};


// property handling
JSBool
JSStockID::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSStockID::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSStockID::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JS_FALSE;
    }
}
JSBool
JSStockID::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case 0:
        default:
            return JS_FALSE;
    }
}


JSBool
JSStockID::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSStockID * myNewObject = 0;

    if (argc == 0) {
        newNative = new NATIVE();
        // newNative->set_events(Gdk::ALL_EVENTS_MASK);

        myNewObject = new JSStockID(OWNERPTR(newNative), newNative);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSStockID::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSStockID::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    //TODO missing stock id props here
    createClassModuleDocumentation("gtk", ClassName(), Properties(), Functions(),
            0, 0, 0, 0);
}

void
JSStockID::addStockIdProps(JSContext * cx, JSObject * obj, JSStockIdPropertySpec * cds) {
    JSBool myOkFlag;
    uintN flags = JSPROP_READONLY | JSPROP_PERMANENT;
    for (myOkFlag = JS_TRUE; cds->name; cds++) {
        myOkFlag = JS_DefineProperty(cx, obj, cds->name, as_jsval(cx, asl::Ptr<NATIVE>(),
                                     & cds->value), NULL, NULL, flags);
        if ( ! myOkFlag) {
            break;
        }
    }
}

JSObject *
JSStockID::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        addStockIdProps(cx, myConstructorFuncObj, StockIdProperties());
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSStockID::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}


jsval as_jsval(JSContext *cx, JSStockID::OWNERPTR theOwner, JSStockID::NATIVE * theNative) {

    //check if theNative is a builtin constant value like Gtk::Stock::OK

    JSObject * myReturnObject = JSStockID::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSStockID::NATIVE * & theStockId) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {

            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSStockID::NATIVE >::Class()) {
                typedef JSClassTraits<JSStockID::NATIVE>::ScopedNativeRef NativeRef;
                NativeRef myObj(cx, myArgument);
                theStockId = & myObj.getNative();

                return true;
            }
        }
    }
    return false;
}


}

