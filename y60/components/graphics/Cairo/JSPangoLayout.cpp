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

#include <string>

#include <y60/image/Image.h>

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSVector.h>

#include <y60/jsbase/JSWrapper.impl>

#include <glib-object.h>

#include "JSCairoContext.h"
#include "JSPangoContext.h"
#include "JSPangoLayout.h"
#include "JSPangoFontDescription.h"

#include <pango/pangocairo.h>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    pango::JSLayout::OWNERPTR myOwner;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);
    const char * theText = pango_layout_get_text(myOwner->get());
    std::string myStringRep = string("Pango.Layout: '"+string(theText)+"'");
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
update_from_cairo_context(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, PLUS_FILE_LINE);

        cairo_t * cairoContext;
        if (!convertFrom(cx, argv[0], cairoContext)) {
            JS_ReportError(cx,"update_from_cairo_context - argument must be a Cairo.Context");

        }
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);
        pango_cairo_update_layout(cairoContext, myOwner->get());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
show_in_cairo_context(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        ensureParamCount(argc, 1, PLUS_FILE_LINE);

        cairo_t * cairoContext;
        if (!convertFrom(cx, argv[0], cairoContext)) {
            JS_ReportError(cx,"show_in_cairo_context - argument must be a Cairo.Context");

        }
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);
        pango_cairo_show_layout(cairoContext, myOwner->get());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
pango::JSLayout::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"update_from_cairo_context",  update_from_cairo_context, 1},
        {"show_in_cairo_context",      show_in_cairo_context,     1},

        {0}
    };
    return myFunctions;
}

JSPropertySpec *
pango::JSLayout::Properties() {
    static JSPropertySpec myProperties[] = {
        {"font_description", PROP_font_description, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"text", PROP_text, JSPROP_ENUMERATE|JSPROP_PERMANENT},
        {"context", PROP_context, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY},
        {0}
    };
    return myProperties;
}

JSBool
pango::JSLayout::getPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
pango::JSLayout::setPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
pango::JSLayout::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        ::JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_text:
            *vp = as_jsval(cx, pango_layout_get_text(theNative.get()));
            return JS_TRUE;
        case PROP_font_description:
            try {
                // this getter returns a read-only, non-owned reference to the PangoFontDescription
                // since we can't wrap that, we'll return a copy
                PangoFontDescription * myCopy = pango_font_description_copy(pango_layout_get_font_description(theNative.get()));
                JSFontDescription::NATIVE * wrappedCopy = new JSFontDescription::NATIVE(myCopy);
                *vp = as_jsval(cx, JSFontDescription::OWNERPTR(wrappedCopy), wrappedCopy);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_context:
            try {
                PangoContext * myContext = pango_layout_get_context(theNative.get());
                // pango_layout_get_context doesn't inc the refcount, so we have to (pass true) 
                pango::JSContext::NATIVE * wrappedContext = new pango::JSContext::NATIVE(myContext, true);
                *vp = as_jsval(cx, pango::JSContext::OWNERPTR(wrappedContext), wrappedContext);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
    }
    return JS_FALSE;
}

JSBool
pango::JSLayout::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        ::JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    switch (theID) {
        case PROP_text:
            try {
                string theText;
                convertFrom(cx, *vp, theText);
                pango_layout_set_text(theNative.get(), theText.c_str(), theText.size());
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_font_description:
            try {
                pango::JSFontDescription::OWNERPTR fontDesc;
                convertFrom(cx, *vp, fontDesc);
                pango_layout_set_font_description(theNative.get(), fontDesc->get());
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JS_FALSE;
    }
}

JSBool
pango::JSLayout::Constructor(::JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    pango::JSLayout * myNewObject = 0;
    NATIVE * newNative = 0;

    if (argc == 1) {
        cairo_t * cairoContext;
        if (convertFrom(cx, argv[0], cairoContext)) {
            PangoLayout * newLayout = pango_cairo_create_layout(cairoContext);
            newNative = new NATIVE(newLayout);
        } else {
            JS_ReportError(cx,"Constructor for %s: first argument must be a Cairo.Context",ClassName());
            return JS_FALSE;
        };

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    myNewObject = new pango::JSLayout(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"pango::JSLayout::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
pango::JSLayout::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value
        {0}
    };
    return myProperties;
};

void
pango::JSLayout::addClassProperties(::JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("Cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
pango::JSLayout::initClass(::JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "pango::JSLayout::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(::JSContext *cx, pango::JSLayout::OWNERPTR theOwner) {
    JSObject * myReturnObject = pango::JSLayout::Construct(cx, theOwner, &*theOwner);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(::JSContext *cx, jsval theValue, pango::JSLayout::OWNERPTR & theOwner) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<pango::JSLayout::NATIVE>::Class()) {
                theOwner = pango::JSLayout::getJSWrapper(cx, myArgument).getOwner();
                return true;
            }
        }
    }
    return false;
}

}
