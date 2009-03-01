#ifndef Y60_APE_SIGNATURE_UTILS_INCLUDED
#define Y60_APE_SIGNATURE_UTILS_INCLUDED

#include <boost/mpl/size.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/type_traits.hpp>

namespace y60 { namespace ape { namespace detail {

template <typename Sig>
uint8 argument_count(Sig const& s) {
    return boost::mpl::size<Sig>::value - 1; // subtract return type
}

template <typename Sig>
struct arguments :
    boost::mpl::iterator_range<
            typename boost::mpl::next< typename boost::mpl::begin<Sig>::type >::type,
            typename boost::mpl::end<Sig>::type >{};

template <typename Sig, int I>
struct argument_storage : 
    boost::remove_const<
        typename boost::remove_reference<
            typename boost::mpl::at<
                typename arguments<Sig>::type, boost::mpl::int_<I> >::type >::type >{};
            

template <typename Sig>
struct arity : boost::mpl::int_<  boost::mpl::size<Sig>::value - 1 > {};

template <typename Sig>
struct return_type : boost::mpl::front<Sig> {};

template <typename Sig>
struct returns_void : boost::is_same< typename return_type<Sig>::type, void> {};


}}} // end of namespace detail, ape, y60

#endif // Y60_APE_SIGNATURE_UTILS_INCLUDED
