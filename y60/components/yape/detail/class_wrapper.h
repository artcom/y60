#ifndef Y60_APE_CLASS_WRAPPER_INCLUDED
#define Y60_APE_CLASS_WRAPPER_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include "monkey_utilities.h"

namespace y60 { namespace ape { namespace detail {

template <typename C>
class class_wrapper {
    public:
        static
        JSBool
        construct(JSContext * cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            try {
                if (!JS_IsConstructing(cx)) {
                    throw bad_call("constructor called as function", PLUS_FILE_LINE);
                }
                return JS_SetPrivate(cx, obj, static_cast<void *>( new C() ));
            } Y60_APE_CATCH_BLOCKS;
        }
        static 
        void 
        finalize(JSContext * cx, JSObject * obj) {
            delete unwrap_native(cx, obj);
        }
        static inline
        C *
        unwrap_native(JSContext * cx, JSObject * obj, jsval * argv = NULL) {
            return static_cast<C*>( JS_GetInstancePrivate( cx, obj, & js_class_, argv));
        }
        static 
        void 
        init_class(JSContext * cx, JSObject * ns, const char * name) {
            js_class_.name        = name;
            // TODO: class flags
            js_class_.flags       = JSCLASS_HAS_PRIVATE;
            js_class_.addProperty = JS_PropertyStub;
            js_class_.delProperty = JS_PropertyStub;
            if ( ! js_class_.getProperty ) {
                js_class_.getProperty = JS_PropertyStub;
            }
            if ( ! js_class_.setProperty ) {
                js_class_.setProperty = JS_PropertyStub;
            }
            js_class_.enumerate   = JS_EnumerateStub;
            js_class_.resolve     = JS_ResolveStub;
            js_class_.convert     = JS_ConvertStub;
            js_class_.finalize    = & finalize;

            if ( ! JS_InitClass( cx, ns,
                        NULL, & js_class_, & construct, 0,
                        specs_.properties.ptr(),
                        specs_.functions.ptr(),
                        specs_.static_properties.ptr(),
                        specs_.static_functions.ptr()))
            {
                throw monkey_error("Failed to init class", PLUS_FILE_LINE);
            }

            // TODO: more class/constructor property flag handling?
            //           for now just make things introspectable
            JSBool found;
            if ( ! JS_SetPropertyAttributes(cx, ns, name, JSPROP_ENUMERATE,
                        &found ))
            {
                throw monkey_error("failed to set property attributes",
                        PLUS_FILE_LINE);
            }
            if ( ! found ) {
                throw ape_error("property not found.", PLUS_FILE_LINE);
            }
        }

        static inline JSClass * js_class_ptr() { return & js_class_; }
        static inline monkey_data & specs() { return specs_; }
    private:
        static JSClass js_class_;
        static monkey_data specs_;
};

template <typename C> JSClass class_wrapper<C>::js_class_ = JSClass();

template <typename C> monkey_data class_wrapper<C>::specs_ = monkey_data();

}}}  // end of namespace detail,ape,y60

#endif // Y60_APE_CLASS_WRAPPER_INCLUDED
