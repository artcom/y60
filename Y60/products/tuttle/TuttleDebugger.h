
#ifndef _TUTTLE_DEBUGGER_H_
#define _TUTTLE_DEBUGGER_H_

#include <vector>

#include <js/jsapi.h>

#include <asl/ThreadFifo.h>

#include "TuttleMessage.h"

namespace tuttle {

    using namespace asl;

    class Application;

    enum State {
        STOPPED,
        TRAPPED,
        RUNNING
    };

    class Debugger {
    public:
        Debugger(Application &theApplication);

        JSRuntime *getJavascriptRuntime();
        JSContext *getJavascriptContext();
        JSObject  *getJavascriptGlobal();


        void applicationHandleRequests();


        void debuggerRequestRoundtrip(Message &theMessage);
        


    private:
        Application        &_myApplication;

        JSRuntime          *_myRuntime;
        JSContext          *_myContext;
        JSObject           *_myGlobal;

        State               _myState;

        ThreadFifo<Message&> _myToApplication;
        ThreadFifo<Message&> _myToShell;
    };

}

#endif
