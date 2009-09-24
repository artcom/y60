#include "y60_ajs_settings.h"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#ifdef Y60_AJS_HAVE_READLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif

#include <asl/base/file_functions.h>
#include <asl/base/buildinfo.h>

#include <y60/components/yape/yape.h>
#include <y60/components/yape/module_loader.h>

#include "custom_globals.h"

// Undo Apple namespace pollution
#ifdef check
#   undef check
#endif

// Undo GNU namespace pollution
#ifdef major
#   undef major
#endif
#ifdef minor
#   undef minor
#endif

struct version {
    version(unsigned maj, unsigned min, unsigned pat,
            const char * xtra = 0) :
            major(maj), minor(min), patch(pat), extra(xtra) {}
    const unsigned major;
    const unsigned minor;
    const unsigned patch;
    const char * const extra;
};

inline
std::ostream &
operator<<(std::ostream & os, version const& v) {
    os << v.major << "." << v.minor << "." << v.patch;
    if ( ! std::string(v.extra).empty() ) {
        os << "-" << v.extra;
    }
    return os;
}

std::string program_name;

version ajs_version(Y60_AJS_VERSION_MAJOR, Y60_AJS_VERSION_MINOR,
        Y60_AJS_VERSION_PATCH, Y60_AJS_VERSION_EXTRA);


class custom_global_factory : public y60::ape::standard_global_factory {
    public:
        custom_global_factory(std::vector<std::string> const& include_dirs,
                std::vector<std::string> const& arguments) :
            include_dirs_(include_dirs), arguments_(arguments)
        {}

        virtual
        JSObject *
        create(JSContext * cx) const {
            using namespace y60::ape;

            JSObject * global = standard_global_factory::create(cx);

            Y60_APE_IMPORT_MODULE(cx, global, custom_globals );
            using y60::ape::module_loader_binding;
            Y60_APE_IMPORT_MODULE(cx, global, module_loader );

            to_js_converter<std::vector<std::string> > conv(cx);

            Object obj(cx, global);
            obj[properties::arguments]    = conv(arguments_);
            obj[prop_include_path] = conv(include_dirs_);

            return global;
        }
    private:
        std::vector<std::string> include_dirs_;
        std::vector<std::string> arguments_;
};

typedef y60::ape::js_engine<custom_global_factory> jse;

struct handled_options {
    handled_options() :
        interactive(false),
        expression(false),
        strict(false),
        werror(false),
        rt_size(jse::default_runtime_size),
        sc_size(jse::default_stack_chunk_size){}

    std::string main;
    std::vector<std::string> include_dirs;
    std::vector<std::string> arguments;
    bool interactive;
    bool expression;
    bool strict;
    bool werror;
    int  rt_size;
    int  sc_size;
};

enum exit_code {
    exit_ok = EXIT_SUCCESS,
exit_error_begin = 64,
    exit_js_error = exit_error_begin,
    exit_file_not_found,
    exit_argument_error,
    exit_other_error,
exit_error_end,
    do_not_exit,
};

std::ostream &
operator<<(std::ostream & os, exit_code c) {
    switch (c) {
        case exit_ok: os << "success"; break;
        case exit_argument_error: os << "argument error"; break;
        case exit_file_not_found: os << "file not found"; break;
        case exit_js_error: os << "javascript error"; break;
        case exit_other_error: os << "other error"; break;

        case exit_error_end: break;
        case do_not_exit: break;
    };
    return os;
}

