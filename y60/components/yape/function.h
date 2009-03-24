#ifndef Y60_APE_FUNCTION_INCLUDED
#define Y60_APE_FUNCTION_INCLUDED

#include "y60_ape_settings.h"

#include <boost/type_traits/is_member_function_pointer.hpp>

#include "ape_thing.h"
#include "monkey_utilities.h"
#include "caller.h"
#include "scope.h"

namespace y60 { namespace ape { namespace detail {

template <typename F, typename Id >
class function_desc : public ape_thing {
    public:
        function_desc(const char * name, F f, uint8 flags) : ape_thing(ape_function,name) {
            fs_.name  = name;
            fs_.call  = get_caller<F,Id>(f, get_signature( f ));
            fs_.nargs = arity<F>::value;
            fs_.flags = flags;
            fs_.extra = 0;
        }

        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            if ( boost::is_member_function_pointer<F>::value ) {
                APE_LOG(log::import,log::dbg,log::dev) 
                    << "importing member function " << get_name();
                ape_ctx.functions.add( fs_ );
            } else {
                APE_LOG(log::import,log::dbg,log::dev) 
                    << "importing static function " << get_name();
                ape_ctx.static_functions.add( fs_ );
            }
        }

    private:
        JSFunctionSpec fs_;
};

template <typename Id, int Idx = 0>
class namespace_helper {
    public:
        namespace_helper() {}
        ~namespace_helper() {}

        template <typename F>
        namespace_helper<Id, Idx + 1>
        function(const char * name, F f) {
            typedef eid<Id, boost::mpl::long_<Idx> > uid;

            current_scope->add( ape_thing_ptr(
                        new function_desc<F,uid>(name,f,current_scope->property_flags())));

            typedef namespace_helper<Id, Idx + 1> next_type;
            return next_type();
        }

        namespace_helper<Id,Idx> &
        enumerate(bool flag) {
            current_scope->enumerate(flag);
            return *this;
        }
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_FUNCTION_INCLUDED
