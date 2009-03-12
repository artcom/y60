#ifndef Y60_APE_ARGUMENTS_INCLUDED
#define Y60_APE_ARGUMENTS_INCLUDED

#include "y60_ape_settings.h"

#include <boost/tuple/tuple.hpp> 
#include <boost/type_traits.hpp> 

#include <boost/mpl/advance.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/transform_view.hpp>

#include "typelist.h"

namespace y60 { namespace ape { namespace detail {

template <typename F>
struct first_arg_index {
    typedef typename boost::mpl::if_<
        boost::is_member_function_pointer<F>,
        boost::mpl::int_<2>,
        boost::mpl::int_<1> >::type type;
};

template <typename F, typename Sig>
class get_arguments {
    private:
        typedef typename first_arg_index<F>::type first_arg_idx;
    public:
        typedef typename boost::mpl::if_<
            boost::mpl::greater< boost::mpl::size<Sig>, first_arg_idx >,
                boost::mpl::iterator_range< 
                    typename boost::mpl::advance<typename boost::mpl::begin<Sig>::type, first_arg_idx>::type,
                    typename boost::mpl::end<Sig>::type>,
                boost::mpl::vector0<> >::type type;
};

template <typename Sig>
struct returns_void :
        boost::is_void< boost::mpl::front<Sig> > {};

template <typename Sig>
struct get_member_function_class : boost::mpl::at_c<Sig, 1 > {};

// XXX ugly shit ... rewrite!
template <typename F, typename Sig>
struct arity {
        enum {
            value  = boost::mpl::minus<
                            typename boost::mpl::size<Sig>::type, 
                            typename first_arg_index<F>::type>::type::value 
        };

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

template <typename F, typename Sig>
inline
void
check_arity(uintN argc) {
    if ( argc != arity<F,Sig>::value ) {
        std::ostringstream os;
        os << "expected " << arity<F,Sig>::value << " arguments but got " << argc;
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
                    << asl::demangled_name< typename boost::mpl::at_c<Types, I::value>::type >() << "'";
                throw bad_arguments(os.str(), PLUS_FILE_LINE);
            }
        }
    private:
        JSContext * cx_;
        jsval     * argv_;
        Tuple     & args_;
};

template <typename F, typename Sig>
class  arguments : public
    tuple_for_args< typename get_arguments<F,Sig>::type>::type 
{
        typedef typename get_arguments<F,Sig>::type argument_types;
        typedef typename get_storage_types<argument_types>::type storage_types;
    public:
        
        arguments( JSContext * cx, uintN argc, jsval * argv ) {
            check_arity<F,Sig>(argc);    
            typedef boost::mpl::range_c<uintN, 0, arity<F,Sig>::value> R;
            boost::mpl::for_each< R >(
                    convert_arguments<arguments, storage_types>( cx, argv, *this ) );
        }

};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_ARGUMENTS_INCLUDED
