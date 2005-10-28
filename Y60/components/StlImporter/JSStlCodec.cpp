#include "JSStlCodec.h"

#include <asl/PackageManager.h>
#include <y60/JSNode.h>

using namespace asl;
using namespace y60;
using namespace std;
using namespace dom;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    const StlCodec & myNative = JSStlCodec::getJSWrapper(cx,obj).getNative();
    std::string myStringRep = string("StlCodec ");
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(), myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
exportShape(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSStlCodec::NATIVE>::call(&JSStlCodec::NATIVE::exportShape,cx,obj,argc,argv,rval);
}

static JSBool
exportShapes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSStlCodec::NATIVE>::call(&JSStlCodec::NATIVE::exportShapes,cx,obj,argc,argv,rval);
}

static JSBool
close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSStlCodec::NATIVE>::call(&JSStlCodec::NATIVE::close,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSStlCodec::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"exportShape",          exportShape,             1},
        {"exportShapes",         exportShapes,            2},
        {"close",                close,                   0},
        {0}
    };
    return myFunctions;
}
JSConstIntPropertySpec *
JSStlCodec::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};
JSPropertySpec *
JSStlCodec::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSStlCodec::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSStlCodec::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSStlCodec::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSStlCodec::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSStlCodec::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }
    ensureParamCount(argc, 2);
    OWNERPTR myNewNative;
    if (argc == 2) {
        std::string myFilename;
        bool myBigEndianFlag;
        convertFrom(cx, argv[0], myFilename);
        convertFrom(cx, argv[1], myBigEndianFlag);
        myNewNative = OWNERPTR(new StlCodec(myFilename, myBigEndianFlag));
    } else {
        JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected none or one "
            "(filepath), got %d", ClassName(), argc);
        return JS_FALSE;
    }

    JSStlCodec * myNewObject = new JSStlCodec(myNewNative, &(*myNewNative));
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSPropertySpec *
JSStlCodec::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSStlCodec::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}


JSObject *
JSStlCodec::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("components", JSStlCodec);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSStlCodec::OWNERPTR & theStlCodec) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSStlCodec::NATIVE >::Class()) {
                theStlCodec = JSClassTraits<JSStlCodec::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSStlCodec::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSStlCodec::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSStlCodec::OWNERPTR theOwner, JSStlCodec::NATIVE * theStlCodec) {
    JSObject * myObject = JSStlCodec::Construct(cx, theOwner, theStlCodec);
    return OBJECT_TO_JSVAL(myObject);
}


} // namespace jslib
