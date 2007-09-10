#include <string>

#include <y60/Image.h>

#include <y60/JScppUtils.h>
#include <y60/JSNode.h>
#include <y60/JSVector.h>

#include <y60/JSWrapper.impl>

#include "JSCairoSurface.h"

using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

namespace jslib {

template class JSWrapper<Cairo::RefPtr<Cairo::Surface>, Ptr< Cairo::RefPtr<Cairo::Surface> >, StaticAccessProtocol>;

static JSBool
checkForErrors(JSContext *theJavascriptSurface, Cairo::RefPtr<Cairo::Surface> *theSurface) {
    Cairo::ErrorStatus myStatus = (*theSurface)->get_status();
    if(myStatus != CAIRO_STATUS_SUCCESS) {
        JS_ReportError(theJavascriptSurface, "cairo error: %s", cairo_status_to_string(myStatus));
        return JS_FALSE;
    }
    return JS_TRUE;
}

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Cairo::RefPtr<Cairo::Surface>@") + as_string(obj);
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

// XXX hack to allow triggering texture upload -ingo
static JSBool
triggerUpload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    ensureParamCount(argc, 0);

    JSCairoSurface *mySurface = reinterpret_cast<JSCairoSurface*>(JS_GetPrivate(cx, obj));

    mySurface->doTriggerUpload();

    return JS_TRUE;
}

// XXX hack to allow triggering texture upload -ingo
void
JSCairoSurface::doTriggerUpload() {
    _myImageNode->getFacade<y60::Image>()->triggerUpload();
}

JSFunctionSpec *
JSCairoSurface::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},

        // XXX hack to allow triggering texture upload -ingo
        {"triggerUpload",        triggerUpload,           0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSCairoSurface::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSBool
JSCairoSurface::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return getPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoSurface::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    return setPropertySwitch(myObj.getNative(), theID, cx, obj, id, vp);
}

JSBool
JSCairoSurface::getPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoSurface::setPropertySwitch(NATIVE & theNative, unsigned long theID,
        JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_FALSE;
}

JSBool
JSCairoSurface::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    NATIVE * newNative = 0;

    JSCairoSurface * myNewObject = 0;

    dom::NodePtr myImageNode;

    if (argc == 1) {

        if(!convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "Need an image node to construct a cairo context.");
            return JS_FALSE;
        }

        ImagePtr myImage = myImageNode->getFacade<Image>();
        int myImageWidth = myImage->get<ImageWidthTag>();
        int myImageHeight = myImage->get<ImageHeightTag>();

        unsigned char *myDataPtr = myImage->getRasterPtr()->pixels().begin();

        cairo_surface_t *myCairoSurface =
            cairo_image_surface_create_for_data(myDataPtr, CAIRO_FORMAT_ARGB32, 
                                                myImageWidth, myImageHeight, myImageWidth*4);

        Cairo::Surface *myCairommSurface = new Cairo::Surface(myCairoSurface);

        newNative = new Cairo::RefPtr<Cairo::Surface>(myCairommSurface);

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected none () %d",ClassName(), argc);
        return JS_FALSE;
    }

    myNewObject = new JSCairoSurface(OWNERPTR(newNative), newNative);
    myNewObject->_myImageNode = myImageNode;

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);

        return JS_TRUE;
    }
    JS_ReportError(cx,"JSCairoSurface::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSCairoSurface::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        // name                id                       value

        {0}
    };
    return myProperties;
};

void
JSCairoSurface::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
    JSA_AddProperties(cx, theClassProto, Properties());
    createClassModuleDocumentation("cairo", ClassName(), Properties(), Functions(), 0, 0, 0);
}

JSObject *
JSCairoSurface::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClassObject = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, 0 ,0);
    if (myClassObject) {
        addClassProperties(cx, myClassObject);
    }
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    } else {
        cerr << "JSCairoSurface::initClass: constructor function object not found, could not initialize static members"<<endl;
    }
    return myClassObject;
}

jsval as_jsval(JSContext *cx, JSCairoSurface::OWNERPTR theOwner, JSCairoSurface::NATIVE * theNative) {
    JSObject * myReturnObject = JSCairoSurface::Construct(cx, theOwner, theNative);
    return OBJECT_TO_JSVAL(myReturnObject);
}

bool convertFrom(JSContext *cx, jsval theValue, JSCairoSurface::NATIVE *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCairoSurface::NATIVE>::Class()) {
                JSClassTraits<JSCairoSurface::NATIVE>::ScopedNativeRef myObj(cx, myArgument);
                theTarget = &myObj.getNative();
                return true;
            }
        }
    }
    return false;
}

}
