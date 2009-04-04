#ifndef Y60_APE_CLASS_INCLUDED
#define Y60_APE_CLASS_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Assure.h>

#include "detail/ape_thing.h"
#include "detail/ape_exceptions.h"
#include "property.h"

#include "detail/class_descriptor.h"
#include "detail/property_descriptor.h"

namespace y60 { namespace ape {

template <typename Class>
detail::class_helper<Class>
class_(const char * name) {
    typedef boost::shared_ptr<detail::class_descriptor<Class> > cp;
    // XXX ugly
    cp cd( new detail::class_descriptor<Class>(name,
                    detail::current_scope->property_flags()));
    detail::current_scope->add( cd );
    detail::scope_lock class_scope_lock( new detail::scope(cd));
    return detail::class_helper<Class>( class_scope_lock );
}

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