int
extract_result(y60::ape::value const& v) {
    y60::ape::extract<int> x(v);
    if ( x.check() ) {
        return x();
    } else {
        y60::ape::extract<bool> x(v);
        if (x.check()) {
            if (x()) {
                return EXIT_SUCCESS;
            } else {
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_FAILURE;
}

bool
get_line(const char * prompt, std::string & buffer) {
#ifdef Y60_AJS_HAVE_READLINE
    char * line;
    if ((line = readline(prompt)) == NULL) {
        return false;
    }
    if (line[0] != '\0') {
        add_history(line);
    }
    buffer += line;
    buffer += '\n';
    return true;
#else
    std::cout << prompt;
    std::getline(std::cin, buffer);
    buffer += '\n';
    return ! std::cin.eof();
#endif
}

int
interactive_session(jse & js) {
    bool hit_eof = false;
    bool got_quit = false;
    int line_number = 1;
    int startline;
    do {
        std::string buffer;
        startline = line_number;
        do {
            std::string line;
            if ( ! get_line(startline == line_number ? "js> " : "", line)) {
                hit_eof = true;
                break;
            }
            if ( ! line.empty() ) {
                line_number++;
                buffer += line;
            }
        } while ( ! js.is_compilable_unit( buffer ) );

        if ( ! buffer.empty() ) {
            js.clear_pending_exception();
            try {
                y60::ape::value result = js.exec(buffer, "console");
                if ( *result.val_ptr() != JSVAL_VOID) {
                    std::cout << result.toString() << std::endl;
                }
            } catch (y60::ape::script_error const&) {
                // silently trap script errors
            }
        }
    } while ( ! hit_eof && ! got_quit );
    std::cout << std::endl;
    return exit_ok;
}

int
execute_javascript(handled_options const& opts)
{
    using namespace y60::ape;
    custom_global_factory g(opts.include_dirs, opts.arguments);
    jse js(g, opts.rt_size, opts.sc_size);
    js.strict(opts.strict);
    js.werror(opts.werror);

    if ( ! opts.main.empty() ) {
        if (opts.interactive) {
            std::cout << "executing '" << opts.main << "'" << std::endl;
        }
        value result = opts.expression ?
            js.exec(opts.main, "cmdline") :
            js.exec_file( opts.main );
        if ( ! opts.interactive ) {
            return extract_result( result );
        }
    }
    if (opts.interactive) {
        return interactive_session(js);
    }
    return EXIT_FAILURE;
}

exit_code
handle_arguments( int argc, char * argv[], handled_options & opts) {
    using std::string;
    using std::vector;
    using std::cout;
    using std::cerr;
    using std::endl;

    namespace po = boost::program_options;
    po::variables_map options;
    po::options_description cmdline_options("Options");
    cmdline_options.add_options()
        ("include-dir,I",  po::value<vector<string> >(), "add arg as an "
                                        "additional include directory")
        ("exec,e", po::value<string>(), "execute arg as a javascript "
                                        "expression and exit")
        ("jsfile", po::value<string>(), "run file and enter interactive mode")
#ifdef Y60_AJS_DEBUGGER
        ("debug",                       "run javascript debugger")
#endif
        ("strict",                      "warn on dubious practice")
        ("werror",                      "convert warnings to errors")
        ("rt-size",
         po::value<int>()->default_value(jse::default_runtime_size),
                                        "initial size of the runtime in MB")
        ("sc-size",
         po::value<int>()->default_value(jse::default_stack_chunk_size),
                                        "stack chunk size in bytes (advanced)")
        ("jshelp",                      "print this text and exit")
        ("buildinfo", po::value<string>()->implicit_value(program_name),
                                        "print details about this build and "
                                        "exit")
        ;
    po::options_description hidden("hidden");
    hidden.add_options()
        ("main.js", po::value<string>(), "")
        ("script-arguments", po::value<vector<string> >(), "")
        ("help",   "application pass through")
        ;
    po::positional_options_description p;
    p.add("main.js", 1).add("script-arguments", -1);

    po::options_description all_options;
    all_options.add( cmdline_options ).add( hidden );

    po::parsed_options parsed = po::command_line_parser(argc, argv)
            . options(all_options)
            . positional(p)
            . allow_unregistered()
            . run()
            ;
    po::store( parsed, options );
    po::notify( options );

    if ( options.count("jshelp") ) {
        cout << "Usage: " << program_name
             << " [options] [main.js [script-arguments]]" <<        endl
             << cmdline_options
             <<                                                     endl
             << "Enters interactive mode if no main.js is given" << endl
             << "All unknown options are passed to the script" <<   endl
             <<                                                     endl
             << "Exit Codes:";
             for (int i = exit_error_begin; i != exit_error_end; ++i) {
                if ( (i - exit_error_begin) % 2 == 0) {
                    cout << endl;
                }
                cout << "   " << i << ": " << exit_code(i);
             }
             cout << endl;
        return exit_ok;
    }
    if ( options.count("buildinfo") ) {
        string component = options["buildinfo"].as<string>();
        if (component == "all") {
            cout << asl::build_information::get();
            return exit_ok;
        }
        asl::build_information::const_iterator it =
            asl::build_information::get().find(component);
        if (it == asl::build_information::get().end()) {
            std::cerr << "Unknown component '" << component << "'" << endl
                << "Try '" << program_name
                << " --buildinfo all'" << endl;
            return exit_argument_error;
        }
        cout << it->second
            << endl
            << "Try '" << program_name
            << " --buildinfo=all' for a list of all components" << endl;
        return exit_ok;
    }

    if ( options.count("main.js") ) {
        opts.main = options["main.js"].as<string>();
    } else {
        opts.interactive = true;
    }

    if ( options.count("file") ) {
        opts.main = options["file"].as<string>();
    }

    if ( options.count("include-dir")) {
        opts.include_dirs = options["include-dir"].
            as<vector<string> >();
    }

    opts.arguments = po::collect_unrecognized(parsed.options,
            po::exclude_positional);

    if ( options.count("script-arguments") ) {
        vector<string> more_args =
            options["script-arguments"].as<vector<string> >();
        opts.arguments.insert(opts.arguments.end(),
                more_args.begin(), more_args.end());
    }

    if ( options.count("help") ) {
        cout << "Try '" << program_name
             << " --jshelp ' for help on the javascript interpreter" << endl;
        opts.arguments.insert(opts.arguments.begin(), "--help");
        opts.interactive = false;
    }

    if ( options.count("exec") ) {
        opts.main = options["exec"].as<string>();
        opts.expression = true;
        opts.interactive = false;
    }
    if ( options.count("jsfile") ) {
        if ( ! opts.main.empty()) {
            cerr << "--jsfile conflicts with argument '"
                << opts.main << "'" << endl;
            return exit_argument_error;
        }
        opts.main = options["jsfile"].as<string>();
        opts.interactive = true;
    }
    if ( options.count("strict") ) {
        opts.strict = true;
    }
    if ( options.count("werror") ) {
        opts.werror = true;
    }
    if ( options.count("rt-size") ) {
        opts.rt_size = options["rt-size"].as<int>();
    }
    if ( options.count("sc-size") ) {
        opts.sc_size = options["sc-size"].as<int>();
    }
    if (! opts.main.empty() && ! opts.expression) {
        if ( ! boost::filesystem::exists(opts.main)) {
            cerr << opts.main << ": file not found" << endl;
            return exit_file_not_found;
        }
    }
    if (opts.interactive) {
        cout << program_name << " " << ajs_version;
        asl::build_information::const_iterator it =
            asl::build_information::get().find(program_name);
        if (it != asl::build_information::get().end()) {
            asl::build_target_info const& info = it->second;
            cout << " (" << info.history_id() << ", " 
                 << info.build_date() << ", "
                 << info.build_time() << ")" << endl
                 << "[" << info.compiler() << " "
                 << info.compiler_version() << " "
                 << info.build_config() << "]";
            if ( std::string::npos == info.repository_id().find("trunk") ) {
                cout << endl << info.repository_id();
            }
        }
        cout << endl;
    }
    return do_not_exit;
}

int main(int argc, char * argv[]) {
    exit_code result = exit_ok;
    try {
        program_name = boost::filesystem::path(argv[0]).filename();
        handled_options opts;
        exit_code result = handle_arguments(argc, argv, opts);
        if (result != do_not_exit) {
            return result;
        }
        return execute_javascript(opts);
    } catch (y60::ape::js_error const& ex) {
        std::cerr << "javascript error" << std::endl;
        result = exit_js_error;
    } catch (boost::program_options::error const& ex) {
        std::cerr << "argument error: " << ex.what() << std::endl;
        result = exit_argument_error;
    } catch (asl::Exception const& ex) {
        std::cerr << "exception: " << ex;
        result = exit_other_error;
    } catch (std::exception const& ex) {
        std::cerr << "exception: " << ex.what() << std::endl;
        result = exit_other_error;
    } catch (...) {
        std::cerr << "unknown exception" << std::endl;
        result = exit_other_error;
    }
    return result;
}

