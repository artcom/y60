
#include <iostream>

#include <js/spidermonkey/jsapi.h>
#include <js/spidermonkey/jsdbgapi.h>

#include "TuttleApplication.h"
#include "TuttleDebugger.h"
#include "TuttleCommand.h"


namespace tuttle {

    using namespace std;


    static JSTrapStatus tuttle_trap_throw (JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure);
    static JSTrapStatus tuttle_trap_debugger (JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure);
    static void         tuttle_report_error (JSContext *cx, const char *message, JSErrorReport *report);

    Debugger::Debugger(Application &theApplication) :
    _myApplication(theApplication) {

        _myApplication.attachDebugger(this);

        _myRuntime = _myApplication.getJavascriptRuntime();
        _myContext = _myApplication.getJavascriptContext();
        _myGlobal  = _myApplication.getJavascriptGlobal();

        JS_SetThrowHook       (_myRuntime, tuttle_trap_throw,    reinterpret_cast<void*>(this));
        JS_SetDebuggerHandler (_myRuntime, tuttle_trap_debugger, reinterpret_cast<void*>(this));

        JS_SetErrorReporter(_myContext, tuttle_report_error);

    }

    JSRuntime *Debugger::getJavascriptRuntime() {
        return _myRuntime;
    }

    JSContext *Debugger::getJavascriptContext() {
        return _myContext;
    }

    JSObject  *Debugger::getJavascriptGlobal() {
        return _myGlobal;
    }

    static JSTrapStatus
    tuttle_trap_break (JSContext *theContext, JSScript *theScript, jsbytecode *pc, jsval *rval, void *closure) {
        Debugger *myDebugger = reinterpret_cast<Debugger*>(closure);
        cout << "Breakpoint trap." << endl;
        return JSTRAP_CONTINUE;
    }

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

            if(report->lineno) {
                cout << report->lineno << ":";
            }

            cout << " ";
        } else {
            cout << "<unknown location>: ";
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

    void Debugger::executeCommands() {
        JSTrapStatus myDummy;
        bool myWait = false;
    while(myWait || !queueEmpty()) {
        Command *myCommand = queuePop();
        myWait = myCommand->execute(&myDummy);
            _myResultSemaphore.post();
    }
    }

    bool Debugger::queueEmpty() {
        bool myEmpty;
        _myQueueLock.lock();
        myEmpty = _myQueue.empty();
        _myQueueLock.unlock();
        return myEmpty;
    }

    Command *Debugger::queuePop() {
        Command *myCommand;
        _myRequestSemaphore.wait();
        _myQueueLock.lock();
        myCommand = _myQueue.front();
        _myQueue.pop();
        _myQueueLock.unlock();
        return myCommand;
    }

    void Debugger::queuePush(Command *theCommand) {
        _myQueueLock.lock();
        _myQueue.push(theCommand);
        _myQueueLock.unlock();
        _myRequestSemaphore.post();
    }

    void Debugger::execute(Command *theCommand) {
        queuePush(theCommand);
        _myResultSemaphore.wait();
    }

}
