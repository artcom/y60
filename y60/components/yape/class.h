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
class class_ : public detail::context_decorator<Class> {
    public:
        class_(const char * name) : scope_( init( name ) ) {}
    private:
        detail::ape_thing_ptr init( const char * name) {
            typedef boost::shared_ptr<detail::class_descriptor<Class> > desc_type;
            desc_type desc( new detail::class_descriptor<Class>(name,
                    detail::current_scope->property_flags()));
            detail::current_scope->add( desc );
            return desc;
        }
        detail::scope scope_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
