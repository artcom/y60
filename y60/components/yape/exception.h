#ifndef Y60_APE_EXCEPTION_INCLUDED
#define Y60_APE_EXCEPTION_INCLUDED

#include "y60_ape_settings.h"

#include <asl/base/Exception.h>

namespace y60 { namespace ape {

DEFINE_EXCEPTION( script_error, asl::Exception );
DEFINE_EXCEPTION( bad_arguments, script_error);

DEFINE_EXCEPTION( internal_error, asl::Exception);
DEFINE_EXCEPTION( internal_monkey_error, internal_error );

}} // end of namespace ape, y60

#endif // Y60_APE_EXCEPTION_INCLUDED
