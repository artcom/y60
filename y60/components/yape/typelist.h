#ifndef Y60_APE_TYPE_LIST_INCLUDED
#define Y60_APE_TYPE_LIST_INCLUDED

#include "y60_ape_settings.h"

#define Y60_APE_LIST_SIZE Y60_APE_MAX_ARITY

#if Y60_APE_LIST_SIZE >  49
#   error Arities above 49 are not supported y60 ape. You might want to \
          rethink your design anyway.
#elif Y60_APE_LIST_SIZE >  39 
#   include <boost/mpl/vector/vector50.hpp>
#elif Y60_APE_LIST_SIZE >  29 
#   include <boost/mpl/vector/vector40.hpp>
#elif Y60_APE_LIST_SIZE >  19 
#   include <boost/mpl/vector/vector30.hpp>
#elif Y60_APE_LIST_SIZE >  9 
#   include <boost/mpl/vector/vector20.hpp>
#else
#   include <boost/mpl/vector/vector10.hpp>
#endif

#endif // Y60_APE_TYPE_LIST_INCLUDED
