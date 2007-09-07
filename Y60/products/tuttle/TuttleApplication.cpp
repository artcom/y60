
#include "TuttleApplication.h"

#define JS_HEAP_SIZE   (8*1024*1024)
#define JS_STACK_CHUNK (8*1024)

namespace tuttle {


    // XXX: get rid of this
    static JSBool
    Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        uintN i, n;
        JSString *str;
        
        for (i = n = 0; i < argc; i++) {
            str = JS_ValueToString(cx, argv[i]);
            if (!str)
                return JS_FALSE;
            fprintf(stdout, "%s%s", i ? " " : "", JS_GetStringBytes(str));
        }
        n++;
        if (n)
            fputc('\n', stdout);
        return JS_TRUE;
    }

    static JSFunctionSpec tuttle_global_functions[] = {
        {"print", Print, 0},
        {0}
    };

    static JSClass tuttle_global_class = {
        "global", 0,
        JS_PropertyStub,  JS_PropertyStub,
        JS_PropertyStub,  JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,
        JS_ConvertStub,   JS_FinalizeStub
    };


    Application::Application() :
        _myTerminate(false), _myInterval(100000), _myDebugger(0) {

        JSRuntime *myRuntime = JS_NewRuntime(JS_HEAP_SIZE);        

        JSContext *myContext = JS_NewContext(myRuntime, JS_STACK_CHUNK);
        JS_ToggleOptions(myContext, JSOPTION_STRICT);
        
        JSObject  *myGlobal  = JS_NewObject(myContext, &tuttle_global_class, NULL, NULL);
        
        JS_InitStandardClasses(myContext, myGlobal);

        JS_DefineFunctions(myContext, myGlobal, tuttle_global_functions);

        _myRuntime = myRuntime;
        _myContext = myContext;
        _myGlobal  = myGlobal;
    }

    bool Application::run() {
        while(!_myTerminate) {
            
            if(!handleRequests())
                goto fail;

            if(!process())
                goto fail;
            
            usleep(_myInterval);
        }

        return true;

    fail:
        return false;
    }

    bool Application::handleRequests() {
        return true;
    }

    bool Application::process() {
        return true;
    }

    void Application::terminate() {
        _myTerminate = true;
    }

#ifdef DEBUG
    void Application::attachDebugger(Debugger *theDebugger) {
        _myDebugger = theDebugger;
    }
#endif

    JSRuntime *Application::getJavascriptRuntime() {
        return _myRuntime;
    }

    JSContext *Application::getJavascriptContext() {
        return _myContext;
    }

    JSObject  *Application::getJavascriptGlobal() {
        return _myGlobal;
    }

}

