#ifndef Y60_APE_RUNTIME_INCLUDED
#define Y60_APE_RUNTIME_INCLUDED

#include "y60_ape_settings.h"

#include <boost/enable_shared_from_this.hpp> 

#include "dynamic_loader.h"

namespace y60 { namespace ape {

class runtime :
    public boost::enable_shared_from_this<runtime>,
    private boost::noncopyable
{
    public:
        typedef boost::shared_ptr<runtime> pointer;
        static pointer create() {
            return pointer(new runtime());
        }

        enum { default_initial_size = 8 * 1024 * 1024 };

        void register_module(dynamic_loader::pointer mod) {
            module_registry_.push_back( mod );
        }

        ~runtime() {
            if ( rt_ ) {
                JS_DestroyRuntime( rt_ );    
            }
        }
        inline pointer ptr() {
            return shared_from_this();
        }
        inline JSRuntime * js_runtime() const { return rt_; }
    private:
        runtime() :
            rt_( JS_NewRuntime( default_initial_size ) )
        {
            if ( ! rt_ ) {
                throw monkey_error("failed to create runtime", PLUS_FILE_LINE);
            }
            JS_SetRuntimePrivate(rt_, this);
        }
        JSRuntime * rt_;
        std::vector<dynamic_loader::pointer> module_registry_;

};

inline
JSRuntime *
get_runtime(JSContext * cx) {
    JSRuntime * jsrt = JS_GetRuntime( cx );
    if ( ! jsrt ) {
        throw monkey_error("failed to get runtime", PLUS_FILE_LINE);
    }
    return jsrt;
}

inline
runtime *
get_ape_runtime(JSContext * cx) {
    JSRuntime * jsrt = get_runtime( cx );
    runtime * rt = static_cast<runtime*>( JS_GetRuntimePrivate( jsrt ) );
    if ( ! rt ) {
        throw ape_error("failed to get ape runtime", PLUS_FILE_LINE);
    }
    return rt;
}

}} // end of namespace ape, y60

#endif // Y60_APE_RUNTIME_INCLUDED
