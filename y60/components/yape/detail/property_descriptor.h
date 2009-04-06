#ifndef Y60_APE_PROPERTY_DESCRIPTOR_INCLUDED
#define Y60_APE_PROPERTY_DESCRIPTOR_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include "property_wrapper.h"

namespace y60 { namespace ape { namespace detail {

template <typename T, typename Id>
class property_descriptor : public ape_thing {
    public:
        property_descriptor(const char * name, T v, uint8 flags) :
            ape_thing(ape_property, name)
        {
            typedef property_wrapper<T,Id> accessor_type;
            accessor_type::init(v);
            ps_.name   = name;
            ps_.tinyid = 0; //tiny_id<Id>::value;
            ps_.flags  = flags;
            ps_.getter = & accessor_type::getter;
            ps_.setter = & accessor_type::setter;
        }

        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            if ( boost::is_member_object_pointer<T>::value ) {
                APE_LOG(log::import,log::dbg,log::dev) 
                    << "importing property " << get_name();
                ape_ctx.properties.add( ps_ );
            } else {
                APE_LOG(log::import,log::dbg,log::dev) 
                    << "importing static property " << get_name();
                ape_ctx.static_properties.add( ps_ );
            }
        }
    private:
        JSPropertySpec ps_;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_PROPERTY_DESCRIPTOR_INCLUDED
