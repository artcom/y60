/*
 * param.h
 */
/**
\ingroup clish
\defgroup clish_param param
@{

\brief This class represents an instance of a parameter type.

Parameter instances are assocated with a command line and used to validate the
the arguments which a user is inputing for a command.

*/
#ifndef _clish_param_h
#define _clish_param_h

typedef struct clish_param_s clish_param_t;

#include "clish/ptype.h"

/*=====================================
 * PARAM INTERFACE
 *===================================== */
/*-----------------
 * meta functions
 *----------------- */
clish_param_t *
    clish_param_new(const char    *name,
                    const char    *text,
                    clish_ptype_t *ptype);
/*-----------------
 * methods
 *----------------- */
void
    clish_param_delete(clish_param_t *instance);
void
    clish_param_help(const clish_param_t *instance,
                     size_t               offset);
char * 
    clish_param_validate(const clish_param_t *instance,
                         const char          *text);
void
    clish_param_dump(const clish_param_t *instance);
/*-----------------
 * attributes
 *----------------- */
const char *
    clish_param__get_name(const clish_param_t *instance);
const char *
    clish_param__get_text(const clish_param_t *instance);
const char *
    clish_param__get_range(const clish_param_t *instance);
const char *
    clish_param__get_prefix(const clish_param_t *instance);
const char *
    clish_param__get_default(const clish_param_t *instance);
clish_ptype_t *
    clish_param__get_ptype(const clish_param_t *instance);
void
    clish_param__set_prefix(clish_param_t *instance,
                            const char    *prefix);
void
    clish_param__set_default(clish_param_t *instance,
                             const char    *defval);

#endif /* _clish_param_h */
/** @} clish_param */
