#ifndef Y60_JAVASCRIPT_SIGNATURE_UTILS_INCLUDED
#define Y60_JAVASCRIPT_SIGNATURE_UTILS_INCLUDED

#include <boost/mpl/size.hpp>
#include <boost/mpl/front.hpp>
#include <boost/type_traits.hpp>

namespace y60 { namespace ape { namespace detail {

template <class Sig>
uint8 argument_count(Sig const& s) {
    return boost::mpl::size<Sig>::value - 1; // subtract return type
}

template <typename Sig>
struct return_type : boost::mpl::front<Sig> {};

template <typename Sig>
struct returns_void : boost::is_same< typename return_type<Sig>::type, void> {};


}}} // end of namespace detail, ape, y60

#endif // Y60_JAVASCRIPT_SIGNATURE_UTILS_INCLUDED
