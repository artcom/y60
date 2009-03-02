#ifndef Y60_APE_CLASS_WRAPPER_INCLUDED
#define Y60_APE_CLASS_WRAPPER_INCLUDED

namespace y60 { namespace ape {

class module_initializer;

template <typename Class>
class class_wrapper {
    public:
        class_wrapper(module_initializer & m) : module_( m ) {
        }

        template <typename FuncPtrT, FuncPtrT Func>
        void function(const char * name) {}

    module_initializer & module_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_WRAPPER_INCLUDED
