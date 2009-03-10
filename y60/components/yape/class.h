#ifndef Y60_APE_CLASS_INCLUDED
#define Y60_APE_CLASS_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Assure.h>

#include "ape_thing.h"
#include "exception.h"
#include "invoke.h"

namespace y60 { namespace ape {

namespace detail {

// XXX intermingles two things: registration and runtime functionalty
template <typename Class>
class class_binding {
    public:
        typedef Class native_type;

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
        static JSClass    js_class;
        static JSObject * prototype;
};

template <typename Class>
JSClass class_binding<Class>::js_class = JSClass();

template <typename Class>
JSObject * class_binding<Class>::prototype = 0;

template <typename Class>
class class_desc : public ape_thing, public ape_list {
    public:
        typedef boost::shared_ptr<class_desc> ptr_type;

        class_desc(const char* name) : ape_thing( ape_class, name ) {}

        virtual void import(JSContext * cx, JSObject * ns, js_functions & free_functions) {
            typedef log::import log_t;
            APE_LOG(log_t,log::inf,log::usr) << "importing class '"
                << get_name() << "'";

            typedef class_binding<Class> cb;
            JSClass & c = cb::js_class;

            c.name        = get_name();
            c.flags       = JSCLASS_HAS_PRIVATE;
            c.addProperty = JS_PropertyStub;
            c.delProperty = JS_PropertyStub;
            c.getProperty = JS_PropertyStub;
            c.setProperty = JS_PropertyStub;
            c.enumerate   = JS_EnumerateStub;
            c.resolve     = JS_ResolveStub;
            c.convert     = JS_ConvertStub;
            c.finalize    = & cb::finalize;

            cb::prototype = JS_InitClass( cx, ns,
                    NULL, & cb::js_class, & cb::construct, 0,
                    NULL, NULL, NULL, NULL );
                    /*
                    properties_.ptr(),
                    functions_.ptr(),
                    static_properties_.ptr(),
                    static_functions_.ptr());*/
            if ( ! cb::prototype ) {
                throw monkey_error("Failed to init class", PLUS_FILE_LINE);
            }
        }

        /*
        template <typename F>
        void
        add_function(F f, const char * name) {
            JSFunctionSpec js_func;
            js_func.name = name;
            setup_invoker( js_func, f, get_signature( f ) );
            functions_.add( js_func );
        }
        */
    private:
        /*
        template <typename F, typename Sig>
        void
        setup_invoker( JSFunctionSpec & js_func, F f, Sig const& sig ) {
            // XXX this should clash!
            typedef detail::member_invoker<class_binding<Class>,F,Sig,Class> invoker_type;
            invoker_type::init( f );
            js_func.call  = & invoker_type::invoke;
            js_func.nargs = arity<F,Sig>::value;
            js_func.flags = 0;
            js_func.extra = 0;
        }
        js_properties properties_;
        js_functions  functions_;
        js_properties static_properties_;
        js_functions  static_functions_;
        */
}; 

template <typename Class, typename Prev = void, int FIdx = 0, int PIdx = 0>
class class_helper {
    public:
        class_helper(ape_list & desc) : desc_( desc ) {}
        ~class_helper() {}

        template <typename F>
        class_helper<Class, class_helper<Class, Prev, FIdx, PIdx>, FIdx + 1, PIdx>
        function(F f, const char * name) {
            typedef class_helper<Class, Prev, FIdx, PIdx> this_type;
            typedef class_helper<Class, this_type, FIdx + 1, PIdx> result_type;

            return result_type( desc_ );
        }
    private:
        ape_list & desc_;
};

} // end of namespace detail

}} // end of namespace ape, y60

#endif // Y60_APE_CLASS_INCLUDED
