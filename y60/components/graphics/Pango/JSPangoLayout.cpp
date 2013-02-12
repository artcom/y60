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

#include <string>

#include <pango/pangocairo.h>
#include <glib-object.h>

#include <y60/image/Image.h>

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSVector.h>

#include <y60/jsbase/JSWrapper.impl>

#include <y60/components/graphics/Cairo/JSCairoSurface.h>

#include "JSPangoContext.h"
#include "JSPangoLayout.h"
#include "JSPangoFontDescription.h"


using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

////////////////////////////////////////////////////// Functions
static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);
        const char * myText = pango_layout_get_text(myOwner->get()->getLayout());
        std::string myStringRep = string("Pango.Layout: '"+string(myText)+"'");
        *rval = as_jsval(cx, myStringRep);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("set text color");
    DOC_PARAM("theColor", "text color", DOC_TYPE_VECTOR4F);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        Vector4f colorVector;
        if (!convertFrom(cx, argv[0], colorVector)) {
            JS_ReportError(cx, "JSPangoLayout::setColor(): argument #1 must be a Vector4f");
            return JS_FALSE;
        }
        cairo_set_source_rgba(myOwner->get()->getCairoContext(), 
                              colorVector[0],
                              colorVector[1],
                              colorVector[2],
                              colorVector[3]);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setBackground(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("set background color");
    DOC_PARAM("theColor", "background color", DOC_TYPE_VECTOR4F);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        Vector4f colorVector;
        if (!convertFrom(cx, argv[0], colorVector)) {
            JS_ReportError(cx, "JSPangoLayout::setBackground(): argument #1 must be a Vector4f");
            return JS_FALSE;
        }
        myOwner->get()->style.backgroundColor = colorVector;
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets text and returns dimensions of rendered text");
    DOC_PARAM("theText", "text to render", DOC_TYPE_STRING);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        cairo_t* cairo = myOwner->get()->getCairoContext();
        PangoLayout * layout = myOwner->get()->getLayout();

        ensureParamCount(argc, 1);
        std::string myText;
        if (!convertFrom(cx, argv[0], myText)) {
            JS_ReportError(cx, "JSPangoLayout::setText(): argument #1 must be a string");
            return JS_FALSE;
        }

        cairo_save (cairo);
        if (myOwner->get()->style.backgroundColor[3] > 0) {
            //clear canvas with background color
            cairo_set_operator (cairo, CAIRO_OPERATOR_SOURCE);
            Vector4f background = myOwner->get()->style.backgroundColor;
            cairo_set_source_rgba (cairo, background[0], background[1], 
                                          background[2], background[3]);
        } else {
            //clear canvas
            cairo_set_operator (cairo, CAIRO_OPERATOR_CLEAR);
        }
        cairo_paint (cairo);
        cairo_restore (cairo);

        //write new text
        //if there are no tags in text we could also use pango_layout_set_text
        pango_layout_set_markup(layout, myText.c_str(), myText.size());
        pango_cairo_update_layout(cairo, layout);
        pango_cairo_show_layout(cairo, layout);
        int width, height;
        pango_layout_get_pixel_size(layout, &width, &height);

        //return dimensions
        Vector2f dimensions(width, height);
        *rval = as_jsval(cx, dimensions);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the maximum text block width (wrap width). Default is '-1' which means no wrapping.");
    DOC_PARAM("theWidth", "text width", DOC_TYPE_INTEGER);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        int width;
        if (!convertFrom(cx, argv[0], width)) {
            JS_ReportError(cx, "JSPangoLayout::setWidth(): argument #1 must be a integer");
            return JS_FALSE;
        }
        width *= 1000; //convert from spark-pixel-unit to pango-unit

        PangoLayout *layout = myOwner->get()->getLayout();
        pango_layout_set_width(layout, width);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the maximum height of an text block. Negative and positive values allowed (resulting in diverse ellipsis behavior). See pango docs. Default is '-1'.");
    DOC_PARAM("theHeight", "text height", DOC_TYPE_INTEGER);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        int height;
        if (!convertFrom(cx, argv[0], height)) {
            JS_ReportError(cx, "JSPangoLayout::setHeight(): argument #1 must be a integer");
            return JS_FALSE;
        }
        height *= 1000; //convert from spark-pixel-unit to pango-unit

        PangoLayout *layout = myOwner->get()->getLayout();
        pango_layout_set_height(layout, height);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setIndent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the indentation of first line of each paragraph. May be negative.");
    DOC_PARAM("theIndent", "text indentation", DOC_TYPE_INTEGER);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        int indent;
        if (!convertFrom(cx, argv[0], indent)) {
            JS_ReportError(cx, "JSPangoLayout::setIndent(): argument #1 must be a integer");
            return JS_FALSE;
        }
        indent *= 1000; //convert from spark-pixel-unit to pango-unit

        PangoLayout *layout = myOwner->get()->getLayout();
        pango_layout_set_indent(layout, indent);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setSpacing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets (additional) spacing between lines in pixel and thus the lineheight. Negative values are possible.");
    DOC_PARAM("theSpacing", "text spacing", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        float spacing;
        if (!convertFrom(cx, argv[0], spacing)) {
            JS_ReportError(cx, "JSPangoLayout::setSpacing(): argument #1 must be a float");
            return JS_FALSE;
        }

        PangoLayout *layout = myOwner->get()->getLayout();
        pango_layout_set_spacing(layout, spacing * PANGO_SCALE);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setAlignment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets alignment. Parameter should one of 'left', 'center', 'right'.");
    DOC_PARAM("theAlignment", "text alignment", DOC_TYPE_STRING);
    DOC_END;
    try {
        pango::JSLayout::OWNERPTR myOwner;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

        ensureParamCount(argc, 1);
        std::string alignment;
        if (!convertFrom(cx, argv[0], alignment)) {
            JS_ReportError(cx, "JSPangoLayout::setAlignment(): argument #1 must be a string");
            return JS_FALSE;
        }

        PangoAlignment pangoAlignment = PANGO_ALIGN_LEFT;
        if (alignment == "right") {
            pangoAlignment = PANGO_ALIGN_RIGHT;
        } else if (alignment == "center") {
            pangoAlignment = PANGO_ALIGN_CENTER;
        }

        PangoLayout *layout = myOwner->get()->getLayout();
        pango_layout_set_alignment(layout, pangoAlignment);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
saveToPNG(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("save cairo surface to png");
    DOC_PARAM("theFilename", "filename of saved image", DOC_TYPE_STRING);
    DOC_END;

    pango::JSLayout::OWNERPTR myOwner;
    convertFrom(cx, OBJECT_TO_JSVAL(obj), myOwner);

    cairo_t* cairoContext = myOwner->get()->getCairoContext();
    cairo_surface_t *cairoSurface = cairo_get_target(cairoContext);
    cairo_status_t status = cairo_surface_write_to_png(cairoSurface, "pango_rendered.png");
    if (status == CAIRO_STATUS_SUCCESS) {
        return JS_TRUE;
    } 
    return JS_FALSE;
}

JSFunctionSpec *
pango::JSLayout::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"setColor",             setColor,                1},
        {"setBackground",        setBackground,           1},
        {"setText",              setText,                 1}, //returns dimensions
        {"setWidth",             setWidth,                1},
        {"setHeight",            setHeight,               1},
        {"setIndent",            setIndent,               1},
        {"setSpacing",           setSpacing,              1},
        {"setAlignment",         setAlignment,            1},
        {"saveToPNG",            saveToPNG,               0},
        {0}
    };
    return myFunctions;
}

