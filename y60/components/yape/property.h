#ifndef Y60_APE_PROPERTY_INCLUDED
#define Y60_APE_PROPERTY_INCLUDED

#include "y60_ape_settings.h"

#include <boost/type_traits/is_member_object_pointer.hpp>

#include <asl/base/TraceUtils.h>

namespace y60 { namespace ape { namespace detail {

template <bool IsDataMember> struct accessor_tag_ {};

template <typename T>
struct accessor_tag : accessor_tag_<boost::is_member_object_pointer<T>::value> {};

template <typename T, typename Id>
inline
void
get_property(accessor_tag_<true>, T v, JSContext * cx, JSObject * obj, jsval * vp) {
    typedef typename Id::context_type C;
    C * n = class_wrapper<C>::unwrap_native(cx, obj);
    *vp = jslib::as_jsval(cx, (*n.*v) );
}

template <typename T, typename Id>
inline
void
set_property(accessor_tag_<true>, T & v, JSContext * cx, JSObject * obj, jsval * vp) {
    typedef typename Id::context_type C;
    C * n = class_wrapper<C>::unwrap_native(cx, obj);
    if ( ! jslib::convertFrom(cx, *vp, (*n.*v) ) ) {
        throw script_error("conversion failed.", PLUS_FILE_LINE);
    }
}

template <typename T, typename Id>
struct property_accessor {
    static
    JSBool
    getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        get_property<T,Id>(accessor_tag<T>(), v_, cx, obj, vp);
        return JS_TRUE;
    }
    static
    JSBool
    setter(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        set_property<T,Id>(accessor_tag<T>(), v_, cx, obj, vp);
        return JS_TRUE;
    }

    static void init(T v) { v_ = v; };
    static T v_;
};

template <typename T, typename Id> T property_accessor<T, Id>::v_ = T();

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
            typedef property_accessor<T,Id> accessor_type;
            accessor_type::init(v);
            ps_.name   = name;
            ps_.tinyid = tiny_id<Id>::value;
            ps_.flags  = flags;
            ps_.getter = & accessor_type::getter;
            ps_.setter = & accessor_type::setter;
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
