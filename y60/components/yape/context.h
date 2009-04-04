#ifndef Y60_APE_CONTEXT_INCLUDED
#define Y60_APE_CONTEXT_INCLUDED

#include "y60_ape_settings.h"

#include <boost/enable_shared_from_this.hpp>

#include "runtime.h"

namespace y60 { namespace ape {

class context :
    public boost::enable_shared_from_this<context>,
    private boost::noncopyable
{
    public:
        typedef boost::shared_ptr<context> pointer;
        static pointer create(runtime::pointer runtime) {
            return pointer(new context(runtime));
        }
        enum { default_stack_chunk_size = 8192 };
        ~context() {
            if (cx_) {
                JS_DestroyContext(cx_);
            }
        }
    inline JSContext * js_context() const { return cx_; }
    private:
        context(runtime::pointer rt) : runtime_( rt ) {
            if ( ! runtime_ ) {
                throw ape_error("invalid runtime", PLUS_FILE_LINE);
            }
            cx_ = JS_NewContext( runtime_->js_runtime(), default_stack_chunk_size );
            if ( ! cx_ ) {
                throw monkey_error("failed to create context", PLUS_FILE_LINE);
            }
            JS_SetContextPrivate( cx_, this );
        }
        runtime::pointer runtime_;
        JSContext *      cx_;

};

inline
context *
get_ape_context(JSContext * cx) {
    return static_cast<context*>( JS_GetContextPrivate( cx ) );
}

}} // end of namespace ape, y60

#endif // Y60_APE_CONTEXT_INCLUDED
