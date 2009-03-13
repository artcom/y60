#ifndef Y60_APE_CLASS_INCLUDED
#define Y60_APE_CLASS_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Assure.h>

#include "ape_thing.h"
#include "exception.h"
#include "function.h"

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
                return JS_SetPrivate(cx, obj, (void *) new C() );
            } Y60_APE_CATCH_BLOCKS;
        }
        static 
        void 
        finalize(JSContext * cx, JSObject * obj) {
            C * native = unwrap_native(cx, obj);
            delete native;
        }
        static inline
        C *
        unwrap_native(JSContext * cx, JSObject * obj) {
            C * n = static_cast<C*>( JS_GetInstancePrivate( cx, obj, & js_class_, NULL));
            if ( ! n ) {
                throw ape_error("failed to get native instance", PLUS_FILE_LINE);
            }
            return n;
        }
        static 
        void 
        init_class(JSContext * cx, JSObject * ns, const char * name) {
            js_class_.name        = name;
            js_class_.flags       = JSCLASS_HAS_PRIVATE;
            js_class_.addProperty = JS_PropertyStub;
            js_class_.delProperty = JS_PropertyStub;
            js_class_.getProperty = JS_PropertyStub;
            js_class_.setProperty = JS_PropertyStub;
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
        typedef boost::shared_ptr<class_desc> ptr_type;

        class_desc(const char* name) : ape_thing( ape_class, name ) {}

        virtual void import(JSContext * cx, JSObject * ns, monkey_data & /*parent_ctx*/ ) {
            typedef log::import log_t;
            APE_LOG(log_t,log::inf,log::usr) << "importing class '"
                << get_name() << "'";

            typedef class_wrapper<C> cw;
            import_children(cx, ns, cw::specs());
            cw::init_class(cx, ns, get_name());
        }
}; 

template <typename C, int FIdx = 0, int PIdx = 0>
class class_helper {
    public:
        class_helper(ape_thing & parent) : parent_( parent ) {}
        ~class_helper() {}

        template <typename F>
        class_helper<C, FIdx + 1, PIdx>
        function(F f, const char * name) {
            typedef eid<C, boost::mpl::long_<FIdx> > unique_id;
            parent_.add( ape_thing_ptr( new function_desc<F,unique_id>(f,name) ));

            typedef class_helper<C, FIdx + 1, PIdx> next_type;
            return next_type( parent_ );
        }
    private:

        ape_thing & parent_;
};

} // end of namespace detail

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
