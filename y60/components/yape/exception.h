#ifndef Y60_APE_EXCEPTION_INCLUDED
#define Y60_APE_EXCEPTION_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>

namespace y60 { namespace ape {

DEFINE_EXCEPTION( script_error, asl::Exception );
DEFINE_EXCEPTION( bad_arguments, script_error);
DEFINE_EXCEPTION( bad_call, script_error);
DEFINE_EXCEPTION( compilation_error, script_error);
DEFINE_EXCEPTION( runtime_error, script_error);

DEFINE_EXCEPTION( internal_error, asl::Exception);
DEFINE_EXCEPTION( monkey_error, internal_error );
DEFINE_EXCEPTION( ape_error, internal_error );

// XXX currently only a copy of the stuff in JScppUtils.h
//     need real exception translation
#define Y60_APE_CATCH_BLOCKS \
      catch (asl::Exception & ex) {\
        JSRuntime * myRuntime = JS_GetRuntime(cx); (void) myRuntime;\
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (std::exception & ex) {\
        JSRuntime * myRuntime = JS_GetRuntime(cx); (void) myRuntime;\
        JS_ReportError(cx,"%s", ex.what());\
        return JS_FALSE;\
    } catch (...) {\
        JSRuntime * myRuntime = JS_GetRuntime(cx); (void) myRuntime;\
        JS_ReportError(cx,"Unknown Exception caught");\
        return JS_FALSE;\
    }

}} // end of namespace ape, y60

#endif // Y60_APE_EXCEPTION_INCLUDED
