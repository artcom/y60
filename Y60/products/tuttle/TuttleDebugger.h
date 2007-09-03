
#ifndef _TUTTLE_DEBUGGER_H_
#define _TUTTLE_DEBUGGER_H_

#include <vector>

#include <js/jsapi.h>

namespace tuttle {

    class Debugger {
    public:
        Debugger();

        JSContext *getContext();
        JSObject  *getGlobal();

    private:
        JSRuntime                *_myRuntime;
        JSContext                *_myContext;
        JSObject                 *_myGlobal;

    };

}

#endif
