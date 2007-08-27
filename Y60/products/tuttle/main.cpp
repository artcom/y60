
#include <stddef.h>
#include <stdlib.h>

#include <sigsegv.h>
#include <clish/shell.h>

#include <iostream>

#include "Tuttle.h"

using namespace std;
using namespace tuttle;

// clish builtin wrappers
#define CLISH_BUILTIN(name,symbol)                                                 \
    bool_t symbol(const clish_shell_t *theShell, const lub_argv_t *theArguments) { \
        void   *myCookie = clish_shell__get_client_cookie(theShell);               \
        Tuttle *myTuttle = reinterpret_cast<Tuttle*>(myCookie);                    \
        return myTuttle->symbol(theShell, theArguments);                           \
    }

#include "TuttleCommands.h"

#undef CLISH_BUILTIN


// clish builtin definitions
#define CLISH_BUILTIN(name,symbol) \
  { #name, symbol },

static const clish_shell_builtin_t tuttle_builtins[] = {
#include "TuttleCommands.h"
  {NULL,NULL}
};

#undef CLISH_BUILTIN

// clish action handler
// This is used for non-builtin actions.
// Currently executes the action with the users shell.
//
static bool_t
tuttle_action(const clish_shell_t *shell, const char *script)
{
    return (0 == system(script)) ? BOOL_TRUE : BOOL_FALSE;
}

static int
sigsegv_handler(void *fault_address, int serious)
{
    if(serious) {
        cout << "Unhandled page fault accessing " << fault_address << endl;
    }
    return 0;
}

// clish hook table
static clish_shell_hooks_t hooks = {
    /* init     */ NULL,
    /* access   */ NULL,
    /* cmd_line */ NULL,
    /* script   */ tuttle_action,
    /* fini     */ NULL,
    /* builtin  */ tuttle_builtins
};

// main entry point
int
main(int argc, char **argv)
{
    Tuttle myTuttle;

    int result = -1;

    sigsegv_install_handler(&sigsegv_handler);
  
    result = clish_shell_spawn_and_wait(&hooks, (void *)&myTuttle);

    sigsegv_deinstall_handler();
  
    return result ? 0 : 1;

}
