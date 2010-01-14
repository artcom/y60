#ifndef Y60_APE_CLASS_DESCRIPTOR_INCLUDED
#define Y60_APE_CLASS_DESCRIPTOR_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include "ape_thing.h"

namespace y60 { namespace ape { namespace detail {

template <typename C>
class class_descriptor : public ape_thing {
    public:
        typedef class_wrapper<C> wrapper_type;

        class_descriptor(const char* name, uint8 flags) :
            ape_thing( ape_class, name, flags) {}

        void
        import(JSContext * cx, JSObject * ns, monkey_data &) {
#ifdef Y60_APE_HAVE_TEMPLOG
            typedef log::import log_t;
#endif
            APE_LOG(log_t,log::dbg,log::dev) << "importing class "
                << get_name();

            import_children(cx, ns, wrapper_type::specs());
            wrapper_type::init_class(cx, ns, get_name());
        }
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_CLASS_DESCRIPTOR_INCLUDED
