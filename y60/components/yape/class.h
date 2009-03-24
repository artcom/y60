#ifndef Y60_APE_CLASS_INCLUDED
#define Y60_APE_CLASS_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Assure.h>

#include "ape_thing.h"
#include "exception.h"
#include "function.h"
#include "property.h"

namespace y60 { namespace ape {

namespace detail {

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
        }

        template <typename AllItems>
        inline
        void
        init_property_accessors( AllItems const& all_items ) {
            
        }

        static inline JSClass * js_class_ptr() { return & js_class_; }
        static inline monkey_data & specs() { return specs_; }
    private:
        static JSClass js_class_;
        static monkey_data specs_;
};

template <typename C> JSClass class_wrapper<C>::js_class_ = JSClass();

template <typename C> monkey_data class_wrapper<C>::specs_ = monkey_data();

template <typename C>
class class_desc : public ape_thing {
    public:
        typedef class_wrapper<C> wrapper_type;

        class_desc(const char* name, uint8 flags) :
            ape_thing( ape_class, name, flags),
            property_accessors_created_( false ) {}

        void 
        import(JSContext * cx, JSObject * ns, monkey_data &) {
            typedef log::import log_t;
            APE_LOG(log_t,log::dbg,log::dev) << "importing class "
                << get_name();

            import_children(cx, ns, wrapper_type::specs());
            wrapper_type::init_class(cx, ns, get_name());
        }
        template <typename AllItems>
        inline
        void
        init_property_accessors( AllItems const& all_items ) {
            if ( ! property_accessors_created_ ) {
                wrapper_type::init_property_accessors( all_items );
                property_accessors_created_ = true;
            }
        }
    private:
        bool property_accessors_created_;
}; 

template <typename C, int FIdx = 0, int PIdx = 0>
class class_helper {
    public:
        class_helper(scope_lock sl) : scope_lock_( sl ) {}
        ~class_helper() {}

        template <typename F>
        class_helper<C, FIdx + 1, PIdx>
        function(const char * name, F f) {
            typedef eid<C, boost::mpl::long_<FIdx> > uid;
            current_scope->add( ape_thing_ptr(
                        new function_desc<F,uid>(name,f,
                            current_scope->property_flags())));

            typedef class_helper<C, FIdx + 1, PIdx> next_type;
            return next_type(scope_lock_);
        }
        template <typename T>
        class_helper<C, FIdx, PIdx + 1>
        property(const char * name, T v, uint8 more_flags = 0) {
            typedef eid<C, boost::mpl::long_<PIdx> > uid;
            current_scope->add( ape_thing_ptr(
                        new property_desc<T,uid>(name,v,
                            current_scope->property_flags() | more_flags )));
            typedef class_helper<C, FIdx, PIdx + 1> next_type;
            return next_type(scope_lock_);
        }
    private:
        scope_lock scope_lock_;
};

} // end of namespace detail

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
