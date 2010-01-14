#ifndef Y60_APE_MONKEY_UTILITIES_INCLUDED
#define Y60_APE_MONKEY_UTILITIES_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include Y60_APE_MONKEY_HEADER(jsapi.h)

// for memset
#include <string.h>

#include <asl/base/begin_end.h>

#include "ape_exceptions.h"

namespace y60 { namespace ape { namespace detail {

template <typename Spec>
inline
Spec
init_monkey_spec_terminator() {
    Spec terminator;
    memset( static_cast<void*>( & terminator), 0, sizeof( Spec ) );
    return terminator;
}

template <typename Spec>
struct monkey_spec_trait {
    static Spec const& terminator() {
        static Spec terminator = init_monkey_spec_terminator<Spec>();
        return terminator;
    }
};

template <typename Spec>
class monkey_specs {
    public:
        monkey_specs() : terminated_( false ) {}
        inline void add(Spec const& s) {
            ensure_terminated(false);
            specs_.push_back( s );
        }
        inline bool empty() const { return specs_.empty(); }
        inline size_t size() const {
            if (empty()) {
                return 0;
            } else if ( terminated_ ) {
                return specs_.size() - 1;
            } else {
                return specs_.size();
            }
        }
        inline void terminate() {
            ensure_terminated(false);
            if ( ! specs_.empty()) {
                specs_.push_back( monkey_spec_trait<Spec>::terminator());
            }
            terminated_ = true;
        }
        Spec * ptr() {
            if ( ! terminated_ ) {
                terminate();
            }
            return asl::begin_ptr( specs_ );
        }
    private:
        inline void ensure_terminated( bool should_state)  const {
            if (terminated_ != should_state) {
                throw ape_error("spidermoney spec array terminator check failed",
                        PLUS_FILE_LINE);
            }
        }

        bool              terminated_;
        std::vector<Spec> specs_;
};

typedef monkey_specs<JSFunctionSpec> js_functions;
typedef monkey_specs<JSPropertySpec> js_properties;

struct monkey_data {
    js_functions  functions;
    js_properties properties;
    js_functions  static_functions;
    js_properties static_properties;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_MONKEY_UTILITIES_INCLUDED
