
#include "Tuttle.h"

#include <stdlib.h>

#include <cassert>
#include <iostream>
#include <iterator>

#include <js/jsapi.h>
#include <js/jsfun.h>
#include <js/jsatom.h>
#include <js/jscntxt.h>
#include <js/jsinterp.h>
#include <js/jsdbgapi.h>

#include <lub/string.h>

using namespace std;
using namespace tuttle;

#define ZERO 0

// global parameters for spidermonkey
#define JS_HEAP_SIZE   (8*1024*1024)
#define JS_STACK_CHUNK (8*1024)

// class for the global object
static JSClass global_class = {
    "global", ZERO,
    JS_PropertyStub,  JS_PropertyStub,
    JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub,   JS_FinalizeStub
};

// XXX: stolen from js.c and not yet adapted.
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

// functions to define in the global object
static JSFunctionSpec global_functions[] = {
    {"print", Print, 0},
    {0}
};

void
ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report) {
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
    } else {
        cout << endl;
    }

}

static JSTrapStatus
DebuggerTrap(JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure) {
    cout << "Debugger trap. Failing." << endl;
    return JSTRAP_ERROR;
}

static JSTrapStatus
ThrowTrap(JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure) {
    cout << "Throw trap. Failing." << endl;
    return JSTRAP_ERROR;
}


// default constructor
//
// creates a fresh javascript runtime/context/global.
//
Tuttle::Tuttle() :
    _myCurrentContext(NULL)
{
    JSRuntime *myRuntime = JS_NewRuntime(JS_HEAP_SIZE);
    JSContext *myContext = JS_NewContext(myRuntime, JS_STACK_CHUNK);
    JS_ToggleOptions(myContext, JSOPTION_STRICT);
    JS_SetErrorReporter(myContext, ErrorReporter);

    JS_SetThrowHook(myRuntime, ThrowTrap, NULL);
    JS_SetDebuggerHandler(myRuntime, DebuggerTrap, NULL);

    JSObject  *myGlobal  = JS_NewObject(myContext, &global_class, NULL, NULL);

    JS_InitStandardClasses(myContext, myGlobal);
    JS_DefineFunctions(myContext, myGlobal, global_functions);

    _myRuntime = myRuntime;
    _myGlobal  = myGlobal;

    registerContext(myContext);
}


// registers a javascript context with this tuttle.  
void Tuttle::registerContext(JSContext *theContext) {
    _myContexts.push_back(theContext);
        
    if(_myCurrentContext == NULL) {
        _myCurrentContext = theContext;
    }
}


bool_t Tuttle::print(const clish_shell_t *theShell, const lub_argv_t *theArguments) {
    bool_t result = BOOL_TRUE;
    char *myCode       = lub_string_dup(lub_argv__get_arg(theArguments, 0));
    const int   myCodeLength = strlen(myCode);

    unsigned i = 0;
    for(i = 0; i < lub_argv__get_count(theArguments); i++) {
        cout << i << ": " << lub_argv__get_arg(theArguments, i) << endl;
    }

    JSContext    *myContext = _myCurrentContext;
    jsval rval, prval;

    cout << myCode << endl;

    if(!JS_EvaluateScript(myContext, _myGlobal, myCode, myCodeLength, "interactor", 1, &rval)) {
        result = BOOL_FALSE;
        goto out;
    } else {
        Print(myContext, _myGlobal, 1, &rval, &prval);
    }

 out:
    if(myCode)
        lub_string_free(myCode);
    return result;
}

bool_t Tuttle::load(const clish_shell_t *theShell, const lub_argv_t *theArguments) {
    const char *myFile = lub_argv__get_arg(theArguments, 0);
    JSContext    *myContext = _myCurrentContext;
    JSObject *myGlobal = _myGlobal;
    JSScript *myScript;
    jsval result;
    myScript = JS_CompileFile(myContext, myGlobal, myFile);
    if (myScript) {
        (void)JS_ExecuteScript(myContext, myGlobal, myScript, &result);
        JS_DestroyScript(myContext, myScript);
    } else {
        cout << "Could not compile." << endl;
    }

    return BOOL_TRUE;
}

// list all contexts registered with this tuttle.
bool_t Tuttle::listContexts(const clish_shell_t *theShell, const lub_argv_t *theArguments) {

    int n;
    vector<JSContext *>::iterator i;
    for(i = _myContexts.begin(), n = 0; i != _myContexts.end(); i++, n++) {
        JSContext *myContext = *i;
        cout << n << ": " << reinterpret_cast<void*>(myContext) << endl;
    }

    return BOOL_TRUE;
}

bool_t Tuttle::setContext(const clish_shell_t *theShell, const lub_argv_t *theArguments) {
    int n = atoi(lub_argv__get_arg(theArguments, 0));

    if((int)_myContexts.size() >= n || n < 0) {
        cout << "No such context." << endl;
        return BOOL_FALSE;
    }

    _myCurrentContext = _myContexts[n];
    
    return BOOL_TRUE;
}

bool_t Tuttle::showContext(const clish_shell_t *theShell, const lub_argv_t *theArguments) {
    
    if(_myCurrentContext) {
      
        JSContext    *myContext     = _myCurrentContext;
        JSStackFrame *myFrame       = NULL;
        int           myFrameNumber = 0;

        JS_FrameIterator(myContext, &myFrame);

        if(myFrame) {
            cout << "Backtrace:\n";
        }

        for(; myFrame; JS_FrameIterator(myContext, &myFrame), myFrameNumber++) {
            JSFunction *myFunction     = myFrame->fun;
            JSAtom     *myFunctionName = myFunction->atom;

            cout << " #" << myFrameNumber << " " << myFrame->pc;
        
            if(myFunctionName) {
                assert(ATOM_IS_STRING(myFunctionName));
                cout << " in " << ATOM_TO_STRING(myFunctionName);
            } else {
                cout << " in <anonymous>";
            }

            cout << endl;
        }
      
    } else {
        cout << "No context selected." << endl;
    }

    return BOOL_TRUE;
}

bool_t Tuttle::trace(const clish_shell_t *theShell, const lub_argv_t *theArguments) {
    const char *arg = lub_argv__get_arg(theArguments, 0);
    
    if(0 == strcmp(arg, "on")) {
        _myCurrentContext->tracefp = stderr;
    }
    if(0 == strcmp(arg, "off")) {
        _myCurrentContext->tracefp = NULL;
    }
    
    return BOOL_TRUE;
}
