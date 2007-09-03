
#include <iostream>

#include <js/jsapi.h>
#include <js/jsdbgapi.h>

#include "TuttleDebugger.h"

#define JS_HEAP_SIZE   (8*1024*1024)
#define JS_STACK_CHUNK (8*1024)

namespace tuttle {

    using namespace std;

    static JSClass tuttle_global_class = {
        "global", 0,
        JS_PropertyStub,  JS_PropertyStub,
        JS_PropertyStub,  JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,
        JS_ConvertStub,   JS_FinalizeStub
    };

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

    static JSTrapStatus
    tuttle_trap_throw (JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure) {
        Debugger *myDebugger = reinterpret_cast<Debugger*>(closure);
        cout << "Throw trap." << endl;
        return JSTRAP_CONTINUE;
    }

    static JSTrapStatus
    tuttle_trap_debugger (JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure) {
        Debugger *myDebugger = reinterpret_cast<Debugger*>(closure);
        cout << "Debugger trap." << endl;
        return JSTRAP_ERROR;
    }

    static void
    tuttle_report_error (JSContext *cx, const char *message, JSErrorReport *report) {
        if(!report) {
            cout << message;
            return;
        }
        
        if(report->filename) {
            cout << report->filename << ":";
        }

        if(report->lineno) {
            cout << report->lineno << ": ";
        }
        
        if(JSREPORT_IS_WARNING(report->flags)) {
            if(JSREPORT_IS_STRICT(report->flags)) {
                cout << "strict ";
            }
            cout << "warning: ";
        }
        
        cout << message << endl;
        
        if(report->linebuf) {
            int buflen = strlen(report->linebuf);
            
            cout << report->linebuf;
            
            if(report->linebuf[buflen-1] != '\n')
                cout << endl;
            
            const char *curchar;
            for(curchar = report->linebuf; curchar <= report->tokenptr; curchar++) {
                if(*curchar == '\t') {
                    cout << "        ";
                } else if (*curchar == ' ') {
                    cout << ' ';
                } else if (curchar == report->tokenptr) {
                    cout << '^';
                }
            }

            cout << endl;
        }
    }

    Debugger::Debugger() {
        JSRuntime *myRuntime = JS_NewRuntime(JS_HEAP_SIZE);        
        JS_SetThrowHook       (myRuntime, tuttle_trap_throw,    reinterpret_cast<void*>(this));
        JS_SetDebuggerHandler (myRuntime, tuttle_trap_debugger, reinterpret_cast<void*>(this));

        JSContext *myContext = JS_NewContext(myRuntime, JS_STACK_CHUNK);
        JS_SetErrorReporter(myContext, tuttle_report_error);

        JS_ToggleOptions(myContext, JSOPTION_STRICT);
        
        JSObject  *myGlobal  = JS_NewObject(myContext, &tuttle_global_class, NULL, NULL);
        
        JS_InitStandardClasses(myContext, myGlobal);

        JS_DefineFunctions(myContext, myGlobal, tuttle_global_functions);

        _myContext = myContext;
        _myGlobal  = myGlobal;

    }

    JSContext *Debugger::getContext() {
        return _myContext;
    }

    JSObject *Debugger::getGlobal() {
        return _myGlobal;
    }

}
