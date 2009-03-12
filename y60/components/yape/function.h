#ifndef Y60_APE_FUNCTION_INCLUDED
#define Y60_APE_FUNCTION_INCLUDED

#include "y60_ape_settings.h"

#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_member_function_pointer.hpp>

#include "ape_thing.h"
#include "monkey_utilities.h"
#include "invoke.h"

namespace y60 { namespace ape { namespace detail {

template <typename F, typename Id, typename Sig>
struct create_invoker {
    typedef typename boost::mpl::if_<
        boost::is_member_function_pointer<F>,
        member_invoker< typename boost::remove_reference< typename get_member_function_class<Sig>::type >::type, F, Sig, Id>,
        invoker<F,Sig,Id>
    >::type type;
};

template <typename F, typename Id, typename Sig>
JSNative
get_invoker( F f, Sig const& sig) {
    typedef typename create_invoker<F,Id,Sig>::type invoker_type;
    invoker_type::init( f );
    return & invoker_type::invoke;
}

template <typename F, typename Id >
class function_desc : public ape_thing {
    public:
        function_desc(F f, const char * name) : ape_thing(ape_function,name) {
            setup_js_function_spec( f, name, get_signature( f ) );
        }

        virtual
        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            APE_LOG(log::import,log::inf,log::usr) 
                << "importing function '" << get_name() << "'";
            ape_ctx.functions.add( fs_ );
        }

        template <typename Sig>
        inline
        void 
        setup_js_function_spec( F f, const char * name, Sig const& sig) {
            fs_.name = name;
            fs_.call = get_invoker<F,Id,Sig>(f, sig);
            fs_.nargs = detail::arity<F,Sig>::value;
            fs_.flags = 0;
            fs_.extra = 0;
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
            typedef typename append_to_id<Id, boost::mpl::long_<Idx> >::type unique_id;

            parent_.add( ape_thing_ptr( new function_desc<F,unique_id>(f,name) ));

            typedef namespace_helper<Id, Idx + 1> next_type;
            return next_type(parent_);
        }
    private:

        ape_thing & parent_;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_FUNCTION_INCLUDED
