#ifndef Y60_APE_LOGGER_SETTINGS_INCLUDED
#define Y60_APE_LOGGER_SETTINGS_INCLUDED

#include "y60_ape_settings.h"

#ifdef Y60_APE_HAVE_TEMPLOG

#include <templog/logging.h>

namespace y60 { namespace ape {
    
namespace log {
    using namespace templog;
    enum ape_severities {
        dbg = sev_debug,
        inf = sev_info,
        msg = sev_message,
        wrn = sev_warning,
        err = sev_error,
        ftl = sev_fatal
    };
#if 0 // would love to get my templog branch merged
    enum ape_audiences {
        ape_dev,   // for ape developers
        mod_dev,   // for module developers
        user       // for users of modules (js hackers)
    };
#endif
    enum ape_audiences {
        dev = aud_developer,   // for ape developers
        usr = aud_user         // for users of the library
    };
    //=== Logger Hierarchy ====================================================
    typedef non_filtering_logger<
            sev_aud_formatting_policy,
            stderr_write_policy>
                                                        root;

    typedef logger<root,
            dbg,
            audience_list<dev,usr> >  
                                                            ape;
    typedef logger<ape,
            dbg,
            audience_list<dev,usr> >  
                                                                module;
    typedef logger<module,
            msg,
            audience_list<dev,usr> >  
                                                                    import;

}}} // end of namespace log, ape, y60

    
#define APE_LOG( logger, sev, aud ) TEMPLOG_LOG( logger, sev, aud )

#else // Y60_APE_HAVE_TEMPLOG

#   include <asl/base/Logger.h>
#   define APE_LOG( logger, sev, aud ) AC_TRACE

#endif // Y60_APE_HAVE_TEMPLOG

#endif // Y60_APE_LOGGER_SETTINGS_INCLUDED
