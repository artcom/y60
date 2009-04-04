#ifndef Y60_APE_CHAIN_HELPERS_INCLUDED
#define Y60_APE_CHAIN_HELPERS_INCLUDED

#include <boost/mpl/long.hpp>

#include "function_descriptor.h"
#include "property_descriptor.h"
#include "meta.h"

namespace y60 { namespace ape { namespace detail {

template <typename C, int FIdx = 0, int PIdx = 0>
class class_helper {
    public:
        class_helper(scope_lock sl) : scope_lock_( sl ) {}
        ~class_helper() {}

        template <typename F>
        class_helper<C, FIdx + 1, PIdx>
        function(const char * name, F f) {
            typedef eid<C, boost::mpl::long_<FIdx> > uid;
            current_scope->add( ape_thing_ptr(
                        new function_descriptor<F,uid>(name,f,
                            current_scope->property_flags())));

            typedef class_helper<C, FIdx + 1, PIdx> next_type;
            return next_type(scope_lock_);
        }
        template <typename T>
        class_helper<C, FIdx, PIdx + 1>
        property(const char * name, T v, uint8 more_flags = 0) {
            typedef eid<C, boost::mpl::long_<PIdx> > uid;
            current_scope->add( ape_thing_ptr(
                        new property_descriptor<T,uid>(name,v,
                            current_scope->property_flags() | more_flags )));
            typedef class_helper<C, FIdx, PIdx + 1> next_type;
            return next_type(scope_lock_);
        }
    private:
        scope_lock scope_lock_;
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
                        new function_descriptor<F,uid>(name,f,
                            current_scope->property_flags())));

            typedef namespace_helper<Id, Idx + 1> next_type;
            return next_type();
        }

        namespace_helper<Id, Idx + 1>
        function(const char * name, JSNative f, uintN min_arity) {
            typedef eid<Id, boost::mpl::long_<Idx> > uid;

            current_scope->add( ape_thing_ptr(
                        new function_descriptor<JSNative,uid>(name,f,min_arity,
                            current_scope->property_flags())));

            typedef namespace_helper<Id, Idx + 1> next_type;
            return next_type();
        }
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_CHAIN_HELPERS_INCLUDED
