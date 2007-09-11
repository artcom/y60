
#ifndef _TUTTLE_APPLICATION_H_
#define _TUTTLE_APPLICATION_H_

#include <unistd.h>

#include <js/jscntxt.h>

namespace tuttle {

#ifdef DEBUG
    class Debugger;
#endif

    class Application {

    public:

        Application();

        bool run();

        void terminate();

        void setInterval(unsigned long theIntervalUsecs);

        JSRuntime *getJavascriptRuntime();
        JSContext *getJavascriptContext();
        JSObject  *getJavascriptGlobal();

#ifdef DEBUG
        void attachDebugger(Debugger *theDebugger);
#endif

    protected:
        bool initJavascript(JSContext *theContext, JSObject *theGlobal);

        bool handleRequests();
        bool process();

    private:
        bool           _myTerminate;
        unsigned long  _myInterval;

        JSRuntime     *_myRuntime;
        JSContext     *_myContext;
        JSObject      *_myGlobal;

#ifdef DEBUG
        Debugger      *_myDebugger;
#endif
    };

}

#endif
