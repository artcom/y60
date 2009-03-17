#ifndef Y60_APE_PROPERTY_INCLUDED
#define Y60_APE_PROPERTY_INCLUDED

#include "y60_ape_settings.h"

#include <boost/type_traits/is_member_object_pointer.hpp>

namespace y60 { namespace ape { namespace detail {

template <typename Id>
struct tiny_id {
    enum { value = Id::index_type::value };
};

template <typename T, typename Id>
class property_desc : public ape_thing {
    public:
        property_desc(const char * name, T v, uint8 flags) :
            ape_thing(ape_property, name)
        {
            ps_.name   = name;
            ps_.tinyid = tiny_id<Id>::value;
            ps_.flags  = flags;
            ps_.getter = JS_PropertyStub;
            ps_.setter = JS_PropertyStub;
        }

        void
        import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) {
            if ( boost::is_member_object_pointer<T>::value ) {
                APE_LOG(log::import,log::inf,log::usr) 
                    << "importing property '" << get_name() << "'";
                ape_ctx.properties.add( ps_ );
            } else {
                APE_LOG(log::import,log::inf,log::usr) 
                    << "importing static property '" << get_name() << "'";
                ape_ctx.static_properties.add( ps_ );
            }
        }
    private:
        JSPropertySpec ps_;
};

}}} // end of namespace ape, y60

#endif // Y60_APE_PROPERTY_INCLUDED
