
#include "TuttleCommand.h"

namespace tuttle {

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
        return *theTrapStatus != JSTRAP_CONTINUE;
    }

}

