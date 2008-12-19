/*
 * param.h
 */
#include "clish/param.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct clish_param_s
{
    char          *name;
    char          *text;
    clish_ptype_t *ptype;  /* The type of this parameter */ 
    char          *prefix; /* if non NULL the name of the identifying switch parameter */
    char          *defval; /* default value to use for this parameter */
};
