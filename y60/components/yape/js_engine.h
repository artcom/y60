#ifndef Y60_APE_JS_ENGINE_INCLUDED
#define Y60_APE_JS_ENGINE_INCLUDED

#include "y60_ape_settings.h"

#include <boost/shared_ptr.hpp>

#include "monkey_headers.h"
#include "exception.h"

#ifdef check
#   undef check
#endif

namespace y60 { namespace ape {

namespace detail {

#ifndef Y60_APE_USE_TRACEMONKEY
#   define JSCLASS_GLOBAL_FLAGS 0
#endif

static JSClass empty_global_class = {
    "Global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

} // end of namespace detail

JSObject *
standard_global_object(JSContext * cx) {
    JSObject * global = JS_NewObject(cx,
            & detail::empty_global_class, NULL, NULL);
    if ( ! global ) {
        throw monkey_error("failed to create global object", PLUS_FILE_LINE);
    }
    if ( ! JS_InitStandardClasses(cx, global)) {
        throw monkey_error("failed to init standard classes", PLUS_FILE_LINE);
    }
    return global;
}

struct default_error_reporter {
    static
    void
    report_error(JSContext * cx, const char * message, JSErrorReport * report) {
        if ( ! report ) {
            std::cout << "NO ERROR REPORT" << std::endl;
            std::cout << message << std::endl;
            return;
        }

        if (JSREPORT_IS_WARNING( report->flags) && ! report_warnings) {
            return;
        }
        std::ostringstream pfx;
        if (report->filename) {
            pfx << report->filename << ":";
        }
        if (report->lineno) {
            pfx << report->lineno << ":";
        }
        pfx << " ";
        if (JSREPORT_IS_WARNING(report->flags)) {
            pfx << (JSREPORT_IS_STRICT(report->flags) ? "strict " : "")
               << "warning: ";
        }
        std::string m(message);
        m.erase(std::remove(m.begin(), m.end(), '\n'), m.end());
        std::cout << pfx.str() << m;

        if (report->linebuf) {
            size_t n = strlen(report->linebuf);
            std::cout << ":" << std::endl
                << pfx.str() << report->linebuf 
                << ((n > 0 && report->linebuf[n-1] == '\n') ? "" : "\n")
                << pfx.str();
            n = PTRDIFF(report->tokenptr, report->linebuf, char);
            size_t j = 0;
            size_t k = 0;
            for (size_t i = 0; i < n; ++i) {
                if (report->linebuf[i] == '\t') {
                    for (k = (j+8) & ~7; j < k; ++j) {
                        std::cout.put('.');
                    }
                    continue;
                }
                std::cout.put('.');
                ++j;
            }
            std::cout << "^" << std::endl;
        } else {
            std::cout << std::endl;
        }
    }
    static const bool report_warnings = true;
};

class value {
    public:
        explicit value(JSContext * cx) : v_( new jsval(JSVAL_VOID)), cx_(cx) {};

        ~value() {}

        std::string to_string() {
            JSString * str = JS_ValueToString(cx_, *v_);
            if ( ! str ) {
                throw monkey_error("string conversion failed",
                        PLUS_FILE_LINE);
            }
            return std::string( JS_GetStringBytes( str ));
        }

        inline bool is_void() const { return *v_ == JSVAL_VOID; }
        inline bool is_null() const { return JSVAL_IS_NULL( *v_ ); }

