
#include "Tuttle.h"

#include <stdlib.h>

#include <cassert>
#include <iostream>
#include <iterator>

#include <js/jsfun.h>
#include <js/jsatom.h>
#include <js/jsinterp.h>
#include <js/jsdbgapi.h>

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

// default constructor
//
// creates a fresh javascript runtime/context/global.
//
Tuttle::Tuttle() :
    _myCurrentContext(NULL)
{
    JSRuntime *myRuntime = JS_NewRuntime(JS_HEAP_SIZE);
    JSContext *myContext = JS_NewContext(myRuntime, JS_STACK_CHUNK);
    JSObject  *myGlobal  = JS_NewObject(myContext, &global_class, NULL, NULL);

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


// evaluate an expression in-frame.
bool_t Tuttle::evaluate(const clish_shell_t *theShell, const lub_argv_t *theArguments) {
    const char *myCode       = lub_argv__get_arg(theArguments, 0);
    const int   myCodeLength = strlen(myCode);

    JSContext    *myContext = _myCurrentContext;
    //    JSStackFrame *myFrame   = NULL;

    jsval rval;

    //    JS_FrameIterator(myContext, &myFrame);

    //    if(myFrame) {
    //        cout << "In-frame evaluation not implemented.\n";
    //    } else {
    if(!JS_EvaluateScript(myContext, _myGlobal, myCode, myCodeLength, "interactor", 1, &rval))
        cout << "ERROR!" << endl;
       
    //    }

        Print(myContext, NULL, 1, &rval, NULL);

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

    if(n >= (int)_myContexts.size()) {
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
