#ifndef Y60_APE_FUNCTION_INCLUDED
#define Y60_APE_FUNCTION_INCLUDED

#include "y60_ape_settings.h"

#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_member_function_pointer.hpp>

#include "ape_thing.h"
#include "monkey_utilities.h"
#include "caller.h"

namespace y60 { namespace ape { namespace detail {

template <typename F, typename Id >
class function_desc : public ape_thing {
    public:
        function_desc(F f, const char * name, uint8 flags) : ape_thing(ape_function,name) {
            fs_.name  = name;
            fs_.call  = get_caller<F,Id>(f, get_signature( f ));
            fs_.nargs = arity<F>::value;
            fs_.flags = flags;
            fs_.extra = 0;
        }

        virtual
        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            APE_LOG(log::import,log::inf,log::usr) 
                << "importing function '" << get_name() << "'";
            ape_ctx.functions.add( fs_ );
        }

    private:
        JSFunctionSpec fs_;
};

template <typename Id, int Idx = 0>
class namespace_helper {
    public:
        namespace_helper(ape_thing & parent) : parent_(parent) {}
        ~namespace_helper() {}

        template <typename F>
        namespace_helper<Id, Idx + 1>
        function(F f, const char * name) {
            typedef eid<Id, boost::mpl::long_<Idx> > uid;

            parent_.add( ape_thing_ptr(
                        new function_desc<F,uid>(f,name,parent_.function_flags())));

            typedef namespace_helper<Id, Idx + 1> next_type;
            return next_type(parent_);
        }

    private:
        ape_thing & parent_;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_FUNCTION_INCLUDED
