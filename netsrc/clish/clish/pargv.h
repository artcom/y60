/*
 * pargv.h
 */
 /**
\ingroup clish
\defgroup clish_pargv pargv
@{

\brief This class represents a vector of command line arguments.

*/

#ifndef _clish_pargv_h
#define _clish_pargv_h

typedef enum 
{
	clish_LINE_OK, 
	clish_BAD_CMD, 
	clish_BAD_PARAM,
	clish_BAD_HISTORY
} clish_pargv_status_t;

typedef struct clish_pargv_s clish_pargv_t;
typedef struct clish_parg_s  clish_parg_t;

#include "clish/ptype.h"
#include "clish/command.h"

/*=====================================
 * PARGV INTERFACE
 *===================================== */
/*-----------------
 * meta functions
 *----------------- */
clish_pargv_t *
		clish_pargv_new(const clish_command_t *cmd,
		                const char            *line,
		                size_t                 offset,
		                clish_pargv_status_t  *status);
/*-----------------
 * methods
 *----------------- */
void
		clish_pargv_delete(clish_pargv_t *instance);
const clish_parg_t *
		clish_pargv_find_arg(clish_pargv_t *instance,
 				     const char    *name);
void
		clish_pargv_dump(const clish_pargv_t *instance);
/*-----------------
 * attributes 
 *----------------- */

/*=====================================
 * PARG INTERFACE
 *===================================== */
/*-----------------
 * meta functions
 *----------------- */
/*-----------------
 * methods
 *----------------- */
void
		clish_parg_dump(const clish_parg_t *instance);
/*-----------------
 * attributes 
 *----------------- */
const char *
		clish_parg__get_name(const clish_parg_t *instance);
const char *
		clish_parg__get_value(const clish_parg_t *instance);
const clish_ptype_t *
		clish_parg__get_ptype(const clish_parg_t *instance);


#endif /* _clish_pargv_h */
/** @} clish_pargv */