/////////////////////////////////////////////////////// Properties
JSPropertySpec *
pango::JSLayout::Properties() {
    static JSPropertySpec myProperties[] = {
        {"font_description", PROP_font_description, JSPROP_ENUMERATE|JSPROP_PERMANENT},
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
        case PROP_font_description:
            try {
                // this getter returns a read-only, non-owned reference to the PangoFontDescription
                // since we can't wrap that, we'll return a copy
                PangoFontDescription * myCopy = pango_font_description_copy(pango_layout_get_font_description(theNative.get()->getLayout()));
                JSFontDescription::NATIVE * wrappedCopy = new JSFontDescription::NATIVE(myCopy);
                *vp = as_jsval(cx, JSFontDescription::OWNERPTR(wrappedCopy), wrappedCopy);
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_context:
            try {
                PangoContext * myContext = pango_layout_get_context(theNative.get()->getLayout());
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
        case PROP_font_description:
            try {
                pango::JSFontDescription::OWNERPTR fontDesc;
                convertFrom(cx, *vp, fontDesc);
                pango_layout_set_font_description(theNative.get()->getLayout(), fontDesc->get());
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            return JS_FALSE;
    }
}

/////////////////////////////////////////////////////// Constructor
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

    dom::NodePtr myImageNode;
    if (argc == 1) {

        if (convertFrom(cx, argv[0], myImageNode)) {

            cairo_surface_t *mySurface = cairo::JSSurface::createFromImageNode(myImageNode);
            cairo_t *cairoContext = cairo_create(mySurface);
            cairo_set_source_rgba(cairoContext, 1.0, 1.0, 1.0, 1.0);

            PangoLayout * newLayout = pango_cairo_create_layout(cairoContext);
            PangoCairo* pangoCairo = new PangoCairo(newLayout, cairoContext);
            newNative = new NATIVE(pangoCairo);
            cairo_surface_destroy(mySurface);
        } else {
            JS_ReportError(cx,"Constructor for %s: first argument must be a ImageNode",ClassName());
            return JS_FALSE;
        };

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected one, got %d",ClassName(), argc);
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
