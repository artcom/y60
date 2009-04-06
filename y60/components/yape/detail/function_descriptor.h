#ifndef Y60_APE_FUNCTOR_DESCRIPTOR_INCLUDED
#define Y60_APE_FUNCTOR_DESCRIPTOR_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include "function_wrapper.h"
#include "signature.h"

namespace y60 { namespace ape { namespace detail {

template <typename F, typename Id >
class function_descriptor : public ape_thing {
    public:
        function_descriptor(const char * name, F f, uint8 flags) :
            ape_thing(ape_function,name)
        {
            fs_.name  = name;
            fs_.call  = get_function_wrapper<F,Id>(f, get_signature( f ));
            fs_.nargs = arity<F>::value;
            fs_.flags = flags;
            fs_.extra = 0;
        }

        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            if ( boost::is_member_function_pointer<F>::value ) {
                ape_ctx.functions.add( fs_ );
            } else {
                ape_ctx.static_functions.add( fs_ );
            }
        }

    private:
        JSFunctionSpec fs_;
};

template <typename Id>
class function_descriptor<JSNative, Id> : public ape_thing {
    public:
        function_descriptor(const char * name, JSNative f, uintN min_arity,
                uint8 flags) :
            ape_thing(ape_function,name)
        {
            fs_.name  = name;
            fs_.call  = f;
            fs_.nargs = min_arity;
            fs_.flags = flags;
            fs_.extra = 0;
        }

        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            ape_ctx.static_functions.add( fs_ );
        }

    private:
        JSFunctionSpec fs_;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_FUNCTOR_DESCRIPTOR_INCLUDED
