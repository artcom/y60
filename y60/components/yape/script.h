#ifndef Y60_APE_SCRIPT_INCLUDED
#define Y60_APE_SCRIPT_INCLUDED

#include "y60_ape_settings.h"

#include <boost/utility.hpp>

#include Y60_APE_MONKEY_HEADER(jsapi.h)

#include "js_value.h"

namespace y60 { namespace ape {

std::string get_message_from_exception(JSContext * cx) {
    std::string result;
    jsval ex_val;
    if ( JS_GetPendingException( cx, & ex_val )) {
        Object ex(cx, ex_val);
        value msg( ex["message"] );
        if ( ! msg.is_void() ) {
            result = msg.toString();
        }
    }
    return result;
}

class script : public boost::noncopyable {
    public:
        script(JSContext * cx, JSScript * script) : cx_(cx), script_(script) {
            if ( ! script ) {
                if ( JS_IsExceptionPending(cx)) {
                    throw js_error(get_message_from_exception(cx_),
                            PLUS_FILE_LINE);
                }
                throw js_error("",PLUS_FILE_LINE);
            }
#ifdef  Y60_APE_WORKAROUND_SPIDERMONKEY_BUG_438633
            script_object_ = JS_NewScriptObject(cx_, script);
            if ( ! script_object_ ) {
                JS_DestroyScript(cx_, script);
                throw monkey_error("failed to create script object",
                        PLUS_FILE_LINE);
            }
            if ( ! JS_AddNamedRoot(cx_, &script_object_, "ape_workaround_root")) {
                throw monkey_error("failed to root script object",
                        PLUS_FILE_LINE);
            }
#endif
        }
        ~script() {
            if (script_) {
#ifdef  Y60_APE_WORKAROUND_SPIDERMONKEY_BUG_438633
                JS_RemoveRoot(cx_, & script_object_);
#else
                JS_DestroyScript(cx_, script_);
#endif
            }
        }
        value
        exec(JSObject * global) {
            value result(cx_);
            if ( ! JS_ExecuteScript(cx_, global, script_, result.val_ptr())) {
                throw js_error(get_message_from_exception(cx_),
                        PLUS_FILE_LINE);
            }
            return result;
        }
    private:
        JSContext * cx_;
        JSScript  * script_;
#ifdef  Y60_APE_WORKAROUND_SPIDERMONKEY_BUG_438633
        JSObject  * script_object_;
#endif

};

}} // end of namespace ape, y60

#endif // Y60_APE_SCRIPT_INCLUDED

