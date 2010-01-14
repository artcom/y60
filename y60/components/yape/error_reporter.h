#ifndef Y60_APE_ERROR_REPORTER_INCLUDED
#define Y60_APE_ERROR_REPORTER_INCLUDED

#include "y60_ape_settings.h"

#include <iostream>
#include <boost/shared_ptr.hpp>

#include Y60_APE_MONKEY_HEADER(jsstddef.h)

namespace y60 { namespace ape {

template <typename Dervied>
class error_reporter {
    public:
        typedef boost::shared_ptr<Dervied> pointer;
    protected:
        error_reporter(JSContext * cx) : cx_(cx) {
            previous_ = JS_SetErrorReporter(cx_, & Dervied::report_error );
        }
        ~error_reporter() {
            JS_SetErrorReporter(cx_, previous_);
        }
        JSContext * cx_;
        static JSErrorReporter previous_;
};

template <typename Dervied>
JSErrorReporter error_reporter<Dervied>::previous_ = 0;

struct default_error_reporter :
    public error_reporter<default_error_reporter>
{
    default_error_reporter(JSContext * cx ) :
        error_reporter<default_error_reporter>(cx) {}

    static
    void
    report_error(JSContext * cx, const char * message, JSErrorReport * report) {
//        std::cout << "default_error_reporter" << std::endl;
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

}} // end of namespace ape, y60
#endif // Y60_APE_ERROR_REPORTER_INCLUDED
