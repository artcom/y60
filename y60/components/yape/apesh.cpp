#include "y60_ape_settings.h"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#ifdef Y60_APE_HAVE_READLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif

#include <asl/base/file_functions.h>
#include <asl/base/buildinfo.h>

#include <y60/components/yape/js_engine.h>

// Undo Apple namespace pollution
#ifdef check
#   undef check
#endif

std::string program_name;

struct handled_options {
    handled_options() :
        interactive(false),
        expression(false),
        strict(false),
        werror(false){}

    std::string main;
    std::vector<std::string> include_dirs;
    std::vector<std::string> arguments;
    bool interactive;
    bool expression;
    bool strict;
    bool werror;
};

enum exit_code {
    exit_ok = EXIT_SUCCESS,
exit_error_begin = 64,
    exit_argument_error = exit_error_begin,
    exit_file_not_found,
    exit_compilation_error,
    exit_runtime_error,
exit_error_end,
    do_not_exit,
};

std::ostream &
operator<<(std::ostream & os, exit_code c) {
    switch (c) {
        case exit_ok: os << "success"; break;
        case exit_argument_error: os << "argument error"; break;
        case exit_file_not_found: os << "file not found"; break;
        case exit_runtime_error: os << "runtime error"; break;
        case exit_compilation_error: os << "compilation error"; break;

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
#ifdef Y60_APE_HAVE_READLINE
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
interactive_session(y60::ape::js_engine & js) {
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
                    std::cout << result.to_string() << std::endl;
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
    js_engine js;
    js.strict(opts.strict);
    js.werror(opts.werror);

    if (opts.interactive) {
        std::cout 
            << "==="  << std::endl
            << "=== " << program_name << " interactive session" << std::endl
            << "==="  << std::endl;
    }

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
        ("include-path,I",  po::value<vector<string> >(), "add arg as an "
                                        "additional include directory")
        ("exec,e", po::value<string>(), "execute arg as a javascript "
                                        "expression and exit")
        ("jsfile", po::value<string>(), "run file and enter interactive mode")
        ("strict",                      "warn on dubious practice")
        ("werror",                      "convert warnings to errors")
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

    if ( options.count("include-path")) {
        opts.include_dirs = options["include-path"].
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
    if (! opts.main.empty() && ! opts.expression) {
        if ( ! boost::filesystem::exists(opts.main)) {
            cerr << opts.main << ": file not found" << endl;
            return exit_file_not_found;
        }
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
    } catch (y60::ape::compilation_error const& ex) {
        result = exit_compilation_error;
    } catch (y60::ape::runtime_error const& ex) {
        result = exit_runtime_error;
    }
    return result;
}
