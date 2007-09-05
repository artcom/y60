
#ifndef _TUTTLE_DEBUGGER_H_
#define _TUTTLE_DEBUGGER_H_

#include <vector>

#include <js/jsapi.h>

#include "TuttleApplication.h"

namespace tuttle {

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
