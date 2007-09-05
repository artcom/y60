
#ifndef _TUTTLE_APPLICATION_H_
#define _TUTTLE_APPLICATION_H_

#include <unistd.h>

#include <js/jscntxt.h>

namespace tuttle {

    class Application {

    public:

        Application();

        bool run();

        bool terminate();

        void setInterval(unsigned long theIntervalUsecs);

        JSRuntime *getJavascriptRuntime();
        JSContext *getJavascriptContext();
        JSObject  *getJavascriptGlobal();

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
    };

}

#endif
