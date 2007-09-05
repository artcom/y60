
#include <string>

#include <y60/Image.h>

#include <y60/JScppUtils.h>
#include <y60/JSNode.h>

#include "JSCairoContext.h"

using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

using namespace Cairo;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Cairo::Context@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}


JSFunctionSpec *
JSCairoContext::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairoContext::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSCairoContext::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoContext::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoContext::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoContext::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoContext::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSCairoContext * myNewObject = 0;

    if (argc == 1) {
        dom::NodePtr myImageNode;
        if(!convertFrom(cx, argv[0], myImageNode)) {
            //            JS_ReportError("Need an image node to construct a cairo context.");
            return JS_FALSE;
        }

        ImagePtr myImage = myImageNode->getFacade<Image>();
        int myImageWidth = myImage->get<ImageWidthTag>();
        int myImageHeight = myImage->get<ImageHeightTag>();


        unsigned char *myDataPtr = myImage->getRasterPtr()->pixels().begin();

        cairo_surface_t *myCairoSurface =
            cairo_image_surface_create_for_data(myDataPtr, CAIRO_FORMAT_ARGB32, 
                                                myImageWidth, myImageHeight, myImageWidth);
        cairo_t *myCairoContext =
            cairo_create(myCairoSurface);

        newNative = new Cairo::Context(myCairoContext);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }
    myNewObject = new JSCairoContext(OWNERPTR(newNative), newNative);

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCairoContext::Constructor: bad parameters");
    return JS_FALSE;
}
/*
JSConstIntPropertySpec *
JSCairoContext::ConstIntProperties() {

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
JSCairoContext::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
JSCairoContext::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
//        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
//        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCairoContext::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCairoContext::OWNERPTR theOwner, JSCairoContext::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoContext::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}
