
#ifndef _TUTTLE_DEBUGGER_H_
#define _TUTTLE_DEBUGGER_H_

#include <vector>

#include <js/jsapi.h>

namespace tuttle {

    class Application; 

    class Debugger {
    public:
        Debugger(Application &theApplication);

        JSRuntime *getJavascriptRuntime();
        JSContext *getJavascriptContext();
        JSObject  *getJavascriptGlobal();

    private:
        Application              &_myApplication;

        JSRuntime                *_myRuntime;
        JSContext                *_myContext;
        JSObject                 *_myGlobal;

    };

}

#endif
