#ifndef Y60_APE_JS_ENGINE_INCLUDED
#define Y60_APE_JS_ENGINE_INCLUDED

#include "y60_ape_settings.h"

#include <boost/shared_ptr.hpp>

#include "detail/monkey_headers.h"
#include "detail/ape_exceptions.h"
#include "error_reporter.h"
#include "js_value.h"
#include "runtime.h"
#include "context.h"
#include "script.h"

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

struct empty_global_factory {
    virtual ~empty_global_factory() {}
    virtual
    JSObject *
    create(JSContext * cx) const {
        JSObject * global = JS_NewObject(cx,
                & detail::empty_global_class, NULL, NULL);
        if ( ! global ) {
            throw monkey_error("failed to create empty global object",
                    PLUS_FILE_LINE);
        }
        return global;
    }
};

struct standard_global_factory : empty_global_factory {
    virtual
    JSObject *
    create(JSContext * cx) const {
        JSObject * global = empty_global_factory::create(cx);
        if ( ! JS_InitStandardClasses(cx, global)) {
            throw monkey_error("failed to init standard classes", PLUS_FILE_LINE);
        }
        return global;
    }
};

template <typename GlobalObjectFactory = standard_global_factory,
          typename ErrorReporterPolicy = default_error_reporter >
class js_engine {
    public:
        enum {
            default_runtime_size     = 8,
            default_stack_chunk_size = 8192
        };

        js_engine() : runtime_(NULL), context_(NULL), global_object_(NULL) {
            GlobalObjectFactory global_factory;
            init(default_runtime_size, default_stack_chunk_size,
                global_factory);
        }
        js_engine(GlobalObjectFactory const& global_factory) :
            global_object_(NULL)
        {
            init(default_runtime_size, default_stack_chunk_size,
                global_factory);
        }
        js_engine(GlobalObjectFactory const& global_factory,
                size_t runtime_size) :
            global_object_(NULL)
        {
            init(runtime_size, default_stack_chunk_size, global_factory);
        }
        js_engine(GlobalObjectFactory const& global_factory,
                size_t runtime_size, size_t stack_chunk_size) :
            global_object_(NULL)
        {
            init(runtime_size, stack_chunk_size, global_factory);
        }
        ~js_engine() {
            context_ = context::pointer();
            runtime_ = runtime::pointer();
            JS_ShutDown();
        }

        value exec_file(std::string const& filename) {
            JSScript * script = JS_CompileFile( context_->js_context(), global_object_,
                    filename.c_str());
            return exec_script(script);
        }

        value exec(std::string const& source, const char * script_name = NULL) {
            uintN line_number(1);
            std::string name = script_name ? script_name : "y60::ape::exec()";
            JSScript * script = JS_CompileScript( context_->js_context(), global_object_,
                    source.c_str(), source.size(), name.c_str(), line_number);
            return exec_script(script);
        }

        bool is_compilable_unit(std::string const& source) {
            return JS_BufferIsCompilableUnit(context_->js_context(), global_object_,
                source.c_str(), source.size());
        }
        void clear_pending_exception() {
            JS_ClearPendingException(context_->js_context());
        }
        void strict(bool v) { set_option(JSOPTION_STRICT, v); }
        bool strict() { return get_option(JSOPTION_STRICT); }

        void werror(bool v) { set_option(JSOPTION_WERROR, v); }
        bool werror() { return get_option(JSOPTION_WERROR); }

        JSContext * context() const { return context_; }
        JSRuntime * runtime() const { return runtime_; }
        JSObject  * global_object() const { return global_object_; }

    private:
        void init(size_t runtime_size, size_t stack_chunk_size,
            GlobalObjectFactory const& global_factory)
        {
            // TODO: pass inital size
            runtime_ = runtime::create();
            context_ = context::create( runtime_ );
            global_object_ = global_factory.create(context_->js_context());
            set_option(JSOPTION_VAROBJFIX);
            typedef typename ErrorReporterPolicy::pointer report_ptr;
            error_reporter =
                report_ptr(new ErrorReporterPolicy(context_->js_context()));
        }

        void set_option(uint32 option, bool v = true) {
            if (v) {
                JS_SetOptions( context_->js_context(), JS_GetOptions(context_->js_context()) | option );
            } else {
                JS_SetOptions( context_->js_context(), JS_GetOptions(context_->js_context()) & ~ option );
            }
        }
        bool get_option(uint32 option) { return JS_GetOptions(context_->js_context()) & option; }

        value exec_script(JSScript * s) {
            return script(context_->js_context(),s).exec(global_object_);
        }

        runtime::pointer runtime_;
        context::pointer context_;
        JSObject  * global_object_;
        boost::shared_ptr<ErrorReporterPolicy> error_reporter;
};

}} // end of namespace ape, y60

#endif // Y60_APE_JS_ENGINE_INCLUDED
