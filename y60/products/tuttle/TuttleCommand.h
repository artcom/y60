
#ifndef _TUTTLE_COMMAND_H_
#define _TUTTLE_COMMAND_H_

#include <lub/argv.h>
#include <lub/types.h>

#include <clish/shell.h>

#include <js/spidermonkey/jsdbgapi.h>

namespace tuttle {

    class Shell;

    typedef bool
        Builtin(Shell               *theShell,
                const clish_shell_t *theClish,
                const lub_argv_t    *theArguments,
                JSTrapStatus        *theTrapStatus);

    class Command {
    public:

        Command(Builtin  theBuiltin,
                Shell   *theShell,
                const clish_shell_t *theClish,
                const lub_argv_t *theArguments);

        bool execute(JSTrapStatus *theTrapStatus);

    private:
        Builtin             *_myBuiltin;

        Shell               *_myShell;
        const clish_shell_t *_myClish;
        const lub_argv_t    *_myArguments;

        bool                 _mySuccess;
    };

}

#endif /* !_TUTTLE_COMMAND_H_ */
