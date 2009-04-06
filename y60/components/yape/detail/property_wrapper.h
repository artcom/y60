#ifndef Y60_APE_PROPERTY_WRAPPER_INCLUDED
#define Y60_APE_PROPERTY_WRAPPER_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include <boost/type_traits/is_member_object_pointer.hpp>

#include <y60/jsbase/JScppUtils.h>

#include "class_wrapper.h"

namespace y60 { namespace ape { namespace detail {

template <bool IsDataMember> struct accessor_tag_ {};

template <typename T>
struct accessor_tag :
    accessor_tag_<boost::is_member_object_pointer<T>::value> {};

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
get_property(accessor_tag_<false>, T const& v, JSContext * cx, JSObject * obj, jsval * vp) {
    *vp = jslib::as_jsval(cx, v );
}

template <typename T, typename Id>
inline
void
set_property(accessor_tag_<true>, T v, JSContext * cx, JSObject * obj, jsval * vp) {
    typedef typename Id::context_type C;
    C * n = class_wrapper<C>::unwrap_native(cx, obj);
    if ( ! jslib::convertFrom(cx, *vp, (*n.*v) ) ) {
        throw script_error("conversion failed.", PLUS_FILE_LINE);
    }
}

template <typename T, typename Id>
inline
void
set_property(accessor_tag_<false>, T & v, JSContext * cx, JSObject * obj, jsval * vp) {
    if ( ! jslib::convertFrom(cx, *vp, v) ) {
        throw script_error("conversion failed.", PLUS_FILE_LINE);
    }
}

template <typename T, typename Id>
struct property_wrapper {
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

template <typename T, typename Id> T property_wrapper<T, Id>::v_ = T();

}}} // end of namespace ape, y60

#endif // Y60_APE_PROPERTY_WRAPPER_INCLUDED
