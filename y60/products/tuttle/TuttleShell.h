
#ifndef _TUTTLE_SHELL_H_
#define _TUTTLE_SHELL_H_

#include <pthread.h>

#include <clish/shell.h>

#include <js/spidermonkey/jscntxt.h>

namespace tuttle {

    class Command;
    class Debugger;

    class Shell {
    public:
        Shell(Debugger &theDebugger);

        bool_t launch(void);
        bool_t shutdown(void);

    void execute(Command *theCommand);

#define CLISH_BUILTIN(name, symbol)                               \
        bool command##symbol (Shell               *theShell,      \
                              const clish_shell_t *theClish,      \
                              const lub_argv_t    *theArguments,  \
                              JSTrapStatus        *theTrapStatus);

#include "TuttleShell.def"

#undef  CLISH_BUILTIN


    private:
        Debugger  &_myDebugger;

        JSContext *_myContext;
        JSObject  *_myGlobal;

        pthread_t       _myClishThread;
        pthread_attr_t  _myClishThreadAttributes;

    };

}

#endif
