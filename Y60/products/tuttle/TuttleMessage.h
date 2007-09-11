
#ifndef _TUTTLE_COMMAND_H_
#define _TUTTLE_COMMAND_H_

#include <lub/argv.h>
#include <lub/types.h>

#include <clish/shell.h>

#include "TuttleShell.h"

namespace tuttle {

    typedef bool_t
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

        bool_t               _mySuccess;
    };


    Command::Command(Builtin  theBuiltin,
                     Shell   *theShell,
                     const clish_shell_t *theClish,
                     const lub_argv_t *theArguments)
        : _myBuiltin(theBuiltin), _myShell(theShell), _myClish(theClish), _myArguments(theArguments) {
    }
    
    bool
    Command::execute(JSTrapStatus *theTrapStatus) {
        *theTrapStatus = JSTRAP_CONTINUE;
        _mySuccess = _myBuiltin(_myShell, _myClish, _myArguments, theTrapStatus);
    }

}




#endif /* !_TUTTLE_COMMAND_H_ */
