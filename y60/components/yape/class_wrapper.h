#ifndef Y60_APE_CLASS_WRAPPER_INCLUDED
#define Y60_APE_CLASS_WRAPPER_INCLUDED

#include "y60_ape_settings.h"

namespace y60 { namespace ape {

class module_initializer;

template <typename Class>
class class_wrapper {
    public:
        class_wrapper(const char* name, module_initializer & m) :
            name_( name ),
            module_( m )
        {}

        template <typename FuncT, FuncT Func>
        void function(const char * name) {}

    private:
        const char *         name_;
        module_initializer & module_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_WRAPPER_INCLUDED
