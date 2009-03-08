#ifndef Y60_APE_CLASS_INCLUDED
#define Y60_APE_CLASS_INCLUDED

#include "y60_ape_settings.h"

namespace y60 { namespace ape {

template <typename Derived> class module;

template <typename Class>
class class_wrapper {
    public:
        class_wrapper(const char* name) :
            name_( name )
        {}

        template <typename FuncT, FuncT Func>
        void function(const char * name) {}

    private:
        const char * name_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
