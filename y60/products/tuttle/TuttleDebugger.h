
#ifndef _TUTTLE_DEBUGGER_H_
#define _TUTTLE_DEBUGGER_H_

#include <queue>

#include <js/spidermonkey/jsapi.h>

#include <asl/base/ThreadLock.h>
#include <asl/base/ThreadSemaphore.h>

namespace tuttle {

    class Application;
    class Command;

    class Debugger {
    public:
        Debugger(Application &theApplication);

        JSRuntime *getJavascriptRuntime();
        JSContext *getJavascriptContext();
        JSObject  *getJavascriptGlobal();

        void executeCommands ();

        void execute(Command *theCommand);

        void queuePush(Command *theCommand);
        bool queueEmpty();
        Command *queuePop();

    private:
        Application        &_myApplication;

        JSRuntime          *_myRuntime;
        JSContext          *_myContext;
        JSObject           *_myGlobal;

    std::queue<Command *> _myQueue;
        asl::ThreadLock       _myQueueLock;

        asl::ThreadSemaphore  _myRequestSemaphore;
        asl::ThreadSemaphore  _myResultSemaphore;
    };

}

#endif
