#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSWrapper.impl>

#include "bench_functions.h"

namespace jslib {

using namespace y60::ape::bench;

static JSBool
RawAdd(JSContext * cx, JSObject * obj,uintN argc, jsval * argv, jsval * rval) {
    if (argc != 2) {
        return JS_FALSE;
    }
    int a;
    if ( ! jslib::convertFrom(cx, argv[0], a) ) {
        return JS_FALSE;
    }
    int b;
    if ( ! jslib::convertFrom(cx, argv[1], b) ) {
        return JS_FALSE;
    }
    *rval = jslib::as_jsval(cx, add(a,b));
    return JS_TRUE;
}

static JSBool
RawAdd8(JSContext * cx, JSObject * obj,uintN argc, jsval * argv, jsval * rval) {
    if (argc != 8) {
        return JS_FALSE;
    }
    int a1(0);
    if ( ! jslib::convertFrom(cx, argv[0], a1) ) {
        return JS_FALSE;
    }
    int a2(0);
    if ( ! jslib::convertFrom(cx, argv[1], a2) ) {
        return JS_FALSE;
    }
    int a3(0);
    if ( ! jslib::convertFrom(cx, argv[2], a2) ) {
        return JS_FALSE;
    }
    int a4(0);
    if ( ! jslib::convertFrom(cx, argv[3], a2) ) {
        return JS_FALSE;
    }
    int a5(0);
    if ( ! jslib::convertFrom(cx, argv[4], a2) ) {
        return JS_FALSE;
    }
    int a6(0);
    if ( ! jslib::convertFrom(cx, argv[5], a2) ) {
        return JS_FALSE;
    }
    int a7(0);
    if ( ! jslib::convertFrom(cx, argv[6], a2) ) {
        return JS_FALSE;
    }
    int a8(0);
    if ( ! jslib::convertFrom(cx, argv[7], a2) ) {
        return JS_FALSE;
    }
    *rval = jslib::as_jsval(cx, add8(a1,a2,a3,a4,a5,a6,a7,a8));
    return JS_TRUE;
}

JSFunctionSpec * free_functions() {
    static JSFunctionSpec specs[] = {
        {"raw_add", RawAdd, 2},
        {"raw_add8", RawAdd8, 8},
        {0},
    };
    return specs;
}

class JSClassicAdder : public jslib::JSWrapper<adder, asl::Ptr<adder>,
                                                jslib::StaticAccessProtocol>
{
        JSClassicAdder() {};
    public:
        typedef adder NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSClassicAdder(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSClassicAdder(){};

        static const char * ClassName() {
            return "ClassicAdder";
        }
        static JSFunctionSpec * Functions();

        static JSPropertySpec * Properties();

        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSClassicAdder & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSClassicAdder &>(JSClassicAdder::getJSWrapper(cx,obj));
        }
};

static JSBool
ClassicAdderSet(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN(""); DOC_END;
    return Method<JSClassicAdder::NATIVE>::call(&JSClassicAdder::NATIVE::set,cx,obj,argc,argv,rval);
}

static JSBool
ClassicAdderAdd(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN(""); DOC_END;
    return Method<JSClassicAdder::NATIVE>::call(&JSClassicAdder::NATIVE::add,cx,obj,argc,argv,rval);
}


static JSBool
ClassicAdderAdd8(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN(""); DOC_END;
    return Method<JSClassicAdder::NATIVE>::call(&JSClassicAdder::NATIVE::add8,cx,obj,argc,argv,rval);
}


JSFunctionSpec *
JSClassicAdder::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"set",                  ClassicAdderSet,                 1},
        {"add",                  ClassicAdderAdd,            2},
        {"add8",                  ClassicAdderAdd8,            8},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSClassicAdder::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSClassicAdder::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSClassicAdder::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSConstIntPropertySpec *
JSClassicAdder::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSClassicAdder::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

// setproperty handling
JSBool
JSClassicAdder::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

JSBool
JSClassicAdder::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    JSClassicAdder * myNewObject = 0;
        OWNERPTR myNative = OWNERPTR(new adder());
        myNewObject = new JSClassicAdder(myNative, myNative.get());

    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}


JSObject *
JSClassicAdder::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("Components", JSClassicAdder);
    return myClass;
}

template <>
struct JSClassTraits<JSClassicAdder::NATIVE>
    : public JSClassTraitsWrapper<JSClassicAdder::NATIVE , JSClassicAdder> {};


bool convertFrom(JSContext *cx, jsval theValue, JSClassicAdder::OWNERPTR & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSClassicAdder::NATIVE>::Class()) {
                theNativePtr = JSClassTraits<JSClassicAdder::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, JSClassicAdder::NATIVE & theNative) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSClassicAdder::NATIVE >::Class()) {
                theNative = JSClassTraits<JSClassicAdder::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSClassicAdder::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSClassicAdder::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSClassicAdder::OWNERPTR theOwner, JSClassicAdder::NATIVE * theSerial) {
    JSObject * myObject = JSClassicAdder::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

} // end of namespace jslib









namespace y60 {
    class ape_bench_classic_binding : public asl::PlugInBase, public jslib::IScriptablePlugin {
        public:
            ape_bench_classic_binding(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

            virtual void initClasses(JSContext * theContext, JSObject *theGlobalObject) {
                jslib::JSClassicAdder::initClass( theContext, theGlobalObject );
                JS_DefineFunctions(theContext, theGlobalObject, jslib::free_functions() );
            }

            const char * ClassName() {
                static const char * myClassName = "ape_bench_classic_binding";
                return myClassName;
            }
    };
}

extern "C"
EXPORT asl::PlugInBase * ape_bench_classic_binding_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::ape_bench_classic_binding(myDLHandle);
}
