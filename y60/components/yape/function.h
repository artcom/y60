#ifndef Y60_APE_FUNCTION_INCLUDED
#define Y60_APE_FUNCTION_INCLUDED

#include "y60_ape_settings.h"

#include "ape_thing.h"
#include "monkey_utilities.h"

namespace y60 { namespace ape { namespace detail {

template <typename Invoker>
class function_desc : public ape_thing {
    public:
        function_desc(const char * name) : ape_thing(ape_function,name) {}

        virtual
        void
        import(JSContext * cx, JSObject * ns, js_functions & free_functions) {
            APE_LOG(log::import,log::inf,log::usr) 
                << "importing function " << get_name() << "(...)";
            JSFunctionSpec func;
            func.name  = get_name();
            func.call  = & Invoker::invoke;
            typedef Invoker inv;
            // XXX arity is a major suckage, cuz it requires two (2) arguments!
            //      ... needs fixing :-(
            func.nargs = detail::arity<typename inv::function_type,
                                       typename inv::signature_type>::value;
            func.flags = 0;
            func.extra = 0;
            free_functions.add( func );
        }
};

template <typename Id, typename Prev = void, int Idx = 0>
class function_helper {
    public:
        function_helper(ape_list & desc) : desc_(desc) {}
        ~function_helper() {}

        template <typename F>
        function_helper<Id, function_helper<Id,Prev,Idx>, Idx + 1>
        function(F f, const char * name) {
            typedef typename append_to_id<Id, boost::mpl::long_<Idx> >::type unique_id;

            setup_invoker( f, name, get_signature( f ), unique_id() );

            typedef function_helper< Id, Prev, Idx> this_type;
            typedef function_helper<Id, this_type, Idx + 1> result_type;
            return result_type(desc_);
        }
    private:
        template <typename F, typename Sig, typename CallId>
        inline
        void
        setup_invoker(F f, const char * name, Sig const& /*s*/, CallId const& /*id*/) const {
            typedef detail::invoker<F,Sig,CallId> invoker_t;
            invoker_t::init( f );
            desc_.push_back( ape_thing_ptr( new function_desc<invoker_t>(name) ));
        }

        ape_list & desc_;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_FUNCTION_INCLUDED
