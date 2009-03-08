#ifndef Y60_APE_ARGUMENTS_INCLUDED
#define Y60_APE_ARGUMENTS_INCLUDED

#include "y60_ape_settings.h"

//#include "signature_utilities.h"

#include <boost/tuple/tuple.hpp> 
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/for_each.hpp>

#include "typelist.h"

namespace y60 { namespace ape { namespace detail {

template <typename Signature>
struct get_arguments :
        boost::mpl::if_<
                boost::mpl::greater< boost::mpl::size<Signature>, boost::mpl::int_<1> >,
                boost::mpl::iterator_range< 
                    typename boost::mpl::next< typename boost::mpl::begin<Signature>::type>::type,
                    typename boost::mpl::end<Signature>::type>,
                boost::mpl::vector0<> > {};

template <typename Signature>
    struct returns_void :
        boost::is_void< typename boost::mpl::front<Signature>::type > {};

template <typename Signature>
    struct arity {
        enum { value = boost::mpl::size< typename get_arguments<Signature>::type >::value };
    };


struct storage_type {
    template <typename T>
        struct apply {
            typedef typename boost::remove_const<
                typename boost::remove_reference<T>::type >::type type;
        };
};

template <typename Seq>
struct get_storage_types : 
        boost::mpl::transform_view< Seq, storage_type > {};

template <typename Arguments>
struct tuple_for_args :
    boost::mpl::reverse_fold<
            typename get_storage_types< Arguments >::type,
                    boost::tuples::null_type, 
                    boost::tuples::cons<boost::mpl::_2, boost::mpl::_1> > {};

template <typename Signature>
inline
void
check_arity(uintN argc) {
    if ( argc != arity<Signature>::value ) {
        std::ostringstream os;
        os << "expected " << arity<Signature>::value << " arguments but got " << argc;
        throw bad_arguments( os.str(), PLUS_FILE_LINE );
    }
}

template <typename Tuple, typename Types>
class convert_arguments {
    public:
        convert_arguments(JSContext * cx, jsval * argv, Tuple & args) :
            cx_(cx), argv_(argv), args_(args) {}

        template <typename I>
        inline
        void
        operator()(I const& /*idx*/) {
            if ( ! jslib::convertFrom(cx_, argv_[I::value], boost::get<I::value>( args_ ) )) {
                JSString * js_string = JS_ValueToString(cx_, argv_[I::value]);
                std::string js_string_rep;
                if(js_string) {
                    js_string_rep = JS_GetStringBytes(js_string);
                }
                JS_RemoveRoot(cx_, & js_string );
                std::ostringstream os;
                os << "could not convert argument " << I::value << " with value '"
                    << js_string_rep << "' to native type '" 
                    << asl::demangled_name< boost::mpl::at_c<Types, I::value> >() << "'";
                throw bad_arguments(os.str(), PLUS_FILE_LINE);
            }
        }
    private:
        JSContext * cx_;
        jsval     * argv_;
        Tuple     & args_;
};

template <typename Signature>
class  arguments : public
    tuple_for_args< typename get_arguments<Signature>::type>::type 
{
        typedef typename get_arguments<Signature>::type argument_types;
        typedef typename get_storage_types<Signature>::type storage_types;
    public:
        
        arguments( JSContext * cx, uintN argc, jsval * argv ) {
            check_arity<Signature>(argc);    
            typedef boost::mpl::range_c<uintN, 0, arity<Signature>::value> R;
            boost::mpl::for_each< R >(
                    convert_arguments<arguments, storage_types>( cx, argv, *this ) );
        }

};


}}} // end of namespace detail, ape, y60

#endif // Y60_APE_ARGUMENTS_INCLUDED