        jsval * val_ptr() const { return &*v_; }
        jsval & val() const { return *v_; }
        boost::shared_ptr<jsval> val_sptr() const { return v_; }
        JSContext * ctx() const { return cx_; }
    private:
        boost::shared_ptr<jsval> v_;
        JSContext * cx_;
};

template <typename T> inline bool js_type_check( jsval * v );

template <>
inline
bool
js_type_check<int>( jsval * v ) {
    return JSVAL_IS_NUMBER( *v );
}

template <>
inline
bool
js_type_check<bool>( jsval * v ) {
    return JSVAL_IS_BOOLEAN( *v );
}

template <typename T> inline T jsval_to(JSContext * cx, jsval const& v );

template <>
inline
int
jsval_to<int>(JSContext * cx, jsval const& v ) {
    int r;
    if ( ! JS_ValueToInt32(cx, v, & r) ) {
        throw ape_error("conversion failed", PLUS_FILE_LINE);
    }
    return r;
}

template <>
inline
bool
jsval_to<bool>(JSContext * cx, jsval const& v ) {
    JSBool r;
    if ( ! JS_ValueToBoolean(cx, v, & r ) ) {
        throw ape_error("conversion failed", PLUS_FILE_LINE);
    }
    if (JS_TRUE == r) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
class extract {
    public:
        extract(value const& v) : v_(v.val_sptr()), cx_(v.ctx()) {}
        bool check() {
            return js_type_check<T>( &*v_ );
        }
        T operator()() {
            return jsval_to<T>(cx_, *v_);
        }
    private:
        boost::shared_ptr<jsval> v_;
        JSContext * cx_;

};

class js_engine {
    public:
        enum {
            default_runtime_size     = 8 * 1024 * 1024,
            default_stack_chunk_size = 8192
        };

        js_engine() :
            runtime_(NULL),
            context_(NULL),
            global_object_(NULL)
        {
            runtime_ = JS_NewRuntime( default_runtime_size );
            if ( ! runtime_ ) {
                throw monkey_error("failed to init js runtime",
                        PLUS_FILE_LINE);
            }
            context_ = JS_NewContext( runtime_, default_stack_chunk_size );
            if ( ! context_ ) {
                throw monkey_error("failed to init js context",
                        PLUS_FILE_LINE);
            }
            global_object_ = standard_global_object(context_);
            set_option(JSOPTION_VAROBJFIX);
            JS_SetErrorReporter(context_, default_error_reporter::report_error);
        }
        ~js_engine() {
            if (context_) {
                JS_DestroyContext( context_ );
            }
            if (runtime_) {
                JS_DestroyRuntime(runtime_);
            }
            JS_ShutDown();
        }

        value exec_file(std::string const& filename) {
            JSScript * script = JS_CompileFile( context_, global_object_,
                    filename.c_str());
            if ( ! script ) {
                if ( JS_IsExceptionPending(context_)) {
                    std::cout << "EXCEPTION PENDING" << std::endl;
                }
                std::ostringstream os;
                os << "TODO: compile failed error message";
                throw compilation_error(os.str(), PLUS_FILE_LINE);
            }
            return exec_script(script);
        }

        value exec(std::string const& source, const char * script_name = NULL) {
            uintN line_number(1);
            std::string name = script_name ? script_name : "y60::ape::exec()";
            JSScript * script = JS_CompileScript( context_, global_object_,
                    source.c_str(), source.size(), name.c_str(), line_number);
            if ( ! script ) {
                std::ostringstream os;
                os << name << ":" << line_number << ": TODO: what?";
                throw compilation_error(os.str(), PLUS_FILE_LINE);
            }
            return exec_script(script);
        }

        bool is_compilable_unit(std::string const& source) {
            return JS_BufferIsCompilableUnit(context_, global_object_,
                source.c_str(), source.size());
        }
        void clear_pending_exception() {
            JS_ClearPendingException(context_);
        }
        void strict(bool v) { set_option(JSOPTION_STRICT, v); }
        bool strict() { return get_option(JSOPTION_STRICT); }

        void werror(bool v) { set_option(JSOPTION_WERROR, v); }
        bool werror() { return get_option(JSOPTION_WERROR); }

        JSContext * context() const { return context_; }
        JSRuntime * runtime() const { return runtime_; }
        JSObject  * global_object() const { return global_object_; }

    private:
        void set_option(uint32 option, bool v = true) {
            if (v) {
                JS_SetOptions( context_, JS_GetOptions(context_) | option );
            } else {
                JS_SetOptions( context_, JS_GetOptions(context_) & ~ option );
            }
        }
        bool get_option(uint32 option) { return JS_GetOptions(context_) & option; }

        value exec_script(JSScript * script) {
#ifdef  Y60_APE_WORKAROUND_SPIDERMONKEY_BUG_438633
            JSObject * script_object = JS_NewScriptObject(context_, script);
            if ( ! script_object ) {
                JS_DestroyScript(context_, script);
                throw monkey_error("failed to create script object",
                        PLUS_FILE_LINE);
            }
            if ( ! JS_AddNamedRoot(context_, &script_object, "ape_workaround_root")) {
                throw monkey_error("failed to root script object",
                        PLUS_FILE_LINE);
            }
#endif
            value result(context_);
            if ( ! JS_ExecuteScript(context_, global_object_, script,
                        result.val_ptr()))
            {
                throw runtime_error("runtime error", PLUS_FILE_LINE);
            }
            JS_MaybeGC(context_);
#ifdef  Y60_APE_WORKAROUND_SPIDERMONKEY_BUG_438633
            JS_RemoveRoot(context_, & script_object);
#else
            JS_DestroyScript(context_, script);
#endif
            return result;
        }

        JSRuntime * runtime_;
        JSContext * context_;
        JSObject  * global_object_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_JS_ENGINE_INCLUDED
