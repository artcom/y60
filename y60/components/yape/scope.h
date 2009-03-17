#ifndef Y60_APE_SCOPE_INCLUDED
#define Y60_APE_SCOPE_INCLUDED

#include "y60_ape_settings.h"

#include "ape_thing.h"

namespace y60 { namespace ape { namespace detail {

ape_thing_ptr current_scope = ape_thing_ptr();


struct scope {
    scope( ape_thing_ptr s) :
        previous_( current_scope )
    {
        APE_LOG(log::import,log::dbg,log::dev) << "scope push: "
            << s->get_name();
        current_scope = s;
    }
    ~scope() {
        APE_LOG(log::import,log::dbg,log::dev) << "scope pop: "
            << current_scope->get_name();
        current_scope = previous_;
    }

    ape_thing_ptr previous_;
};

typedef boost::shared_ptr<scope> scope_lock;

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_SCOPE_INCLUDED
