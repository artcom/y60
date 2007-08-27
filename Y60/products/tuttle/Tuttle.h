
#ifndef _TUTTLE_H_
#define _TUTTLE_H_

#include <vector>

#include <js/jsapi.h>

#include <clish/shell.h>

namespace tuttle {

    class Tuttle {
    public:

        Tuttle();

        // c++ api
        void registerContext(JSContext *);

        // commands - debug.xml
        bool_t evaluate    (const clish_shell_t *theShell, const lub_argv_t *theArguments);
        
        // commands - context.xml
        bool_t listContexts(const clish_shell_t *theShell, const lub_argv_t *theArguments);
        bool_t setContext  (const clish_shell_t *theShell, const lub_argv_t *theArguments);
        bool_t showContext (const clish_shell_t *theShell, const lub_argv_t *theArguments);
        
    private:
        JSRuntime                *_myRuntime;
        JSContext                *_myCurrentContext;
        std::vector<JSContext *>  _myContexts;
        JSObject                 *_myGlobal;        
    };
}

#endif
