#ifndef Y60_APE_TYPE_LIST_INCLUDED
#define Y60_APE_TYPE_LIST_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#define Y60_APE_LIST_SIZE Y60_APE_MAX_ARITY

/* Include the smallest mpl::vector that satisfies Y60_APE_MAX_ARITY. The
 * offset of two between vector size and arity is because we need slots
 * for the return type and possibly for the class the function is a member of.
 */
#if Y60_APE_LIST_SIZE >  48
#   error Arities above 48 are not supported y60 ape. You might want to \
          rethink your design anyway.
#elif Y60_APE_LIST_SIZE >  38
#   include <boost/mpl/vector/vector50.hpp>
#elif Y60_APE_LIST_SIZE >  28
#   include <boost/mpl/vector/vector40.hpp>
#elif Y60_APE_LIST_SIZE >  18
#   include <boost/mpl/vector/vector30.hpp>
#elif Y60_APE_LIST_SIZE >  8
#   include <boost/mpl/vector/vector20.hpp>
#else
#   include <boost/mpl/vector/vector10.hpp>
#endif

#endif // Y60_APE_TYPE_LIST_INCLUDED
