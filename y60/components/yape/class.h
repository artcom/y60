#ifndef Y60_APE_CLASS_INCLUDED
#define Y60_APE_CLASS_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Assure.h>

#include "ape_thing.h"
#include "exception.h"
#include "function.h"

namespace y60 { namespace ape {

namespace detail {

// XXX intermingles two things: registration and runtime functionalty
template <typename Class>
class class_binding {
    public:
        static
        JSBool
        construct(JSContext * cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
            try {
                if (!JS_IsConstructing(cx)) {
                    throw bad_call("constructor called as function", PLUS_FILE_LINE);
                }
                return JS_SetPrivate(cx, obj, (void *) new Class() );
            } Y60_APE_CATCH_BLOCKS;
        }
        static 
        void 
        finalize(JSContext * cx, JSObject * obj) {
            // XXX performance?
            if(JS_GET_CLASS(cx, obj) != & class_binding<Class>::js_class) {
                throw ape_error("finalize() object does not belong to this class.",
                        PLUS_FILE_LINE);
            }
            Class * native = static_cast<Class*>( JS_GetPrivate(cx, obj));
            if (native) {
                delete native;
            }
        }
        static void init_class(JSContext * cx, JSObject * ns, const char * name) {
            typedef class_binding<Class> cb;
            js_class.name        = name;
            js_class.flags       = JSCLASS_HAS_PRIVATE;
            js_class.addProperty = JS_PropertyStub;
            js_class.delProperty = JS_PropertyStub;
            js_class.getProperty = JS_PropertyStub;
            js_class.setProperty = JS_PropertyStub;
            js_class.enumerate   = JS_EnumerateStub;
            js_class.resolve     = JS_ResolveStub;
            js_class.convert     = JS_ConvertStub;
            js_class.finalize    = & cb::finalize;

            if ( ! JS_InitClass( cx, ns,
                        NULL, & cb::js_class, & cb::construct, 0,
                        cb::specs.properties.ptr(),
                        cb::specs.functions.ptr(),
                        cb::specs.static_properties.ptr(),
                        cb::specs.static_functions.ptr()))
            {
                throw monkey_error("Failed to init class", PLUS_FILE_LINE);
            }
        }
// XXX    private:
        static JSClass     js_class;
        static monkey_data specs;
};

template <typename Class>
JSClass class_binding<Class>::js_class = JSClass();

template <typename Class>
monkey_data class_binding<Class>::specs = monkey_data();

template <typename Class>
class class_desc : public ape_thing {
    public:
        typedef boost::shared_ptr<class_desc> ptr_type;

        class_desc(const char* name) : ape_thing( ape_class, name ) {}

        virtual void import(JSContext * cx, JSObject * ns, monkey_data & /*parent_ctx*/ ) {
            typedef log::import log_t;
            APE_LOG(log_t,log::inf,log::usr) << "importing class '"
                << get_name() << "'";

            typedef class_binding<Class> cb;
            import_children(cx, ns, cb::specs);
            cb::init_class(cx, ns, get_name());
        }
}; 

template <typename Class, int FIdx = 0, int PIdx = 0>
class class_helper {
    public:
        class_helper(ape_thing & parent) : parent_( parent ) {}
        ~class_helper() {}

        template <typename F>
        class_helper<Class, FIdx + 1, PIdx>
        function(F f, const char * name) {
            typedef eid<Class, boost::mpl::long_<FIdx> > unique_id;
            parent_.add( ape_thing_ptr( new function_desc<F,unique_id>(f,name) ));

            typedef class_helper<Class, FIdx + 1, PIdx> next_type;
            return next_type( parent_ );
        }
    private:

        ape_thing & parent_;
};

} // end of namespace detail

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
