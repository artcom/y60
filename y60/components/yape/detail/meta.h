#ifndef Y60_APE_META_INCLUDED
#define Y60_APE_META_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include <boost/type_traits/function_traits.hpp> 
#include <boost/type_traits/is_void.hpp> 
#include <boost/type_traits/remove_pointer.hpp> 
#include <boost/type_traits/remove_reference.hpp> 
#include <boost/mpl/if.hpp> 
#include <boost/mpl/front.hpp> 
#include <boost/mpl/at.hpp> 

namespace y60 { namespace ape { namespace detail {

template <typename Sig>
struct get_member_function_class : 
    boost::remove_reference<
            typename boost::mpl::at_c<Sig, 1 >::type > {};

template <typename F>
struct get_member_function_class2;

template <typename F, typename Class>
struct get_member_function_class2<F Class::*> {
    typedef Class type;
};

template <typename F> struct member_function_traits;

template <typename F, typename Class>
struct member_function_traits<F Class::*> : boost::function_traits<F> {};

template <typename F>
struct function_traits {
    typedef typename boost::mpl::if_<
        typename boost::is_member_function_pointer<F>::type,
        member_function_traits<typename boost::remove_pointer<F>::type >,
        boost::function_traits<typename boost::remove_pointer<F>::type > >::type type;
};

template <typename F>
struct arity {
    enum { value = function_traits<F>::type::arity };
};

template <typename F>
struct returns_void :
        boost::is_void< typename function_traits<F>::type::result_type > {};


// integer wrapper. used to create unique ids for functions and properties added
// to the namespace scope of the module.
template <int LineNumber> struct line_number {};

// extended id: used to concatenate two ids
// the name was chosen because it may appear in a compiler error message
// many times but does not contribute any valuable information. With namespaces
// it still is too long.
template <typename Id, typename Appendix> struct eid {
    typedef Id       context_type;
    typedef Appendix index_type;
}; 

}}}

#endif // Y60_APE_META_INCLUDED
