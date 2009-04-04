#include "custom_globals.h"

#include <y60/components/yape/js_value.h>
#include <y60/components/yape/module.h>

using namespace y60::ape;
static
JSBool
print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    for (uintN i = 0; i < argc; i++) {
        std::cout <<  (i ? " " : "") << value(cx, argv[i]).toString();
    }
    std::cout << std::endl;
    return JS_TRUE;
}

Y60_APE_MODULE_IMPL( custom_globals ) {
    Y60_APE_NS_SCOPE()
        . function("print", print, 0)
        ;
}

