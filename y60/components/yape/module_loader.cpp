#include "module_loader.h"

#include <boost/filesystem.hpp>
#include <acmake/module_name.h>

#include "error_reporter.h"
#include "runtime.h"
#include "script.h"
#include "js_value.h"

namespace y60 { namespace ape {

namespace fs = boost::filesystem;

// XXX does not work properly ... investigate
struct load_error_reporter :
    error_reporter<load_error_reporter>
{
    load_error_reporter(JSContext * cx) :
        y60::ape::error_reporter<load_error_reporter>(cx) {}

    static
    void
    report_error(JSContext * cx, const char * msg, JSErrorReport * report) {
        std::cout << "load_error_reporter" << std::endl;
        if ( ! report) {
            std::cout << msg << std::endl;
            return;
        }
        if (JSREPORT_IS_EXCEPTION(report->flags)) {
            std::cout << "intentionaly ignoring exception" << std::endl;
            return;
        }

        previous_(cx, msg, report);

    }
};

static
JSBool
load_library(JSContext * cx, JSObject * ns, fs::path const& file,
        std::string const& module_name)
{
    std::string const& f = file.file_string();
    dynamic_loader::pointer module(
            new dynamic_loader(f.c_str()));
    std::string func = module_name + "_init_ape_module";
    y60::ape::init_function mod_init =
        module->load_function<y60::ape::init_function>(func.c_str());

    jslib::IJSModuleLoader * ml = mod_init(NULL);
    if ( ! ml ) {
        throw y60::ape::ape_error("failed to init module", PLUS_FILE_LINE);
    }
    ml->initClasses(cx, ns);
    get_ape_runtime( cx )->register_module( module );
    return JS_TRUE;
}

static
void
load_javascript(JSContext * cx, JSObject * ns, fs::path const& file) {
    try {
        load_error_reporter reporter(cx);
        script s(cx, JS_CompileFile(cx, ns, file.file_string().c_str()));
        s.exec( ns );
    } catch (js_error const&) {
        //std::cout << "got javascript error" << std::endl;
        throw exception_pending("",PLUS_FILE_LINE);
    }

}

bool
find_library(fs::path const& p, std::string & result) {
    if ( ! p.empty() ) {
        std::string mod_name = acmake::module_name( (--p.end())->c_str() );
        if ( fs::exists( p / mod_name )) {
            result = mod_name;
            return true;
        }
    }
    return false;
}

JSObject *
get_namespace(JSContext * cx, JSObject * global, fs::path const& path) {
    JSObject * current = global;
    for (fs::path::const_iterator it = path.begin(); it != path.end(); ++it) {
        jsval val;
        if ( ! JS_GetProperty(cx, current, it->c_str(), & val)) {
            throw monkey_error("JS_GetProperty failed",PLUS_FILE_LINE);
        }
        if ( JSVAL_VOID == val ) {
            current = JS_DefineObject(cx, current, it->c_str(), NULL, NULL,
                    JSPROP_ENUMERATE);
        } else {
            if ( ! JSVAL_IS_OBJECT( val )) {
                throw js_error("",PLUS_FILE_LINE);
            }
            current = JSVAL_TO_OBJECT( val );
        }
    }
    return current;
};

const char * const init_filename = "__init__.js";

//static bool load(Object & global, arg_array & arguments, value & rval)
static
JSBool
load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        std::string raw_name = value(cx, argv[0]).toString();
        if (raw_name.empty()) {
            JS_ReportError(cx, "load() called with empty string");
            return JS_FALSE;
        }
        if ( fs::exists(raw_name)) {
            load_javascript(cx, obj, raw_name);
        } else {
            std::string module_path_str(raw_name);
            std::replace( module_path_str.begin(), module_path_str.end(), '.', '/' );
            fs::path module_path( module_path_str );
            if ( ! module_path.empty() ) {
                Array include_path( Object(cx, obj)[prop_include_path] );
                for (jsuint i = 0; i < include_path.length(); ++i) {
                    fs::path dir = include_path[i].toString();
                    if ( ! fs::exists(dir)) {
                        std::cout << "WARNING: include dir '" << dir
                            << "' does not exist" << std::endl;
                    } else {
                        if (fs::is_directory( dir / module_path )) {
                            std::string lib_name;
                            if( find_library( dir / module_path, lib_name )) {
                                JSObject * ns = get_namespace(cx, obj, module_path);
                                load_library(cx, ns, dir / module_path / lib_name,
                                    *--module_path.end());
                            }
                            if ( fs::exists( dir / module_path / init_filename)) {
                                JSObject * ns = get_namespace(cx, obj, module_path);
                                load_javascript(cx, ns, dir / module_path / init_filename);
                            }
                            break;
                        }
                    }
                }
            } else {
                std::cout << "empty module path" << std::endl;
            }
        }
    } Y60_APE_CATCH_BLOCKS;
    return JS_TRUE;
}

Y60_APE_MODULE_IMPL( module_loader ) {
    Y60_APE_NS_SCOPE()
        . function("load",  load,  1)
        ;
}

}} // end of namespace ape, y60
