
#ifndef _TUTTLE_SHELL_H_
#define _TUTTLE_SHELL_H_

#include <pthread.h>

#include <clish/shell.h>

#include <js/jscntxt.h>

#include "TuttleDebugger.h"

namespace tuttle {

    class Shell {
    public:
        Shell(Debugger &theDebugger);

        bool_t launch(void);
        bool_t shutdown(void);

#define CLISH_BUILTIN(name, symbol)                                    \
        bool_t command##symbol (const lub_argv_t *theArguments);

#include "TuttleShell.def"

#undef  CLISH_BUILTIN


    private:
        Debugger  &_myDebugger;
        
        JSContext *_myContext;
        JSObject  *_myGlobal;

        pthread_t      _myClishThread;
        pthread_attr_t _myClishThreadAttributes;

    };

}

#endif
